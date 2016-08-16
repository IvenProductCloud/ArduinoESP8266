// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek

/*
IvenCloudESP is an Arduino library to connect Arduino boards to Iven Product Cloud by using ESP8266 with serial communication.
Checkout the examples folder to see examples.
Further examples and guides can be found in iven blog : http://blog.iven.io
*/

#ifndef PROJECT_IVENCLOUDESP_H
#define PROJECT_IVENCLOUDESP_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SoftReset.h>
#include "ShaClass.h"
#include "IvenData.h"
#include "IvenResponse.h"

#define server "demo.iven.io"
#define port "80"

class IvenCloudESP {
public:
    /* Creates IvenCloudESP object and initilazes RX/TX pins of Arduino for communication with ESP8266,
    sets baud rate and sets automatic reset function for IoT projects*/
    IvenCloudESP(uint8_t arduino_rx_esp_tx, uint8_t arduino_tx_esp_rx, int baud_rate, bool systemReset = true);

    // Activates device, saves API-KEY into object and returns IvenResponse object for observation.
    IvenResponse activateDevice(const char* secretKey, const char* deviceId);

    // Posts data to Iven Product Cloud by using API-KEY and returns IvenResponse object for observation.
    IvenResponse sendData(IvenData& sensorData);
    
private:
    static IvenResponse response;
    SoftwareSerial _client;
    String _apiKey;
    char buffer[128];
    bool _sysReset, _check;
private:
    bool isOk();
    void reset();
    bool handleResponseHeader();
    bool parseApiKey();
    bool handleResponseBody();
    void connectIvenTCP();
    void createActivationCode(const char* secretKey, const char* deviceId, char* activationCode);
    void sendDataRequest(IvenData* data);
    void activationRequest(char* activationCode);
};



#endif //PROJECT_IVENCLOUDESP_H
