#include <Arduino.h>
#include <Wire.h>
#include <Arduino_LSM6DS3.h>
#include <string>
#include "NeuralNetwork.h"
#include "model_data.h"
#include "EEPROM.h"

#define EEPROM_SIZE 9000

void manualWriteModel();

NeuralNetwork *nn;
int mode = 0;
int modePin = 4;

uint8_t modelLowByte;
uint8_t modelHighByte;
int modelSize;
alignas(16) unsigned char *memoryModel;
char * test;

// TwoWire i2c = TwoWire(0);
// LSM6DS3Class myIMU(i2c, 0x6A);

void setup() {
  // Instanciate IC2 communication with IMU. GPIO 21 and 22 for SDA and SCL respectively
  Wire.begin();
  Serial.begin(115200);

  // Initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // Initialize EEPROM with defined size
  EEPROM.begin(EEPROM_SIZE);

  // Manual switch for swicthing modes
  pinMode(4, INPUT);

  manualWriteModel();
  nn = new NeuralNetwork();
}

void loop() {
  // Read in the current switch value to determine mode
  mode = digitalRead(modePin);

  if(!mode) {

    // IMU/IR data
    float ir_dist = 7;
    float acc_x, acc_y, acc_z, rot_x, rot_y, rot_z;

    // If IMU is available read sensors and then make a prediction
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(acc_x, acc_y, acc_z);
      IMU.readGyroscope(rot_x, rot_y, rot_z);
      
      //Serial.print((String) acc_x + '\t' + acc_y + "\t" + acc_z + "\t" + rot_x + "\t" + rot_y + "\t" + rot_z  + "\n");

      nn->getInputBuffer()[0] = acc_x;
      nn->getInputBuffer()[1] = acc_y;
      nn->getInputBuffer()[2] = acc_z;
      nn->getInputBuffer()[3] = rot_x;
      nn->getInputBuffer()[4] = rot_y;
      nn->getInputBuffer()[5] = rot_z;
      nn->getInputBuffer()[6] = ir_dist;

      float *result = nn->predict();
      std::vector<float> resultVector = {result[0], result[1],result[2],result[3],result[4],result[5]};

      int index = std::distance(resultVector.begin(),std::max_element(resultVector.begin(), resultVector.end()));

      char action[50];
      switch(index) {
        case 0:
          strcpy(action, "BicepCurls");
          break;
        case 1:
          strcpy(action, "Posture");
          break;
        case 2:
          strcpy(action, "SideLunges");
          break;
        case 3:
          strcpy(action, "Sitting");
          break;
        case 4:
          strcpy(action, "Squats");
          break;
        case 5:
          strcpy(action, "Standing");
          break;
        default:
          strcpy(action, "Not Found");
      }
      Serial.println((String)"Action: " + action + " Index: " + index + " Value: " + resultVector[index] + " Mode: " + mode);
      //Serial.printf("action %s, index %d, resultVector: %.2f, Mode %d,\n", action, index, resultVector[2], mode);
    }
    delay(10);
  }

  else {
    //Write new model from device
  }
}

void manualWriteModel() {
  Serial.println("Writing");
  modelLowByte = converted_model_tflite_len & 0xFF;
  modelHighByte = (converted_model_tflite_len >> 8) & 0xFF;
  EEPROM.write(3000, modelLowByte);
  EEPROM.write(3001, modelHighByte);

  for(int i  = 0; i < converted_model_tflite_len; i++){
      EEPROM.write(i+3002, converted_model_tflite[i]);
  }
}
