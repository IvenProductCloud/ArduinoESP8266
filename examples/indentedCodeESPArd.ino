#include <SoftwareSerial.h>
#include <SoftReset.h>

#define ard_rx_esp_tx 2
#define ard_tx_esp_rx 3
#define lightPin 1

SoftwareSerial ESPserial(ard_rx_esp_tx, ard_tx_esp_rx); // RX | TX

char jSonBuffer[128];
String api_key = "";
char ivenCode[5];

bool isOk();
bool apiParse();
bool ivenCodeParse();

void returnSetup();
void responseHandle();


String jSonBody1 = "{\"data\":[{\"available\":true,\"at\":\"now\"}]}";
String jSonBody2 = "{\"data\":[{\"available\":false,\"at\":\"now\"}]}";

int sensorValue = 0;
int i, j, index, count, arrayIndex;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);     // communication with the host computer
  while (!Serial);

  // Start the software serial for communication with the ESP8266
  ESPserial.begin(9600);
  pinMode( lightPin, INPUT);
  Serial.println("");
  Serial.println(F("Remember to to set Both NL & CR in the serial monitor."));
  Serial.println(F("Ready"));
  Serial.println(F(""));
  Serial.println(F("start"));
  ESPserial.println("AT+CIPCLOSE");
  long startTime = millis();
  while (millis() - startTime < 5000) {
    if (ESPserial.available()) char c = ESPserial.read();
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  if (api_key.length() == 0) {

    ESPserial.println(F("AT+CIPSTART=\"TCP\",\"demo.iven.io\",80"));
    if (!isOk()) returnSetup();

    ESPserial.println(F("AT+CIPSEND=107"));
    if (!isOk()) returnSetup();

    ESPserial.println(F("GET /activate/device HTTP/1.1"));
    ESPserial.println(F("Host: demo.iven.io"));
    ESPserial.println(F("Activation: b353165e8f54e6fbf5cfa4220b38b19a5f74defa"));
    ESPserial.println();
    responseHandle();
    Serial.println(jSonBuffer);

    if (!apiParse()) returnSetup();
    Serial.println(api_key);
  
  } else {

    ESPserial.println("AT+CIPCLOSE");
    long startTime = millis();
    while (millis() - startTime < 3000) {
      if (ESPserial.available()) char c = ESPserial.read();
    }

    ESPserial.println(F("AT+CIPSTART=\"TCP\",\"demo.iven.io\",80"));
    if (!isOk()) returnSetup();

    sensorValue = analogRead(lightPin);

    if (sensorValue < 250) {

      ESPserial.println(F("AT+CIPSEND=257"));
      if (!isOk()) returnSetup();

      ESPserial.println(F("POST /data HTTP/1.1"));
      ESPserial.println(F("Host: demo.iven.io"));
      ESPserial.print(F("Connection: keep-alive\r\n"));
      ESPserial.println(F("Accept-Encoding: gzip, deflate"));
      ESPserial.println(F("Accept: */*"));
      ESPserial.println(F("Content-Type: application/json"));
      ESPserial.print(F("API-KEY: "));
      ESPserial.println(api_key);
      ESPserial.print(F("Content-Length: "));
      ESPserial.println(jSonBody1.length());
      ESPserial.println();
      ESPserial.println(jSonBody1);

    } else {

      ESPserial.println(F("AT+CIPSEND=258"));
      if (!isOk()) returnSetup();

      ESPserial.println(F("POST /data HTTP/1.1"));
      ESPserial.println(F("Host: demo.iven.io"));
      ESPserial.print(F("Connection: keep-alive\r\n"));
      ESPserial.println(F("Accept-Encoding: gzip, deflate"));
      ESPserial.println(F("Accept: */*"));
      ESPserial.println(F("Content-Type: application/json"));
      ESPserial.print(F("API-KEY: "));
      ESPserial.println(api_key);
      ESPserial.print(F("Content-Length: "));
      ESPserial.println(jSonBody2.length());
      ESPserial.println();
      ESPserial.println(jSonBody2);

    }

    responseHandle();
    Serial.println(jSonBuffer);

    if (!ivenCodeParse()) returnSetup();

  }
}

  bool isOk() {

    int readCount = 0;
    long startTime = millis();
    while (millis() - startTime < 4000) { // Run for at least 5 seconds
      // Check to make sure we don't exceed espBuffer's boundaries
      if (ESPserial.available() > readCount + sizeof(jSonBuffer) - 1)
        break;
      readCount += ESPserial.readBytes(jSonBuffer + readCount, ESPserial.available());
    }
    readCount = 0;
    while (jSonBuffer[readCount] != '\0') {
      if (jSonBuffer[readCount] == 'O' && jSonBuffer[readCount + 1] == 'K') {
        Serial.println("is ok");
        for (i = 0; i < 128; i++) {
          jSonBuffer[i] = '\0';
        }
        return true;
      }
      readCount++;
    }
    return false;
  }

  void returnSetup() {
    delay(1000);
    Serial.println("resetting");
    ESPserial.println("AT+RST");
    long startTime = millis();
    while (millis() - startTime < 5000) {
      if (ESPserial.available()) char c = ESPserial.read();
    }
    delay(2000);
    soft_restart();
  }

  void responseHandle() {
    i = 0;
    long startTime = millis();
    while (millis() - startTime < 15000) {
      if (ESPserial.available()) {
        jSonBuffer[i] = ESPserial.read();
        if (i > 1) {
          if (jSonBuffer[i - 2] == '2' && jSonBuffer[i - 1] == '0' && jSonBuffer[i] == '0') {
            i = 0;
            while (!(jSonBuffer[i - 4] == '\r' && jSonBuffer[i - 3] == '\n' && jSonBuffer[i - 2] == '\r' && jSonBuffer[i - 1] == '\n')) {
              if (ESPserial.available()) {
                jSonBuffer[i] = ESPserial.read();
                jSonBuffer[i + 1] = '\0';
                i++;
                i %= 120;
              }
            }
            i = 0;
            jSonBuffer[4] = '\0';
            jSonBuffer[5] = '\0';
            while (i != 4) {
              if (ESPserial.available()) {
                jSonBuffer[i] = ESPserial.read();
                i++;
              }
            }
            Serial.println(jSonBuffer);
            j = strtoul(jSonBuffer, 0, 16);
            i = 0;
            while (i != j) {
              if (ESPserial.available()) {
                jSonBuffer[i] = ESPserial.read();
                i++;
              }
            }
            for (index = j; index < 128; index++) {
              jSonBuffer[j] = '\0';
            }
            break;
          }
        }
        i++;
        i %= 100;
      }
    }
  }

  bool apiParse() {
    i = 0;
    j = 0;
    bool api = false;
    long startTime = millis();
    while (millis() - startTime < 5000) {
      if (jSonBuffer[i] == 'a' && jSonBuffer[i + 1] == 'p' && jSonBuffer[i + 2] == 'i' && jSonBuffer[i + 3] == '_' && jSonBuffer[i + 4] == 'k' && jSonBuffer[i + 5] == 'e' && jSonBuffer[i + 6] == 'y') {
        api = true;
        break;
      }
      i++;
    }

    i += 10;
    j = i;

    while (jSonBuffer[j] != '"') {
      j++;
    }
 
    for (index = i; index < j; index++) {
      api_key += jSonBuffer[index];
    }

    for (i = 0; i<128; i++) {
      jSonBuffer[i] = '\0';
    }
    return api;
  }

  bool ivenCodeParse() {
    i = 0;
    j = 0;
    arrayIndex = 0;
    bool code = false;
    long startTime = millis();
    while (millis() - startTime < 5000) {
      if (jSonBuffer[i] == 'i' && jSonBuffer[i + 1] == 'v' && jSonBuffer[i + 2] == 'e' && jSonBuffer[i + 3] == 'n' && jSonBuffer[i + 4] == 'C' && jSonBuffer[i + 5] == 'o' && jSonBuffer[i + 6] == 'd' && jSonBuffer[i + 7] == 'e') {
        code = true;
        break;
      }
      i++;
    }

    i += 10;
    j = i;

    while (jSonBuffer[j] != '}') {
      j++;
    }
    
    for (index = i; index < j; index++) {
      ivenCode[arrayIndex] = jSonBuffer[index];
      arrayIndex++;
    }
    ivenCode[arrayIndex] = '\0';

    if (ivenCode[0] == '1' && ivenCode[1] == '0' && ivenCode[2] == '0' && ivenCode[3] == '0') {
      count++;
      Serial.print("Iven Code: ");
      Serial.println(ivenCode);
      Serial.print("Succesful count: ");
      Serial.println(count);
    } else {
      Serial.print("Iven Code: ");
      Serial.println(ivenCode);
    }
    
     for (i = 0; i<128; i++) {
      jSonBuffer[i] = '\0';
    }
    return code;
    
  }
