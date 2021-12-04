#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM6DS3.h>
#include <string>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "filesystems.h"
#include "NeuralNetwork.h"
#include "model_data.h"

#define FORMAT_SPIFFS_IF_FAILED true

void spiffManualWriteDefualtModel();
void spiffWriteNewModel(char[], int, std::string);
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

/* 
Credits to Nuno Santos's blog for websocket help. See article below:
 https://techtutorialsx.com/2018/09/13/esp32-arduino-web-server-receiving-data-from-javascript-websocket-client/ for websocket help 
 */

// WiFi and Websocket variables
const char* ssid = "Actiontec-7830-2.4G";
const char* password =  "42389e65";
AsyncWebServer server(80);
AsyncWebSocket ws("/websocket");

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

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
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

// Writes a new model with parameters
void spiffWriteNewModel(char  newModel[], int newModelLen, std::string path){
	bool modelExists = SPIFFS.exists(path.c_str());
	if(modelExists){
		Serial.println((String)"Deleting old " + path.c_str() + " model");
		SPIFFS.remove(path.c_str());
	}
	Serial.println((String)"Writing new model to " + path.c_str());
	File file =  SPIFFS.open(path.c_str(), FILE_WRITE); 
	if(!file){
		Serial.println("Failed to open file for writing");
		return;
	}
	for(int i  = 0; i < newModelLen; i++){
		file.print(newModel[i]);
	}		
	file.close();
}

/*
Don't need to write the default model to memory because it already exists (model.cpp). This function was used to test the SPIFF filesystem
and see if writing and reading a model works as intended. This was necessary because there was no functionality to get a user model
to the device to write to memory and read from.
*/
// Writes the defualt model to the ESP32. If a model already exists it will just return.
void spiffManualWriteDefualtModel(){
	//SPIFFS.remove("/model.txt");
	bool modelExists = SPIFFS.exists("/model.txt");
	if(!modelExists){
		Serial.println("File does not exist. Writing default model to /model.txt");
		File file =  SPIFFS.open("/model.txt", FILE_WRITE); 
		if(!file){
			Serial.println("Failed to open file for writing");
			return;
		}
		for(int i  = 0; i < converted_model_tflite_len; i++){
			file.print(converted_model_tflite[i]);
		}		
		file.close();
	}
	else{
		Serial.println("Default model already exists, skipping step");
	}
}

// Handles websocket events
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){
		Serial.println("Websocket client connection received");
	}
	else if(type == WS_EVT_DISCONNECT){
		Serial.println("Client disconnected");
		Serial.println("-----------------------");
	} 
	else if(type == WS_EVT_DATA){
		//data packet
		AwsFrameInfo * info = (AwsFrameInfo*)arg;

		char new_model_tflite[info->len];
		int new_model_tflite_len = info->len;

		if(info->final && info->index == 0 && info->len == len){
			//the whole message is in a single frame and we got all of it's data
			printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
			if(info->opcode == WS_TEXT){
				data[len] = 0;
				printf("%s\n", (char*)data);
			} else {
				for(size_t i=0; i < info->len; i++){
					printf("%02x ", data[i]);
				}
				printf("\n");
			}
			if(info->opcode == WS_TEXT)
				client->text("I got your text message");
			else
				client->binary("I got your binary message");
		}
		else {
			//message is comprised of multiple frames or the frame is split into multiple packets
			if(info->index == 0){
				if(info->num == 0)
					printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
				printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
			}

			printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
			if(info->message_opcode == WS_TEXT){
				data[len] = 0;
				printf("%s\n", (char*)data);
			} else {
				for(size_t i=0; i < len; i++){
					printf("%02x ", data[i]);
					new_model_tflite[info->index + i] = (char)data[i];
				}
				printf("\n");
			}

			if((info->index + len) == info->len){
				printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
				if(info->final){
					spiffWriteNewModel(new_model_tflite, new_model_tflite_len, "/userModel.txt");
					printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
					if(info->message_opcode == WS_TEXT)
						client->text("I got your text message");
					else
						client->binary("I got your binary message");
				}
			}
		}
	}
}