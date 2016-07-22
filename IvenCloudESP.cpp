//
// Created by berk ozdilek on 19/07/16.
// 

#include "IvenCloudESP.h"

char buffer[128];
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

// Arduino and ESP reset function
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

  while (millis() - startTime < 10000);

  soft_restart();
}

// Server response parser function
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
            i++;
            if (buffer[i - 1] != '\r' && buffer[i - 1] != '\n')
            	i %= 64;
          }
        }
        buffer[i] = '\0';
        i = 0;
        buffer[4] = '\0';
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
      if (buffer[i - 1] != '2' && buffer[i - 1] != '0')
      	i %= 64;
    }
  }
}


// API-KEY parser function
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

  // Set API-KEY
  for (index = i; index < i + 40; index++) {
    buffer[arrayIndex] = buffer[index];
    arrayIndex++;
  }

  buffer[40] = '\0';

  return api;
}

// Iven code parser function
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


  return code;
}

// Iven TCP connection function
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

int sendDataRequest(SoftwareSerial& client, IvenData* data, const char* apiKey)
{   
    // Connect (make TCP connection)
    connectIvenTCP(client, buffer);

    // Calculate CIPSEND length for TCP connection.
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

    String cipSendStr = String(cipSend);

    // Make request
    String atCipSend = "AT+CIPSEND=";
    atCipSend += cipSendStr;

    client.println(atCipSend);
    if (!isOk(client, buffer)) 
      returnSetup(client);

    client.println(F("POST /data HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Connection: keep-alive"));
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

  	return strtoul(buffer, 0, 10);

}

void activationRequest(SoftwareSerial& client, char* activationCode, char* buffer)
{
    // Connect (make tcp connection)
    connectIvenTCP(client, buffer);

    // Calculate CIPSEND length for TCP connection.
    int cipSend = 95;
    int hostLength = strlen(server);
    cipSend += hostLength;
    String cipSendStr = String(cipSend);
    String atCipSend = "AT+CIPSEND=";
    atCipSend += cipSendStr;

    
    // Make request
    client.println(atCipSend);
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
    // Initializes ESP Serial
    _client.begin(9600);
}

// Activates device on Iven cloud
IvenResponse IvenCloudESP::activateDevice(const char* secretKey, const char* deviceId)
{
    if (!secretKey || !deviceId)
        return IR_ERROR_NULL_PARAMETER; // One or both of them are null
    if (strlen(secretKey) != 40)
        return IR_ERROR_INVALID_PARAMETER; // Secret key must be length of 40

    // Creates activation code as hex string
    char activationCode[41];
    createActivationCode(secretKey, deviceId, activationCode);
    activationRequest(_client, activationCode, buffer);

    _apiKey.concat(buffer);

    return IR_OK;
}

// Send data to Iven cloud
IvenResponse IvenCloudESP::sendData(IvenData& sensorData)
{
    if (_apiKey.length() == 0)
        return IR_ERROR;

    IvenResponse ivenCode = (IvenResponse)sendDataRequest(_client, &sensorData, _apiKey.c_str());
    
    return ivenCode;
}