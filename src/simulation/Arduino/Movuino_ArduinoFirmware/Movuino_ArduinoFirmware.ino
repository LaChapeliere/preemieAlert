/*
    This sketch is a Movuino firmware.
    It allows the Movuino to send data on a specific Wifi through an OSC protocol. (Open Sound Control)
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <OSCBundle.h>
#include <OSCMessage.h>
#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>
#include <Adafruit_NeoPixel.h>

// Set your wifi network configuration here
char * ssid = "movuino-02";                     // your network SSID (name of the wifi network)
char * pass = "z12345678";                     // your network password
char * hostIP =  "192.168.0.100";       // IP address of the host computer
const unsigned int portOut = 7400;               // port on which data are sent (send OSC message)
const unsigned int portIn = 7401;                // local port to listen for UDP packets (receive OSC message)
char movuinoIP[4];

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;
int packetNumber = 0;

// Button variables
const int pinBtn = 13;     // the number of the pushbutton pin
boolean isBtn = 0;         // variable for reading the pushbutton status
float pressTime = 500;    // pressure time needed to switch Movuino state
float lastButtonTime;
boolean lockPress = false; // avoid several activation on same button pressure

// LEDs
#define NUMPIXELS 5
#define PINNEOPIXEL 15 //pin for the neopixel strip
const int pinLedWifi = 2; // wifi led indicator
const int pinLedBat = 0;  // battery led indicator
int delayval = 500; // delay for half a second
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PINNEOPIXEL, NEO_GRB + NEO_KHZ800);


// Vibrator
const int pinVibro = 14;  // vibrator pin
boolean isVibro = false; // true when vibrating
int dVibON = 1000; // duration of vibration when vibrator is ON
int dVibOFF = 1000; // delay between vibrations when vibrator is ON
int dVib = dVibON + dVibOFF; // time between 2 vibration cycles
float rVib = dVibON / (float)dVib; // ratio of time on which vibrations are ON
long timerVibro = 0; // time where vibrations start
int nVib = 3; // number of vibration (-1 for infinite)

// Serial variables
int inByte = 0;  // incoming serial byte
char buff[40]; // buff to store incoming bytes
int bufIndex = 0; // current index of the buff
char msgAdr = 'X'; // address of received messages
String msgVal = "X"; // values of received messages

WiFiUDP Udp;
OSCErrorCode error;

void setup() {
   pixels.begin(); // This initializes the NeoPixel library.
   pixels.setPixelColor(0, pixels.Color(0,0,0));
   pixels.show();
  
  // pin setup
  pinMode(pinBtn, INPUT_PULLUP); // pin for the button
  pinMode(pinLedWifi, OUTPUT);   // pin for the wifi led
  pinMode(pinLedBat, OUTPUT);    // pin for the battery led
  pinMode(pinVibro, OUTPUT);    // pin for the vibrator

  Wire.begin();
  Serial.begin(115200);
  delay(10);


  // We start by connecting to a WiFi network
  startWifi();
  
}

void loop() {
  getSerialMsg(); // update msgAdr & msgMsg

  // BUTTON CHECK
  checkButton();

  // MOVUINO DATA
  if (WiFi.status() == WL_CONNECTED) {
    IPAddress myIp = WiFi.localIP();

    
    delay(2);
    if(!digitalRead(pinVibro)){
      // SEND MOVUINO DATA
      OSCMessage msg("/movuinOSC"); // create an OSC message on address "/movuinOSC"
      msg.add(splitFloatDecimal(12.48 / 2.3));   // add arbitrary float as message
      Udp.beginPacket(hostIP, portOut); // send message to computer target with "hostIP" on "port"
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
  
      delay(5);
    }

    // RECEIVE EXTERNAL OSC MESSAGES
    OSCMessage bundle;
    int size = Udp.parsePacket();
    if (size > 0) {
      while (size--) {
        bundle.fill(Udp.read()); // read incoming message into the bundle
      }
      if (!bundle.hasError()) {
        bundle.dispatch("/filter", callbackFilter);
      } else {
        error = bundle.getError();
        Serial.print("error: ");
        Serial.println(error);
      }
    }

    // MANAGE VIBRATIONS
    if (isVibro) {
      vibroPulse();
    }
  }
  else {
    delay(50); // wait more if Movuino is sleeping
  }
}

void callbackFilter(OSCMessage &msg) {
  int pixelNb = msg.getInt(0); // Get the index of the LED to change
  Serial.println(pixelNb);
  // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
  pixels.setPixelColor(pixelNb, pixels.Color(150,0,0)); // Moderately bright green colour.

  pixels.show(); // This sends the updated pixel color to the hardware.

  delay(delayval); // Delay for a period of time (in milliseconds).
  pixels.setPixelColor(pixelNb, pixels.Color(0,0,0)); //  Back to no colour.
  pixels.show(); 
  delay(delayval);
  
}

float splitFloatDecimal(float f_){
  int i_ = f_ * 1000;
  return i_/1000.0f;
}
