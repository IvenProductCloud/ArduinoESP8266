//
// Created by berk ozdilek on 19/07/16.
//

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
    IvenCloudESP(int arduino_rx_esp_tx, int arduino_tx_esp_rx, int baud_rate);
    IvenResponse activateDevice(const char* secretKey, const char* deviceId);
    IvenResponse sendData(IvenData& sensorData);
    static IvenResponse response;

private:
    SoftwareSerial _client;
    String _apiKey;
};



#endif //PROJECT_IVENCLOUDESP_H
