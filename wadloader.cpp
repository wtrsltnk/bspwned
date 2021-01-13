/* 
 * File:   wadloader.cpp
 * Author: wouter
 * 
 * Created on April 22, 2009, 9:10 AM
 */

#include "wadloader.h"

#include "stringcompare.h"
#include <iostream>
#include <string.h>

using namespace std;

WADLoader::WADLoader(
    const IData *data,
    const std::string &filename)
    : mFilename(filename),
      mData(std::unique_ptr<IData>(data->copy()))
{
    mData->read(&mHeader);
    mLumps = std::unique_ptr<tWADLump[]>(new tWADLump[mHeader.lumpsCount]);
    mData->read(mLumps.get(), mHeader.lumpsCount, mHeader.lumpsOffset);
}

WADLoader::~WADLoader() = default;

const unsigned char *WADLoader::getTextureData(
    const std::string &name) const
{
    for (int i = 0; i < mHeader.lumpsCount; i++)
    {
        if (iequals(mLumps[i].name, name))
        {
            return mData->Data() + mLumps[i].offset;
        }
    }

    return nullptr;
}
