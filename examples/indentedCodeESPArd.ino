#define ard_rx_esp_tx 2
#define ard_tx_esp_rx 3
#define lightPin 1
char response[725];
String api_key = "";
#include <SoftwareSerial.h>
String jSonBody1 = "{\"data\":[{\"available\":true,\"at\":\"now\"}]}";
String jSonBody2 = "{\"data\":[{\"available\":false,\"at\":\"now\"}]}";

int sensorValue = 0;

SoftwareSerial ESPserial(ard_rx_esp_tx, ard_tx_esp_rx); // RX | TX

int i, j;
int count = 0;

void setup()
{

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


  delay(1000);


}
void loop() {
  // put your main code here, to run repeatedly:
  if (api_key.length() == 0) {

    ESPserial.println(F("AT+CIPSTART=\"TCP\",\"demo.iven.io\",80"));
    delay(1000);

    i = 0;
    while (ESPserial.available() ) {
      response[i] = ESPserial.read();
      i++;
    }
    response[i++] = '\0';
    Serial.println(response);
    for (i = 0; i < 725; i++) {
      response[i] = '\0';
    }

    ESPserial.println(F("AT+CIPSEND=107"));
    delay(1000);

    i = 0;
    while (ESPserial.available() ) {
      response[i] = ESPserial.read();
      i++;
    }
    response[i++] = '\0';
    Serial.println(response);
    for (i = 0; i < 725; i++) {
      response[i] = '\0';
    }

    ESPserial.println(F("GET /activate/device HTTP/1.1"));
    ESPserial.println(F("Host: demo.iven.io"));
    ESPserial.println(F("Activation: b353165e8f54e6fbf5cfa4220b38b19a5f74defa"));
    ESPserial.println();


    i = 0;
    while (true) {
      if (ESPserial.available()) {
        response[i] = ESPserial.read();
        i++;
        if (i > 3) {
          if (response[i - 4] == 'I' && response[i - 3] == 'P' && response[i - 2] == 'D' && response[i - 1] == ',') {
            Serial.print("index is: ");
            Serial.println(i);
            i = 0;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            response[i++] = '\0';
            Serial.println(response);
            sscanf(response, "%d", &j);
            Serial.println(j);
            i = 0;
            response[0] = '\0';
          }
        }

        if (i == j) {
          response[i++] = '\0';
          break;
        }
      }
      //    if ( Serial.available() )    {
      //      ESPserial.write( Serial.read() );
      //    }
    }
    Serial.println(F("left loop"));
    response[i++] = '\0';
    Serial.println(F("Server response is: "));
    Serial.println(response);
    if (response[9] == '2' && response[10] == '0' && response[11] == '0')
    {
      Serial.println("parser response: ");
      Serial.println(response);
      delay(1000);
      // read api key in response body
      int index;
      i = 0;
      j = 0;
      bool api = true;
      while (api) {
        if (response[i] == 'a' && response[i + 1] == 'p' && response[i + 2] == 'i' && response[i + 3] == '_' && response[i + 4] == 'k' && response[i + 5] == 'e' && response[i + 6] == 'y') {
          api = false;
        }
        i++;
      }

      i += 9;
      j = i;

      while (response[j] != '"') {
        j++;
      }
      Serial.println(i);
      Serial.println(j);
      Serial.println(response[i]);
      Serial.println(response[j]);
      for (index = i; index < j; index++) {
        api_key += response[index];
      }

    } else {
      Serial.print(F("Something went wrong, server response is: "));
      Serial.println(response);
    }

  } else {

    for (i = 0; i < 725; i++) {
      response[i] = '\0';
    }
    Serial.println(F("done"));
    Serial.print(F("API_KEY: "));
    Serial.println(api_key);
    delay(1000);


    ESPserial.println("AT+CIPCLOSE");
    delay(1000);
    i = 0;
    while (ESPserial.available() ) {
      response[i] = ESPserial.read();
      i++;
    }
    response[i++] = '\0';
    Serial.println(response);
    for (i = 0; i < 725; i++) {
      response[i] = '\0';
    }


    ESPserial.println(F("AT+CIPSTART=\"TCP\",\"demo.iven.io\",80"));
    delay(1000);

    i = 0;
    while (ESPserial.available() ) {
      response[i] = ESPserial.read();
      i++;
    }
    response[i++] = '\0';
    Serial.println(response);
    for (i = 0; i < 725; i++) {
      response[i] = '\0';
    }


    sensorValue = analogRead(lightPin);

    if (sensorValue < 250) {

      ESPserial.println(F("AT+CIPSEND=257"));
      delay(1000);

      i = 0;
      while (ESPserial.available() ) {
        response[i] = ESPserial.read();
        i++;
      }
      response[i++] = '\0';
      Serial.println(response);
      for (i = 0; i < 725; i++) {
        response[i] = '\0';
      }

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
      delay(1000);

      i = 0;
      while (ESPserial.available() ) {
        response[i] = ESPserial.read();
        i++;
      }
      response[i++] = '\0';
      Serial.println(response);
      for (i = 0; i < 725; i++) {
        response[i] = '\0';
      }

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
    i = 0;
    while (true) {
      if (ESPserial.available()) {
        response[i] = ESPserial.read();
        i++;
        if (i > 3) {
          if (response[i - 4] == 'I' && response[i - 3] == 'P' && response[i - 2] == 'D' && response[i - 1] == ',') {
            Serial.print("index is: ");
            Serial.println(i);
            i = 0;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            if ( ESPserial.available()) response[i] = ESPserial.read();
            i++;
            response[i++] = '\0';
            Serial.println(response);
            sscanf(response, "%d", &j);
            Serial.println(j);
            i = 0;
            response[0] = '\0';
          }
        }

        if (i == j) {
          response[i++] = '\0';
          break;
        }
      }
    }
    response[i++] = '\0';
    Serial.println(response);
    if (response[9] == '2' && response[10] == '0' && response[11] == '0') {
      count++;
      Serial.println("Response is 200");
      Serial.print("Count is: ");
      Serial.println(count);
    }
    delay(5000);



  }

}


