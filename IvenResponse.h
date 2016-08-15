// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 

#ifndef PROJECT_IVENRESPONSE_H
#define PROJECT_IVENRESPONSE_H

#include <Arduino.h>


typedef enum Error_e {
    IR_OK = 0,
    IR_ERROR_INVALID_PARAMETER = 1,
    IR_ERROR_NULL_PARAMETER = 2,
    IR_ERROR = 3,
} ErrorList;

class IvenResponse
{
public:
    IvenResponse();
    clearResponse();
    int ivenCode;
    uint16_t status;
    ErrorList error;
    String task;
};


#endif //PROJECT_IVENRESPONSE_H
