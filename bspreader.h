/* 
 * File:   bspreader.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:28 PM
 */

#ifndef _BSPREADER_H
#define	_BSPREADER_H

#include "bsperror.h"
#include "bspcontent.h"
#include "interfaces.h"
#include "tokenizer.h"
#include <fstream>

class BSPReader : public BSPError
{
public:
    class Configuration
    {
    public:
        Configuration();
        virtual ~Configuration();

        IResources* resources;
    };

public:
    BSPReader(const Configuration& config);
    virtual ~BSPReader();

    bool readBSP(const char* filename, BSPContent* content);
    int readLump(const tBSPLump& lump, std::istream& bsp, unsigned char** data, int itemSize);
    bool readEntities(const tBSPLump& lump, std::istream& bsp);
    bool readAndDecompressVisiblity(const tBSPLump& lump, std::istream& bsp);
    bool readTextures(const tBSPLump& lump, std::istream& bsp);
    bool readFaces(const tBSPHeader& header, std::istream& bsp);

protected:
    bool readEntity(Tokenizer& tok);
	bool findWadfiles();
    bool readMiptex(const tBSPMipTexHeader& header, tBSPTexture& texture, unsigned char* buffer);
    bool readTextureFromMiptex(const tBSPMipTexHeader& header, tBSPTexture& texture, unsigned char* buffer);

protected:
    BSPContent* mContent;
    const Configuration& mReadConfig;
    
};

#endif	/* _BSPREADER_H */

