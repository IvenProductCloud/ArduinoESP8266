//
// Created by berk ozdilek on 19/07/16.
//

#include "IvenCloudESP.h"

/***********************************************/
/*********     HELPER FUNCTIONS      ***********/
/***********************************************/

int ConnectClient(WiFiClient* client) // TODO: remake
{

}

int createActivationCode(const char* secretKey, const char* deviceId, char* activationCode)
{
    uint8_t* hash;
    ShaClass Sha1;
    Sha1.initHmac((const uint8_t*)secretKey, strlen(secretKey));
    Sha1.write(deviceId);
    hash = Sha1.resultHmac();

    int i,j;
    j = 0;
    for (i=0; i<20; i++)
    {
        activationCode[j] = "0123456789abcdef"[hash[i]>>4];
        activationCode[j + 1] = "0123456789abcdef"[hash[i]&0xf];
        j += 2;
    }
    activationCode[41] = '\0';

    return 0;
}

int sendDataRequest(WiFiClient* client, const char* apiKey, IvenData* data, char* serverResponse)
{
    // Connect (make tcp connection)

    // Make request

    // Read response
}

int activationRequest(WiFiClient* client, char* activationCode, char* serverResponse)
{
    // Connect (make tcp connection)

    // Make request

    // Read response
}

/***********************************************/
/*********     CLASS FUNCTIONS      ************/
/***********************************************/

// Constructor
IvenCloudESP::IvenCloudESP()
{
    _apiKey = String();

    // init esp serial
}

// activate Device
IvenResponse IvenCloudESP::activateDevice(const char* secretKey, const char* deviceId)
{
    if (!secretKey || !deviceId)
        return IR_ERROR_NULL_PARAMETER; // one or both of them are null
    if (strlen(secretKey) != 40)
        return IR_ERROR_INVALID_PARAMETER; // secret key must be length of 40

    // create activation code as hex string
    char activationCode[41];
    createActivationCode(secretKey, deviceId, activationCode);

    char serverResponse[625];
    activationRequest(&_client, activationCode, serverResponse);

    // parse response

    return IR_OK;
}

// send data
IvenResponse IvenCloudESP::sendData(IvenData& sensorData)
{
    if (_apiKey.length() == 0)
        return IR_ERROR;

    char serverResponse[625];
    sendDataRequest(&_client, _apiKey.c_str(), &sensorData, serverResponse);

    return IR_OK;
}