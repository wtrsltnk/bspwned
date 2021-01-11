/* 
 * File:   error.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 10:55 PM
 */

#include "error.h"

#include <string.h>

Error::Error()
        : mLastError(0)
{
}

Error::~Error()
{
    if (this->mLastError != 0)
        delete this->mLastError;
}

const char* Error::getLastError()
{
    return this->mLastError;
}

void Error::setError(const char* message)
{
    if (this->mLastError != 0)
        delete this->mLastError;

    this->mLastError = new char[strlen(message) + 1];
    
    if (this->mLastError != 0)
        strcpy(this->mLastError, message);
}

