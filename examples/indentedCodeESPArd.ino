#include <IvenCloudESP.h>

#define lightPin 0

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
    Serial.println(i.status);
    if (i.status == 200)
      isActive = true;
      Serial.println("api-key is taken");
  } else {
    IvenData data;
    sensorValue = analogRead(lightPin);
    if (sensorValue < 400) 
      avail = true;
    else avail = false;
    data.add("available", avail);
    IvenResponse p = client.sendData(data);
    Serial.println(p.status);
    Serial.println(p.ivenCode);
    Serial.println(p.task);
  }
}