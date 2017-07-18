/*Source :
   modifications by @anneclairebrld
*/

#include<Wire.h>
#include "MAX30105.h" // works with MAX30102

#include "heartRate.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;

//BPM
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

//SPO2
#define MAX_BRIGHTNESS 255
uint16_t irBuffer[50]; //infrared LED sensor data
uint16_t redBuffer[50]; //red LED sensor data
int at = 0;
const byte bufferLength = 50;
int32_t spo2_val;

void setup(){
  Serial.begin(115200);
  // Initialize sensor
  Serial.println(F("Place index"));
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power. "));
    while (1);
  }


  //configure sensor settings
  //byte powerLevel, byte sampleAverage, byte ledMode, int sampleRate, int pulseWidth, int adcRange 
  particleSensor.setup(60, 4, 2, 100, 411, 4096);
  particleSensor.setPulseAmplitudeGreen(0);
}

void loop(){
  calculate_vals();
  Serial.println();
}

void calculate_vals(){
  long irValue = particleSensor.getIR();
  int i = at%bufferLength;
  redBuffer[i] = particleSensor.getRed();
  irBuffer[i] = (uint16_t)irValue;
      
  if (checkForBeat(irValue) == true){
    Serial.println(F("THERE IS  A BEATTTT"));
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20){
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (at > bufferLength){
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2_val);   
  }
  
  if (irValue < 50000)
    Serial.print(F(" No finger?"));
  else {
    print_info(irValue); 
  }
  at++;
}

void print_info(long irValue){
  Serial.print(F("IR="));
  Serial.print(irValue);
  Serial.print(F(", BPM="));
  Serial.print(beatsPerMinute);
  Serial.print(F(", Avg BPM="));
  Serial.print(beatAvg);
  Serial.print(F(", SPO2="));
  Serial.print(spo2_val);
}

