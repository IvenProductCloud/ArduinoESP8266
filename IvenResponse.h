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
    uint16_t httpStatus;
    ErrorList error;
    String task;
};


#endif //PROJECT_IVENRESPONSE_H
