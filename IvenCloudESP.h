//
// Created by berk ozdilek on 19/07/16.
//

#ifndef PROJECT_IVENCLOUDESP_H
#define PROJECT_IVENCLOUDESP_H

#include <Arduino.h>
#include "ShaClass.h"
#include "IvenData.h"

#define server "demo.iven.io"
#define port 80

typedef enum ivenResponse_e {
    IR_OK = 0,
    IR_ERROR_INVALID_PARAMETER = 1,
    IR_ERROR_NULL_PARAMETER = 2,
    IR_ERROR = 3,

    IR_IVENCODE_NOT_FOUND = 999,
    IR_IVENCODE_1000	=	1000,
    IR_IVENCODE_1001    =   1001,
    IR_IVENCODE_1003    =   1003,
    IR_IVENCODE_1004	=	1004,
    IR_IVENCODE_1005	=	1005,
    IR_IVENCODE_1010	=   1010,
    IR_IVENCODE_1015	=   1015,
    IR_IVENCODE_1016	=	1016,
} IvenResponse;

class IvenCloudESP {
public:
    IvenCloudWifi();
    IvenResponse activateDevice(const char* secretKey, const char* deviceId);
    IvenResponse sendData(IvenData& sensorData);

private:
    String _apiKey;

};


#endif //PROJECT_IVENCLOUDESP_H
