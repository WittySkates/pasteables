#include <Arduino.h>
#include "NeuralNetwork.h"
#include <string>

NeuralNetwork *nn;
int mode = 0;
int modePin = 4;

void setup()
{
  Serial.begin(115200);
  pinMode(4, INPUT);
  nn = new NeuralNetwork();
}

void loop()
{
  mode = digitalRead(modePin);

  if(!mode) {

    //IMU/IR data
    float data[] = {-0.157470703125,0.8681640625,-0.41455078125,1.77001953125,-4.8828125,-2.86865234375, 7};

    float acc_x = data[0];
    float acc_y = data[1];
    float acc_z = data[2];
    float rot_x = data[3];
    float rot_y = data[4];
    float rot_z = data[5];
    float ir_dist = data[6];

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

    delay(1000);
  }
  else {
    //to device
    Serial.printf("Mode %d\n", mode);
    delay(1000);
  }

}