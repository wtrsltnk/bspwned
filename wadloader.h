/* 
 * File:   wadloader.h
 * Author: wouter
 *
 * Created on April 22, 2009, 9:10 AM
 */

#ifndef _WADLOADER_H
#define _WADLOADER_H

#include "interfaces.h"
#include "types.h"

class WADLoader
{
public:
    WADLoader(
        const IData *data,
        const std::string &filename);

    virtual ~WADLoader();

    const unsigned char *getTextureData(
        const std::string &name) const;

private:
    std::string _filename;
    std::unique_ptr<IData> _fileData;
    tWADHeader _header;
    std::unique_ptr<tWADLump[]> _lumps;
};

#endif /* _WADLOADER_H */
