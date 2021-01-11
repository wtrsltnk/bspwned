/* 
 * File:   bspentity.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 9:33 PM
 */

#include "bspentity.h"

using namespace std;

BSPEntity::BSPEntity()
{
}

BSPEntity::~BSPEntity()
{
}

const char* BSPEntity::getClassName() const
{
    return this->mClassName.c_str();
}

const char* BSPEntity::getValue(const char* key) const
{
    const map<string, string>::const_iterator& itr = this->mEntityKeys.find(key);
    if (itr != this->mEntityKeys.end())
    {
	return itr->second.c_str();
    }
    return NULL;
}
