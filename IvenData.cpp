// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 

#include "IvenData.h"

IvenData::IvenData()
{
    String();
};

// Creates string data object for Iven Product Cloud.
void IvenData::add(const char* key, const char* value)
{
    bool isFirst = true;
    if (len == 0)
    {
        concat("{\"data\":[{"); // }]}
        isFirst = false;
    }

    if (isFirst)
    {
        concat(',');
    }

    concat('"');
    concat(key);
    concat("\":\"");
    concat(value);
    concat('"');
}

// Creates integer data object for Iven Product Cloud.
void IvenData::add(const char* key, int value)
{
    bool isFirst = true;
    if (len == 0)
    {
        concat("{\"data\":[{");
        isFirst = false;
    }

    if (isFirst)
    {
        concat(',');
    }

    concat('"');
    concat(key);
    concat("\":");
    concat(value);
}

// Creates boolean data object for Iven Product Cloud.
void IvenData::add(const char* key, boolean value)
{
    bool isFirst = true;
    if (len == 0)
    {
        concat("{\"data\":[{"); // }]}
        isFirst = false;
    }

    if (isFirst)
    {
        concat(',');
    }

    concat('"');
    concat(key);
    concat("\":"); 
    if (value) concat("true");
    else concat("false");
}

char* IvenData::print()
{
    return &buffer[10];
}

// Tells Iven Product Cloud that the task sent from cloud is done.
void IvenData::taskDone(int ivenCode)
{
    concat("}],\"iven_code\":");
    concat(ivenCode);
    _task = true;
}

char* IvenData::toJson()
{
    if(!_task)
        concat("}]}");
    else {
        concat("}");
        _task = false;
    }
    return buffer;
}

void IvenData::clear()
{
    invalidate();
}
