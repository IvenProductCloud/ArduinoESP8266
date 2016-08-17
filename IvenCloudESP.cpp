// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 

#include "IvenCloudESP.h"

uint8_t i, j, index, arrayIndex;
long startTime;

/***********************************************/
/*********  PRIVATE HELPER FUNCTIONS ***********/
/***********************************************/

// Checks if ESP software serial response contains OK
bool IvenCloudESP::isOk() 
{
  _check = false;
  i = 0;
  startTime = millis();
  while (millis() - startTime < 5000) { 
    if (_client.available()) {
      buffer[i] = _client.read();
      if (buffer[i - 1] == 'O' && buffer[i] == 'K') {
        _check = true;
        break;
      }
      i++;
    }
  }

  buffer[i + 1] = '\0';

  while (_client.available()) 
    _client.read();

  return _check;
}

// Arduino and ESP reset function
void IvenCloudESP::reset() 
{
  if (!_sysReset)
    return;
  delay(1000);
  _client.println(F("AT+RST"));
  startTime = millis();
  while (millis() - startTime < 5000) {
    if (_client.available()) 
      _client.read();
  }

  delay(10000);

  soft_restart();
}


bool IvenCloudESP::handleResponseHeader() 
{
  i = 0;
  index = 0;
  startTime = millis();
  while (millis() - startTime < 15000) {
    if (_client.available()) {
      buffer[i] = _client.read();
      if (buffer[i - 3] == '1' && buffer[i - 2] == '.' && buffer[i - 1] == '1' && buffer[i] == ' ') {
        i = 0;
        while (i != 3) {
          if (_client.available()) {
            buffer[i] = _client.read();
            i++;
          }
        }
        buffer[3] = '\0';
        response.httpStatus = strtoul(buffer, 0, 10);
        if (response.httpStatus > 500)
          return false;
        i = 0;
        while (!(buffer[i - 4] == '\r' && buffer[i - 3] == '\n' && buffer[i - 2] == '\r' && buffer[i - 1] == '\n')) {
          if (_client.available()) {
            buffer[i] = _client.read();
            i++;
            if (buffer[i - 1] != '\r' && buffer[i - 1] != '\n')
            	i %= 64;
          }
        }
        buffer[i] = '\0';
        i = 0;
        buffer[4] = '\0';
        while (i != 4) { 
          if (_client.available()) {
            buffer[i] = _client.read();
            i++;
          }
        }
        j = strtoul(buffer, 0, 16);
        i = 0;
        while (i != j) {
          if (_client.available()) {
            buffer[i] = _client.read();
            i++;
          }
        }

        buffer[j] = '\0';
        return true;
      }
      i++;
      if (buffer[i - 1] != '1' && buffer[i - 1] != ' ')
      	i %= 64;
    }
  }
  return false;
}


// API-KEY parser function
bool IvenCloudESP::parseApiKey() 
{
  //Find API-KEY
  i = 0;
  arrayIndex = 0;
  _check = false;
  startTime = millis();
  while (millis() - startTime < 5000) {
    if (i == 127)
      break;
    if (buffer[i] == 'a' && buffer[i + 1] == 'p' && buffer[i + 2] == 'i' && buffer[i + 3] == '_' && 
           buffer[i + 4] == 'k' && buffer[i + 5] == 'e' && buffer[i + 6] == 'y') {
      _check = true;
      break;
    }
    i++;
  }

  i += 10;

  buffer[i + 40] = '\0';
  // Set API-KEY
  _apiKey.concat((buffer + i));

  return _check;
}

// Iven code parser function
bool IvenCloudESP::handleResponseBody() 
{
  i = 0;
  arrayIndex = 0;
  _check = false;
  startTime = millis();
  while (millis() - startTime < 5000) {
    if (i == 127)
      break;
    if (buffer[i] == 'i' && buffer[i + 1] == 'v' && buffer[i + 2] == 'e' && buffer[i + 3] == 'n' &&
          buffer[i + 4] == 'C' && buffer[i + 5] == 'o' && buffer[i + 6] == 'd' && buffer[i + 7] == 'e') {

      _check = true;
      i += 10;
      j = i;

      while (buffer[j] != '"')
        j++;

      buffer[j] = '\0';

      // Set Iven Code
      response.ivenCode = strtoul((buffer + i), 0, 10);
    }

    if (buffer[i] == 't' && buffer[i + 1] == 'a' && buffer[i + 2] == 's' && buffer[i + 3] == 'k') {
      response.task = "";
      i += 7;
      j = i;

      while (buffer[j] != '"')
        j++;

      buffer[j] = '\0';

      response.task.concat((buffer + i));
    }

    i++;
  }

  return _check;
}


