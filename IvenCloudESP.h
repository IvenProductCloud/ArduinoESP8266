// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek

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
    IvenCloudESP(uint8_t arduino_rx_esp_tx, uint8_t arduino_tx_esp_rx, int baud_rate, bool systemReset = true);
    IvenResponse activateDevice(const char* secretKey, const char* deviceId);
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
