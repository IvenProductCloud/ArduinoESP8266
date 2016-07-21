#include <IvenCloudESP.h>

#define lightPin 1
#define arduino_rx_esp_tx 2
#define arduino_tx_esp_rx 3

int sensorValue = 0;

bool avail;
bool isActive = false;

IvenCloudESP client(arduino_rx_esp_tx, arduino_tx_esp_rx);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("start");
  pinMode(lightPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!isActive) {
    int i = client.activateDevice("device_secret_key", "device_uid");
    Serial.println(i);
    if (i != 3)
      isActive = true;
    Serial.println("activated");
  } else {
    IvenData data;
    sensorValue = analogRead(lightPin);
    if (sensorValue < 250) avail = true;
    else avail = false;
    data.add("available", avail);
    int i = client.sendData(data);
    Serial.println(i);
    Serial.println("end");
    delay(2000);
  }
}