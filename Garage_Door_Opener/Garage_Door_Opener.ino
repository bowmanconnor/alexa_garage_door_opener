#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
//#include <Wire.h>;
#include <ESP8266HTTPClient.h>

//const int mpu_addr = 0x68;
//int16_t AcX, AcY, AcZ;

#define ssid  "My WiFi Name"
#define ssid_pass "Definitely not 'password'......"
#define SERIAL_BAUDRATE                 115200

#define opener 12
#define sensor 4
boolean last = false;
fauxmoESP fauxmo;

void wifiSetup() {
  WiFi.mode(WIFI_STA);
  Serial.printf("[WIFI] Connecting to %s ", ssid);
  WiFi.begin(ssid, ssid_pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void setup() {
  pinMode(opener, OUTPUT);
  pinMode(sensor, INPUT);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println();
  wifiSetup();

  fauxmo.addDevice("Garage Door");
  fauxmo.enable(true);
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.printf("[MAIN] Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF", value );
    if (state == true && state != last){
      digitalWrite(opener, LOW);
      last = true;
    }
    if (state == false && state != last){
      digitalWrite(opener, LOW);
      last = false;
    }
  }) ;
}

void loop() {
    if (digitalRead(sensor) == HIGH){
      fauxmo.setState("Garage Door", true, 255);
      last = true;
    }
    else{
      fauxmo.setState("Garage Door", false, 0);
      last = false;
    }
    fauxmo.handle();
    delay(500);
    digitalWrite(opener, HIGH);
}
