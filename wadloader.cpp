/* 
 * File:   wadloader.cpp
 * Author: wouter
 * 
 * Created on April 22, 2009, 9:10 AM
 */

#include "wadloader.h"
#include "stringcompare.h"
#include <string.h>
#include <iostream>
using namespace std;

WADLoader::WADLoader(const IData* data, const char* filename)
        : mData(data->copy())
{
    strcpy(this->mFilename, filename);
    this->mData->read(&this->mHeader);
    this->mLumps = new tWADLump[this->mHeader.lumpsCount];
    this->mData->read(this->mLumps, this->mHeader.lumpsCount, this->mHeader.lumpsOffset);
}

WADLoader::~WADLoader()
{
    delete []this->mLumps;
    delete this->mData;
}

const unsigned char* WADLoader::getTextureData(const char* name) const
{
    for (int i = 0; i < this->mHeader.lumpsCount; i++)
    {
        str l = { this->mLumps[i].name };
        str r = { name };
        if (l == r)
        {
            return this->mData->data + this->mLumps[i].offset;
        }
    }
    return NULL;
}
