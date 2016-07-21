//
// Created by berk ozdilek on 19/07/16.
// 
//

#include "IvenCloudESP.h"

/***********************************************/
/*********     HELPER FUNCTIONS      ***********/
/***********************************************/

// Checks if ESP software serial response contains OK

bool isOk(SoftwareSerial& client, char* buffer) 
{
  bool check = false;
  int i = 0;
  long startTime = millis();
  while (millis() - startTime < 5000) { 
    if (client.available()) {
      buffer[i] = client.read();
      if (buffer[i - 1] == 'O' && buffer[i] == 'K') {
        check = true;
        break;
      }
      i++;
    }
  }

  buffer[i + 1] = '\0';

  while (client.available()) 
    client.read();

  return check;
}

//ARDUINO AND ESP8266 RESTART FUNCTION
void returnSetup(SoftwareSerial& client) 
{
  delay(1000);
  client.println(F("AT+RST"));
  long startTime = millis();
  while (millis() - startTime < 5000) {
    if (client.available()) 
      client.read();
  }
  startTime = millis();

  while (millis() - startTime < 5000);

  soft_restart();
}

//SERVER RESPONSE HANDLER FUNCTION
void handleResponse(SoftwareSerial& client, char* buffer) 
{
  int i = 0;
  int j;
  int index;
  long startTime = millis();
  while (millis() - startTime < 15000) {
    if (client.available()) {
      buffer[i] = client.read();
      if (buffer[i - 2] == '2' && buffer[i - 1] == '0' && buffer[i] == '0') {
        i = 0;
        while (!(buffer[i - 4] == '\r' && buffer[i - 3] == '\n' && buffer[i - 2] == '\r' && buffer[i - 1] == '\n')) {
          if (client.available()) {
            buffer[i] = client.read();
            buffer[i + 1] = '\0';
            i++;
            i %= 120;
          }
        }
        i = 0;
        buffer[4] = '\0';
        buffer[5] = '\0';
        while (i != 4) {
          if (client.available()) {
            buffer[i] = client.read();
            i++;
          }
        }
        j = strtoul(buffer, 0, 16);
        i = 0;
        while (i != j) {
          if (client.available()) {
            buffer[i] = client.read();
            i++;
          }
        }

        buffer[j] = '\0';
        break;
      }
      i++;
      i %= 100;
    }
  }
}


//API-KEY PARSER FUNCTION
bool parseApiKey(char* buffer) 
{
  //Find API-KEY
  int i = 0;
  int index;
  int arrayIndex = 0;
  bool api = false;
  long startTime = millis();
  while (millis() - startTime < 5000) {
    if (buffer[i] == 'a' && buffer[i + 1] == 'p' && buffer[i + 2] == 'i' && buffer[i + 3] == '_' && buffer[i + 4] == 'k' && buffer[i + 5] == 'e' && buffer[i + 6] == 'y') {
      api = true;
      break;
    }
    i++;
  }

  i += 10;

  //Set API-KEY
  for (index = i; index < i + 40; index++) {
    buffer[arrayIndex] = buffer[index];
    arrayIndex++;
  }

  buffer[40] = '\0';

  return api;
}

//IVEN CODE PARSER FUNCTION
bool parseIvenCode(char* buffer) 
{
  int i = 0;
  int index;
  int arrayIndex = 0;
  bool code = false;
  long startTime = millis();
  while (millis() - startTime < 5000) {
    if (buffer[i] == 'i' && buffer[i + 1] == 'v' && buffer[i + 2] == 'e' && buffer[i + 3] == 'n' && buffer[i + 4] == 'C' && buffer[i + 5] == 'o' && buffer[i + 6] == 'd' && buffer[i + 7] == 'e') {
      code = true;
      break;
    }
    i++;
  }

  i += 10;

  for (index = i; index < i + 4; index++) {
    buffer[arrayIndex] = buffer[index];
    arrayIndex++;
  }
  buffer[4] = '\0';

  if (buffer[0] == '1' && buffer[1] == '0' && buffer[2] == '0' && buffer[3] == '0') {
    Serial.print(F("Iven Code: "));
    Serial.println(buffer);
  } else {
    Serial.print(F("Iven Code: "));
    Serial.println(buffer);
  }

  return code;
}

