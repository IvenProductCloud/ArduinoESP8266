// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek.
/*
This is an example of IvenCloudESP library in which you can activate your device,
create data objects, send data to Iven Product Cloud and handle tasks.
For further explanations about functions in library, hardware setup and REQUIRED manual
settings of ESP8266 check blog: http://blog.iven.io
*/

#include <IvenCloudESP.h>

#define lightPin 0

// Serial communication pins of Arduino for ESP8266 (dont use 0 and 1; because Arduino uses it to communicate with computer)
#define arduino_rx_esp_tx 2
#define arduino_tx_esp_rx 3

int sensorValue = 0;

bool avail;
bool isActive = false;

IvenCloudESP client(arduino_rx_esp_tx, arduino_tx_esp_rx, 9600, true);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(lightPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!isActive) {
    IvenResponse i = client.activateDevice("secret_key", "device_uid");
    Serial.println(i.httpStatus);
    if (i.httpStatus == 200)
      isActive = true;
      Serial.println("api-key is taken");
  } else {
    IvenData data;
    sensorValue = analogRead(lightPin);
    if (sensorValue < 400) 
      avail = true;
    else avail = false;
    data.add("available", avail);
    if (avail)
      data.taskDone(5555);
    IvenResponse p = client.sendData(data);
    Serial.println(p.error);
    Serial.println(p.httpStatus);
    Serial.println(p.ivenCode);
    Serial.println(p.task);
  }
}