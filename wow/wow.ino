#include <Arduino.h>
#define USE_SERIAL Serial
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
#include "Adafruit_LEDBackpack.h"



Adafruit_8x16matrix matrix = Adafruit_8x16matrix();

int sensorPin = A0;    // select the input pin for the potentiometer

int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  USE_SERIAL.begin(115200);

  // set up LED matrix and clear any previous display
  matrix.begin(0x70);  
  matrix.clear();
  matrix.writeDisplay();

      USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("Puppet Guest", "argon4949");

}



// draw frowny face
static const uint8_t PROGMEM
frown_bmp[] =
{ B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};



void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  USE_SERIAL.println(sensorValue);
  delay(500);

  //if spoiled food is detected, blink
  if (sensorValue > 400) {
    USE_SERIAL.println("hey eat me");
    matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    matrix.blinkRate(2);
    delay(500);

       if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        //send text about fruit going bad
        http.begin("http://ec2-54-191-196-44.us-west-2.compute.amazonaws.com:3000/"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

  }

  else {
    USE_SERIAL.println("all good");
    matrix.clear();
    matrix.writeDisplay();
  }
}



