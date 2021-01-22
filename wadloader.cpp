/* 
 * File:   wadloader.cpp
 * Author: wouter
 * 
 * Created on April 22, 2009, 9:10 AM
 */

#include "wadloader.h"

#include <iostream>
#include <string.h>

using namespace std;

WADLoader::WADLoader(
    const IData *data,
    const std::string &filename)
    : _filename(filename),
      _fileData(std::unique_ptr<IData>(data->copy()))
{
    _fileData->read(&_header);
    _lumps = std::unique_ptr<tWADLump[]>(new tWADLump[_header.lumpsCount]);
    _fileData->read(_lumps.get(), _header.lumpsCount, _header.lumpsOffset);
}

WADLoader::~WADLoader() = default;

const unsigned char *WADLoader::getTextureData(
    const std::string &name) const
{
    for (int i = 0; i < _header.lumpsCount; i++)
    {
        if (iequals(_lumps[i].name, name))
        {
            return _fileData->Data() + _lumps[i].offset;
        }
    }

    return nullptr;
}
