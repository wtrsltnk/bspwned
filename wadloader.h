/* 
 * File:   wadloader.h
 * Author: wouter
 *
 * Created on April 22, 2009, 9:10 AM
 */

#ifndef _WADLOADER_H
#define	_WADLOADER_H

#include "types.h"
#include "interfaces.h"

class WADLoader
{
public:
    WADLoader(const IData* data, const char* filename);
    virtual ~WADLoader();

    const unsigned char* getTextureData(const char* name) const;
    
private:
    char mFilename[256];
    IData* mData;
    tWADHeader mHeader;
    tWADLump* mLumps;

};

#endif	/* _WADLOADER_H */