//IVEN TCP CONNECTION FUNCTION
void connectIvenTCP(SoftwareSerial& client, char* buffer) 
{
    client.println(F("AT+CIPCLOSE"));
    long startTime = millis();
    while (millis() - startTime < 3000) 
      client.read(); // Flush Software Serial
    client.print(F("AT+CIPSTART=\"TCP\",\""));
    client.print(server);
    client.print("\",");
    client.println(port);
    if (!isOk(client, buffer)) 
      returnSetup(client);
}


void createActivationCode(const char* secretKey, const char* deviceId, char* activationCode)
{
    uint8_t* hash;
    ShaClass Sha1;
    Sha1.initHmac((const uint8_t*)secretKey, strlen(secretKey));
    Sha1.write(deviceId);
    hash = Sha1.resultHmac();

    int i,j;
    j = 0;
    for (i=0; i<20; i++)
    {
      activationCode[j] = "0123456789abcdef"[hash[i]>>4];
      activationCode[j + 1] = "0123456789abcdef"[hash[i]&0xf];
      j += 2;
    }
    activationCode[40] = '\0';
}

void sendDataRequest(SoftwareSerial& client, IvenData* data, const char* apiKey)
{   
    char buffer[128];
    // Connect (make tcp connection)
    connectIvenTCP(client, buffer);

    //Calculate CIPSEND length for TCP connection.
    char* jSonData = data->toJson();
    int cipSend = 215;
    int contentLength = 1;
    int jSonLength = strlen(jSonData);
    cipSend += jSonLength;
    while (jSonLength / 10 != 0) {
        jSonLength /= 10;
        contentLength++;
    }
    cipSend += contentLength;

    String chipSendStr = String(cipSend);

    // Make request
    String atCipSend = "AT+CIPSEND=";
    atCipSend += chipSendStr;

    client.println(atCipSend);
    if (!isOk(client, buffer)) 
      returnSetup(client);

    client.println(F("POST /data HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.print(F("Connection: keep-alive\r\n"));
    client.println(F("Accept-Encoding: gzip, deflate"));
    client.println(F("Accept: */*"));
    client.println(F("Content-Type: application/json"));
    client.print(F("API-KEY: "));
    client.println(apiKey);
    client.print(F("Content-Length: "));
    client.println(strlen(jSonData));
    client.println();
    client.println(jSonData);

    // Read response
    handleResponse(client, buffer);

    // Parse ivenCode
    if (!parseIvenCode(buffer)) 
      returnSetup(client);
}

void activationRequest(SoftwareSerial& client, char* activationCode, char* buffer)
{
    // Connect (make tcp connection)
    connectIvenTCP(client, buffer);
    
    // Make request
    client.println(F("AT+CIPSEND=107"));
    if (!isOk(client, buffer)) 
      returnSetup(client);

    client.println(F("GET /activate/device HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.print(F("Activation: "));
    client.println(activationCode);
    client.println();
    
    // Read response
    handleResponse(client, buffer);

    // Parse API-KEY
    if (!parseApiKey(buffer)) 
      returnSetup(client);
}

/***********************************************/
/*********     CLASS FUNCTIONS      ************/
/***********************************************/

// Constructor
IvenCloudESP::IvenCloudESP(int arduino_rx_esp_tx, int arduino_tx_esp_rx) :
     _client(arduino_rx_esp_tx, arduino_tx_esp_rx), _apiKey()
{
    // init esp serial
    _client.begin(9600);
}

// activate Device
IvenResponse IvenCloudESP::activateDevice(const char* secretKey, const char* deviceId)
{
    if (!secretKey || !deviceId)
        return IR_ERROR_NULL_PARAMETER; // one or both of them are null
    if (strlen(secretKey) != 40)
        return IR_ERROR_INVALID_PARAMETER; // secret key must be length of 40

    // create activation code as hex string
    char activationCode[41];
    char buffer[128];
    createActivationCode(secretKey, deviceId, activationCode);
    activationRequest(_client, activationCode, buffer);

    _apiKey.concat(buffer);

    Serial.print("API-KEY is: ");
    Serial.println(_apiKey);

    return IR_OK;
}

// send data
IvenResponse IvenCloudESP::sendData(IvenData& sensorData)
{
    if (_apiKey.length() == 0)
        return IR_ERROR;

    sendDataRequest(_client, &sensorData, _apiKey.c_str());

    return IR_OK;
}