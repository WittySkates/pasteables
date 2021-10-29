#include <Wire.h>
#include <Arduino_LSM6DS3.h>

// TwoWire i2c = TwoWire(0);
// LSM6DS3Class myIMU(i2c, 0x6A);

void setup() {
  Wire.begin();
  Serial.begin(115200);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() { 
  float acc_x, acc_y, acc_z, rot_x, rot_y, rot_z;
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(acc_x, acc_y, acc_z);
    IMU.readGyroscope(rot_x, rot_y, rot_z);
    Serial.print((String) acc_x + '\t' + acc_y + "\t" + acc_z + "\t" + rot_x + "\t" + rot_y + "\t" + rot_z  + "\n");
  }
  delay(10);
}
