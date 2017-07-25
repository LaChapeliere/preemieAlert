/*
 * Temperature Measurement Code
 * author: anneclairebrld
 * sources: https://github.com/sparkfun/SparkFun_MLX90614_Arduino_Library/
 */

#include "SparkFunMLX90614.h" // SparkFunMLX90614 Arduino library
#include <Wire.h>             // I2C library, required for MLX90614

//Thermometer sensor set up
IRTherm thermometer;
float obj_val = 0;
float ambient_val = 0;

void setup() {
  Serial.begin(115200);
  thermometer.begin(0x5B);
  thermometer.setUnit(TEMP_C);
  thermometer.setMax(85.0);
  thermometer.setMin(-20.0);
  Serial.println(F("Temperature Readings"));
}

void loop() {
  if (thermometer.read()){ 
    obj_val = thermometer.object();
    ambient_val = thermometer.ambient();
    Serial.print(F("OBJ= "));
    Serial.print(obj_val);
    Serial.print(F(" ,AMB= "));
    Serial.print(ambient_val);
    Serial.println();
  }
  delay(500);
}
