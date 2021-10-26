#include <Arduino.h>
#include <Arduino_LSM6DS3.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
 
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
 
    while (1);
  }
 
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println();

  Serial.println("Acceleration in G's \t Gyroscope in degrees/second");
  Serial.println("X\tY\tZ\tX\tY\tZ");
}
 
void loop() {
  float acc_x, acc_y, acc_z, rot_x, rot_y, rot_z;
 
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(acc_x, acc_y, acc_z);
    IMU.readGyroscope(rot_x, rot_y, rot_z);
    Serial.print((String) acc_x + '\t' + acc_y + "\t" + acc_z + "\t" + rot_x + "\t" + rot_y + "\t" + rot_z  + "\n");
  }
  else{
    Serial.println("Waiting");
  }
  delay(10);

}