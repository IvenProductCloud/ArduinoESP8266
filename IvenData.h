// Created by Rıza Arda Kırmızıoğlu and Berk Özdilek 
/*
IvenData is a class to create appropriate json string objects to POST data to Iven Product Cloud.
*/
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
    void taskDone(int ivenCode);
    char* print();
    void clear();
    inline unsigned int length(void) const {return len;}

private:
	bool _task = false;
private:
	char* toJson();
	friend class IvenCloudESP;
};


#endif //PROJECT_IVENDATA_H
