// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 
/*
IvenResponse is a server response object class in which one can check the vital parts of 
the server response and possible error codes while connecting a device to Iven Product Cloud.
*/
#ifndef PROJECT_IVENRESPONSE_H
#define PROJECT_IVENRESPONSE_H

#include <Arduino.h>


typedef enum Error_e {
    IR_OK = 0,
    IR_INVALID_PARAMETER = 1,
    IR_NULL_PARAMETER = 2,
    IR_CONNECTION_ERROR = 3,
    IR_API_KEY_MISSING = 4,
    IR_IVEN_CODE_MISSING = 5,
    IR_WRONG_ACTIVATION_CODE = 6
} ErrorList;

class IvenResponse
{
public:
    IvenResponse();
    void clearResponse();
    int ivenCode;
    uint16_t httpStatus;
    ErrorList error;
    String task;
};


#endif //PROJECT_IVENRESPONSE_H
