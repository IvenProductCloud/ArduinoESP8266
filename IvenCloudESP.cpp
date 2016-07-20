//
// Created by berk ozdilek on 19/07/16.
// 
//

#include "IvenCloudESP.h"

//char IvenCloudESP::_apiKey[41];
//char jSonBuffer[128];

/***********************************************/
/*********     HELPER FUNCTIONS      ***********/
/***********************************************/

// Checks if ESP software serial response contains OK

bool isOk(SoftwareSerial& _client) {
    Serial.println("in isOk()");
  char okBuff[64];
  bool check = false;
  int i = 0;
  long startTime = millis();
  while (millis() - startTime < 5000) { 
    if (_client.available()) {
      okBuff[i] = _client.read();
      if (okBuff[i - 1] == 'O' && okBuff[i] == 'K') {
        check = true;
        break;
      }
      i++;
    }
  }
  return check;
}

//ARDUINO AND ESP8266 RESTART FUNCTION
void returnSetup(SoftwareSerial& _client) {
  delay(1000);
  Serial.println(F("Reseting"));
  _client.println(F("AT+RST"));
  long startTime = millis();
  while (millis() - startTime < 5000) {
    if (_client.available()) Serial.print((char)_client.read());
  }
  startTime = millis();
  while (millis() - startTime < 5000);
  soft_restart();
}

//SERVER RESPONSE HANDLER FUNCTION
void responseHandle(SoftwareSerial& _client, char* jSonBuffer) {
  int i = 0;
  int j;
  int index;
  long startTime = millis();
  while (millis() - startTime < 15000) {
    if (_client.available()) {
      jSonBuffer[i] = _client.read();
      if (jSonBuffer[i - 2] == '2' && jSonBuffer[i - 1] == '0' && jSonBuffer[i] == '0') {
        i = 0;
        while (!(jSonBuffer[i - 4] == '\r' && jSonBuffer[i - 3] == '\n' && jSonBuffer[i - 2] == '\r' && jSonBuffer[i - 1] == '\n')) {
          if (_client.available()) {
            jSonBuffer[i] = _client.read();
            jSonBuffer[i + 1] = '\0';
            i++;
            i %= 120;
          }
        }
        i = 0;
        jSonBuffer[4] = '\0';
        jSonBuffer[5] = '\0';
        while (i != 4) {
          if (_client.available()) {
            jSonBuffer[i] = _client.read();
            i++;
          }
        }
        Serial.println(jSonBuffer);
        j = strtoul(jSonBuffer, 0, 16);
        i = 0;
        while (i != j) {
          if (_client.available()) {
            jSonBuffer[i] = _client.read();
            i++;
          }
        }
        Serial.println(jSonBuffer);
        for (index = j; index < 128; index++) {
          jSonBuffer[j] = '\0';
        }
        break;
      }
      i++;
      i %= 100;
    }
  }
}


//API-KEY PARSER FUNCTION
bool apiParse(char* apiKey, char* jSonBuffer) {
  int i = 0;
  int j = 0;
  int index;
  int arrayIndex = 0;
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

  //Set API-KEY
  for (index = i; index < j; index++) {
    apiKey[arrayIndex] = jSonBuffer[index];
    arrayIndex++;
  }

  apiKey[41] = '\0';

  for (i = 0; i < 128; i++) {
    jSonBuffer[i] = '\0';
  }
  return api;
}

//IVEN CODE PARSER FUNCTION
bool ivenCodeParse(char* jSonBuffer) {
  char ivenCode[5];
  int i = 0;
  int j = 0;
  int index;
  int arrayIndex = 0;
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
    Serial.print("Iven Code: ");
    Serial.println(ivenCode);
    Serial.print("Succesful count: ");
  } else {
    Serial.print("Iven Code: ");
    Serial.println(ivenCode);
  }

  for (i = 0; i < 128; i++) {
    jSonBuffer[i] = '\0';
  }
  return code;

}

//IVEN TCP CONNECTION FUNCTION
void tcpConnectIven(SoftwareSerial& _client) 
{
    _client.println(F("AT+CIPCLOSE"));
    long startTime = millis();
    while (millis() - startTime < 3000) _client.read(); // Flush Software Serial
    _client.print(F("AT+CIPSTART=\"TCP\",\""));
    _client.print(server);
    _client.print("\",");
    _client.println(port);
    if (!isOk(_client)) returnSetup(_client);
}

void cipSendClient(SoftwareSerial& _client) {
    _client.println(F("AT+CIPSEND=107"));
    if (!isOk(_client)) returnSetup(_client);
}

int createActivationCode(const char* secretKey, const char* deviceId, char* activationCode)
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
    activationCode[41] = '\0';

    return 0;
}

int sendDataRequest(SoftwareSerial& _client, IvenData* data, char* jSonBuffer, char* apiKey)
{
    // Connect (make tcp connection)
    tcpConnectIven(_client);

    //Calculate CIPSEND length and put it in a string
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
    _client.print(F("AT+CIPSEND="));
    _client.println(chipSendStr);
    if (!isOk(_client)) returnSetup(_client);

    _client.println(F("POST /data HTTP/1.1"));
    _client.println(F("Host: demo.iven.io"));
    _client.print(F("Connection: keep-alive\r\n"));
    _client.println(F("Accept-Encoding: gzip, deflate"));
    _client.println(F("Accept: */*"));
    _client.println(F("Content-Type: application/json"));
    _client.print(F("API-KEY: "));
    _client.println(apiKey);
    _client.print(F("Content-Length: "));
    _client.println(strlen(jSonData));
    _client.println();
    _client.println(jSonData);

    // Read response
    responseHandle(_client, jSonBuffer);
    if (!ivenCodeParse(jSonBuffer)) returnSetup(_client);
}

int activationRequest(SoftwareSerial& _client, char* activationCode, char* apiKey, char* jSonBuffer)
{
    // Connect (make tcp connection)
    tcpConnectIven(_client);
    
    // Make request
    _client.println(F("GET /activate/device HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(server);
    _client.print(F("Activation: "));
    _client.println(activationCode);
    _client.println();
    
    // Read response
    responseHandle(_client, jSonBuffer);
    if(!apiParse(apiKey, jSonBuffer)) returnSetup(_client);
}

/***********************************************/
/*********     CLASS FUNCTIONS      ************/
/***********************************************/

// Constructor
IvenCloudESP::IvenCloudESP(int arduino_rx_esp_tx, int arduino_tx_esp_rx) : _client(arduino_rx_esp_tx, arduino_tx_esp_rx)
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
    createActivationCode(secretKey, deviceId, activationCode);
    char* apiKey = _apiKey;
    char* jsBuf = jSonBuffer;
    activationRequest(_client, activationCode, apiKey, jsBuf);


    return IR_OK;
}

// send data
IvenResponse IvenCloudESP::sendData(IvenData& sensorData)
{
    if (_apiKey[0] == '\0' && _apiKey[1] == '\0' && _apiKey[2] == '\0')
        return IR_ERROR;

    char* jsBuf = jSonBuffer;
    char* apiKey = _apiKey;
    sendDataRequest(_client, &sensorData, jsBuf, apiKey);

    return IR_OK;
}