// Iven TCP connection function
void IvenCloudESP::connectIvenTCP() 
{
    _client.println(F("AT+CIPCLOSE"));
    startTime = millis();
    while (millis() - startTime < 3000) {
      i = 0;
      if (_client.available()) {
        buffer[i] = _client.read();
        if ((buffer[i - 1] == 'O' && buffer[i] == 'R' ) || (buffer[i - 1] == 'O' && buffer[i] == 'K')) {
          break;
        }
        i++;
      }
    }

    while(_client.available())
      _client.read(); 
     
    _client.print(F("AT+CIPSTART=\"TCP\",\""));
    _client.print(server);
    _client.print("\",");
    _client.println(port);
    if (!isOk()) {
      response.error = IR_CONNECTION_ERROR;
      reset();
    }
}

// Creates activation code to have api-key from server.
void IvenCloudESP::createActivationCode(const char* secretKey, const char* deviceId, char* activationCode)
{
    uint8_t* hash;
    ShaClass Sha1;
    Sha1.initHmac((const uint8_t*)secretKey, strlen(secretKey));
    Sha1.write(deviceId);
    hash = Sha1.resultHmac();

    i = 0;
    j = 0;
    for (i=0; i<20; i++)
    {
      activationCode[j] = "0123456789abcdef"[hash[i]>>4];
      activationCode[j + 1] = "0123456789abcdef"[hash[i]&0xf];
      j += 2;
    }
    activationCode[40] = '\0';
}

void IvenCloudESP::sendDataRequest(IvenData* data)
{   
    // Connect (make TCP connection)
    connectIvenTCP();

    // Calculate CIPSEND length for TCP connection.
    char* jSonData = data->toJson();
    uint16_t cipSend = 203 + strlen(server);
    uint16_t contentLength = 1;
    uint16_t jSonLength = strlen(jSonData);
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

    _client.println(atCipSend);
    if (!isOk()) 
      reset();

    _client.println(F("POST /data HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(server);
    _client.println(F("Connection: keep-alive"));
    _client.println(F("Accept-Encoding: gzip, deflate"));
    _client.println(F("Accept: */*"));
    _client.println(F("Content-Type: application/json"));
    _client.print(F("API-KEY: "));
    _client.println(_apiKey);
    _client.print(F("Content-Length: "));
    _client.println(strlen(jSonData));
    _client.println();
    _client.println(jSonData);

    // Read response
    if (handleResponseHeader()) {

      // Parse ivenCode
      if (!handleResponseBody()) {
        response.error = IR_IVEN_CODE_MISSING;
        reset();
      }

    }
}

void IvenCloudESP::activationRequest(char* activationCode)
{
    // Connect (make tcp connection)
    connectIvenTCP();

    // Calculate CIPSEND length for TCP connection.
    uint16_t cipSend = 95;
    cipSend += strlen(server);
    String cipSendStr = String(cipSend);
    String atCipSend = "AT+CIPSEND=";
    atCipSend += cipSendStr;

    
    // Make request
    _client.println(atCipSend);
    if (!isOk()) 
      reset();

    _client.println(F("GET /activate/device HTTP/1.1"));
    _client.print(F("Host: "));
    _client.println(server);
    _client.print(F("Activation: "));
    _client.println(activationCode);
    _client.println();
    
    // Read response
    if (handleResponseHeader()) {

    // Parse API-KEY
    if (!parseApiKey()) 
      reset();
  }
}

/***********************************************/
/*********     CLASS FUNCTIONS      ************/
/***********************************************/

// Constructor
IvenCloudESP::IvenCloudESP(uint8_t arduino_rx_esp_tx, uint8_t arduino_tx_esp_rx, int baud_rate, bool systemReset) :
     _client(arduino_rx_esp_tx, arduino_tx_esp_rx), _apiKey()
{
    response = IvenResponse();

    // Initializes ESP Serial
    _client.begin(baud_rate);

    // Sets system reset
    _sysReset = systemReset;
}

// Activates device on Iven cloud
IvenResponse IvenCloudESP::activateDevice(const char* secretKey, const char* deviceId)
{
    response.clearResponse();

    if (!secretKey || !deviceId) {
      response.error = IR_ERROR_NULL_PARAMETER;
      return response;
    }
    if (strlen(secretKey) != 40) {
      response.error = IR_ERROR;
      return response;
    }
         
    char activationCode[41];

    // Creates activation code as hex string
    createActivationCode(secretKey, deviceId, activationCode);
    activationRequest(activationCode);

    response.error = 0;
    return response;
}

// Send data to Iven cloud
IvenResponse IvenCloudESP::sendData(IvenData& sensorData)
{
    response.clearResponse();

    if (_apiKey.length() == 0) {
      response.error = IR_ERROR;
      
      return response;
    }

    sendDataRequest(&sensorData);
    
    return response;
}