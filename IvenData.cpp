//
// Created by berk ozdilek on 27/06/16.
//

#include "IvenData.h"

IvenData::IvenData()
{
    String();
};

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

char* IvenData::toJson()
{
    concat("}]}");
    return buffer;
}

void IvenData::clear()
{
    invalidate();
}
