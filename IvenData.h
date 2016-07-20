//
// Created by berk ozdilek on 27/06/16.
//

#ifndef PROJECT_IVENDATA_H
#define PROJECT_IVENDATA_H

#include <WString.h>
#include <Arduino.h>

class IvenData : private String
{
public:
    IvenData();
    void add(const char* key, const char* value);
    void add(const char* key, int value);
    void add(const char* key, boolean value);
    char* print();
    void clear();
    inline unsigned int length(void) const {return len;}

    char* toJson();

};


#endif //PROJECT_IVENDATA_H
