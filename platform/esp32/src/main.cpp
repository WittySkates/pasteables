#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM6DS3.h>
#include <string>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "filesystems.h"
#include "NeuralNetwork.h"
#include "websockets.h"
#include "model_data.h"

#define FORMAT_SPIFFS_IF_FAILED true

// WiFi and Websocket variables
const char* ssid = "Actiontec-7830-2.4G";
const char* password =  "42389e65";
AsyncWebServer server(80);
AsyncWebSocket modelWS("/modelWS");
AsyncWebSocket otherWS("/otherWS");

// Neural network object
NeuralNetwork *nn;

int mode = 0;
int modePin = 4;

// Necessary if multiple slave devices are used
// TwoWire i2c = TwoWire(0);
// LSM6DS3Class myIMU(i2c, 0x6A);

void setup() {
	// Instanciate IC2 communication with IMU. GPIO 21 and 22 for SDA and SCL respectively
	Wire.begin();

	Serial.begin(115200);

	// Initialize SPIFFS file system
	if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
		Serial.println("SPIFFS Mount Failed");
		return;
	}

	// Initialize IMU
	if (!IMU.begin()) {
		Serial.println("Failed to initialize IMU!");
		return;
	}

	//SPIFFS.remove("/userModel.txt");

	listDir(SPIFFS, "/", 1);

	// Connect to WiFi and start server
	Serial.println("Connecting to WiFi");
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("WiFi Failed!");
		return;
	}
	Serial.println(WiFi.localIP());

	modelWS.onEvent(onWsEvent);
	server.addHandler(&modelWS);

	otherWS.onEvent(onWsEvent);
	server.addHandler(&otherWS);

	server.on("/html", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/websocket.html", "text/html");
	});
	server.begin();

	// Manual switch for swicthing modes
	pinMode(4, INPUT);

	// Need to build with this uncommented once, then it is saved in memory for future build.
 	//spiffWriteNewModel(converted_model_tflite, converted_model_tflite_len, "/defaultModel.txt");
	 
	// New neural network object
	nn = new NeuralNetwork();
}

void loop() {
	// Read in the current switch value to determine mode
	mode = digitalRead(modePin);

	if(!mode) {

		// IMU/IR data
		float ir_dist = 0;
		float inputs[7]; // acc_x, acc_y, acc_z, rot_x, rot_y, rot_z

		// If IMU is available read sensors and then make a prediction
		if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
			IMU.readAcceleration(inputs[0], inputs[1], inputs[2]);
			IMU.readGyroscope(inputs[3], inputs[4], inputs[5]);
			inputs[6] = ir_dist;

			//Serial.print((String) acc_x + '\t' + acc_y + "\t" + acc_z + "\t" + rot_x + "\t" + rot_y + "\t" + rot_z  + "\n");

			// Put the inputs into the neural network input buffer
			float * inputBuffer = nn->getInputBuffer();
			std::copy(inputs, inputs+7, inputBuffer);

			// Make a prediction with the current values in the input buffer out push to output buffer
			float *result = nn->predict();

			// Store the results from the output buffer
			std::vector<float> resultVector = {result[0], result[1],result[2],result[3],result[4],result[5]};

			// Output class  is the index with the maximum value
			int index = std::distance(resultVector.begin(),std::max_element(resultVector.begin(), resultVector.end()));

			// Convert index to readable value
			std::string action;
			switch(index) {
				case 0:
					action = "BicepCurls";
					break;
				case 1:
					action = "Posture";
					break;
				case 2:
					action = "SideLunges";
					break;
				case 3:
					action = "Sitting";
					break;
				case 4:
					action = "Squats";
					break;
				case 5:
					action = "Standing";
					break;
				default:
					action = "Not Found";
			}
			Serial.println((String)"Action: " + action.c_str() + " Index: " + index + " Value: " + resultVector[index] + " Mode: " + mode);
			//Serial.printf("action %s, index %d, resultVector: %.2f, Mode %d,\n", action, index, resultVector[2], mode);
		}
		delay(10);
	}
	else {
		// Receive model through websocket and save it 

		// Receive model from through serial external device and assign in to values
		// if(Serial.available()){
		// 	String command = Serial.readString();
		// 	unsigned int new_model_tflite_len = std::atoi(command.c_str());
		// 	Serial.println((String) "The new  model has  a length of " + command);

		// 	Serial.flush();

		// 	char new_model_tflite[new_model_tflite_len];
		// 	Serial.readBytes(new_model_tflite, new_model_tflite_len);
		// 	Serial.println((String) "The new  model has  been read ");

		// 	//Writes the new model to memory (userModel)
		// 	spiffWriteNewModel(new_model_tflite, new_model_tflite_len, "/userModel.txt");
		// 	Serial.println((String)"Done writing new model to memory");
		// }
	}
}