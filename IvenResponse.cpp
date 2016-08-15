// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 

#include "IvenResponse.h"

IvenResponse::IvenResponse() 
{
    ivenCode = 0;
    status = 0;
    error = IR_OK;
    task = "";
}

IvenResponse::clearResponse()
{
    ivenCode = 0;
    status = 0;
    error = IR_OK;
    task = "";
}