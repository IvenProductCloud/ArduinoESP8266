#ifndef PROJECT_IVENRESPONSE_H
#define PROJECT_IVENRESPONSE_H

#include <Arduino.h>


typedef enum Error {
    IR_OK = 0,
    IR_ERROR_INVALID_PARAMETER = 1,
    IR_ERROR_NULL_PARAMETER = 2,
    IR_ERROR = 3,
} Error;

class IvenResponse
{
public:
    IvenResponse();
    int ivenCode;
    int status;
    Error params;
    String task;
};


#endif //PROJECT_IVENRESPONSE_H
