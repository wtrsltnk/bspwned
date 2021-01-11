/* 
 * File:   bspreader.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 8:28 PM
 */

#include "bsperror.h"


#include "bspcontent.h"
#include "interfaces.h"


#include "bspreader.h"
#include "bsptypes.h"
#include "bspentity.h"
#include "lightmap.h"
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])

BSPReader::BSPReader(const Configuration& config)
        : mReadConfig(config)
{
}

BSPReader::~BSPReader()
{
}

bool BSPReader::readBSP(const char* filename, BSPContent* content)
{
    tBSPHeader header = { 0 };
    this->mContent = content;
    ifstream bsp(filename);

    if (!bsp.good())
    {
        setError("Can\'t read file");
        return false;
    }
    
    bsp.read((char*)&header, sizeof(tBSPHeader));

    if (bsp.fail())
    {
        bsp.close();
        setError("Can\'t read header from BSP file");
        return false;
    }

    if (header.signature != HL1_BSP_SIGNATURE)
    {
        bsp.close();
        setError("Signature of this BSP file is not correct");
        return false;
    }

    // First read the dumb data blocks
    
    try
    {
        this->mContent->mNodeCount = this->readLump(header.lumps[HL1_BSP_NODELUMP], bsp, (unsigned char**)&this->mContent->mNodes, sizeof(tBSPNode));
        this->mContent->mLightingDataSize = this->readLump(header.lumps[HL1_BSP_LIGHTINGLUMP], bsp, (unsigned char**)&this->mContent->mLightingData, sizeof(unsigned char));
        this->mContent->mClipNodeCount = this->readLump(header.lumps[HL1_BSP_CLIPNODELUMP], bsp, (unsigned char**)&this->mContent->mClipNodes, sizeof(tBSPClipNode));
        this->mContent->mLeafCount = this->readLump(header.lumps[HL1_BSP_LEAFLUMP], bsp, (unsigned char**)&this->mContent->mLeafs, sizeof(tBSPLeaf));
        this->mContent->mMarkedSurfacesCount = this->readLump(header.lumps[HL1_BSP_MARKSURFACELUMP], bsp, (unsigned char**)&this->mContent->mMarkedSurfaces, sizeof(unsigned short));
        this->mContent->mEdgeCount = this->readLump(header.lumps[HL1_BSP_EDGELUMP], bsp, (unsigned char**)&this->mContent->mEdges, sizeof(tBSPEdge));
        this->mContent->mModelCount = this->readLump(header.lumps[HL1_BSP_MODELLUMP], bsp, (unsigned char**)&this->mContent->mModels, sizeof(tBSPModel));
    }
    catch (const char* error)
    {
        bsp.close();
        setError(error);
        return false;
    }

    // Then read the data blocks that need parsing or decompression
    
    if (!readEntities(header.lumps[HL1_BSP_ENTITYLUMP], bsp))
    {
        bsp.close();
        return false;
    }

    if (!readTextures(header.lumps[HL1_BSP_TEXTURELUMP], bsp))
    {
        bsp.close();
        return false;
    }

    if (!readAndDecompressVisiblity(header.lumps[HL1_BSP_VISIBILITYLUMP], bsp))
    {
        bsp.close();
        return false;
    }

    if (!readFaces(header, bsp))
    {
        bsp.close();
        return false;
    }

    bsp.close();
    return true;
}

int BSPReader::readLump(const tBSPLump& lump, istream& bsp, unsigned char** data, int itemSize)
{
    int count = lump.size / itemSize;
    if (lump.size % itemSize != 0)
        throw "Incorrect lump size";

    if (*data != 0)
        delete []*data;

    *data = new unsigned char[lump.size];
    if (*data == NULL)
        throw "Could not allocate buffer to read file data";

    bsp.seekg(lump.offset, ios_base::beg);
    if (bsp.fail())
        throw "Could not jump to lump offset";

    bsp.readsome((char*)*data, lump.size);
    if (bsp.fail())
        throw "Could not read lump from file";

    return count;
}

bool BSPReader::readEntities(const tBSPLump& lump, istream& bsp)
{
    bsp.seekg(lump.offset, ios_base::beg);

    if (bsp.fail())
    {
        setError("Can\'t read entity data from BSP file");
        return false;
    }

    Tokenizer tok(bsp);

    while (tok.nextToken() && strcmp(tok.getToken(), "{") == 0)
    {
        if (!readEntity(tok))
            return false;
    }

	return findWadfiles();
}

bool BSPReader::readEntity(Tokenizer& tok)
{
    BSPEntity* e = new BSPEntity();
    while (tok.nextToken() && strcmp(tok.getToken(), "}") != 0)
    {
        string key = tok.getToken();
        if (!tok.nextToken())
        {
            setError("Unexpected end of stream");
            delete e;
            return false;
        }
        string value = tok.getToken();
        if (value == "}")
        {
            setError("Unexpected end of entity block");
            delete e;
            return false;
        }
        e->mEntityKeys.insert(make_pair(key, value));
        if (key == "classname")
            e->mClassName = value;
    }
    this->mContent->mEntities.push_back(e);
    return true;
}

bool BSPReader::readAndDecompressVisiblity(const tBSPLump& lump, std::istream& bsp)
{
    if (this->mContent->mLeafCount == 0)
        return true;

    unsigned char* buffer = NULL;
    try
    {
        readLump(lump, bsp, &buffer, sizeof(unsigned char));
    }
    catch (const char* error)
    {
        if (buffer != NULL)
            delete []buffer;
        setError(error);
        return false;
    }

    this->mContent->mPvs = new tBSPIndexList[this->mContent->mLeafCount];

    tBSPIndexList* pvs =this->mContent->mPvs;
    tBSPLeaf* leafs = this->mContent->mLeafs;

    for (int i = 0; i < this->mContent->mLeafCount; i++)
    {
        vector<int> leafList;
        int visOffset = leafs[i].visofs;
        
        for (int j = 1; j < this->mContent->mModels[0].visLeafs; visOffset++)
        {
            if (buffer[visOffset] == 0)
            {
                visOffset++;
                j += (buffer[visOffset]<<3);
            }
            else
            {
                for (unsigned char bit = 1; bit; bit<<=1, j++)
                {
                    if (buffer[visOffset] & bit)
                        leafList.push_back(j);
                }
            }
        }

        if (leafList.size() > 0)
        {
            pvs[i].size = leafList.size();
            pvs[i].indices = new int[pvs[i].size];
            for (int j = 0; j < pvs[i].size; j++)
            {
                pvs[i].indices[j] = leafList[j];
            }
        }
        else
        {
            pvs[i].size = 0;
            pvs[i].indices = NULL;
        }
    }
    delete []buffer;
    
    return true;
}

bool BSPReader::findWadfiles()
{
	if (this->mReadConfig.resources == NULL)
	{
		setError("There is no resource manager defined to load wad files from");
		return false;
	}
	const BSPEntity* worldSpawn = this->mContent->findEntity("worldspawn");
	if (worldSpawn == NULL)
	{
		setError("Ther worldspawn could not be found");
	}
	string strWads = worldSpawn->getValue("wad");

	int start = 0;
	int hit = strWads.find(';', start);
	while (hit > -1)
	{
		// Pick the wad full wad file with path from the wads string
		string wad = strWads.substr(start, hit-start);
		// Strip the found wadfile of its path
		string stripedWad = wad.substr(wad.find_last_of('\\') + 1);
		// Read the wad file
		const char* wadFile = this->mReadConfig.resources->findFile(stripedWad.c_str());
		// Add it to the wad list if it is not NULL
		if (wadFile != NULL)
			this->mContent->mWadFiles.push_back(wadFile);

		start = hit + 1;
		hit = strWads.find(';', start);
	}
	return true;
}

bool BSPReader::readTextures(const tBSPLump& lump, istream& bsp)
{
    unsigned char* buffer = NULL;
    try
    {
        readLump(lump, bsp, &buffer, sizeof(unsigned char));
    }
    catch (const char* error)
    {
        if (buffer != NULL)
            delete []buffer;
        setError(error);
        return false;
    }

    this->mContent->mTextureCount = ((int*)buffer)[0];
    this->mContent->mTextures = new tBSPTexture[this->mContent->mTextureCount];
    if (this->mContent->mTextures == NULL)
        throw "Could not allocate buffer create textures";

    int* offsetTable = (int*)(buffer + sizeof(int));

    for (int i = 0; i < this->mContent->mTextureCount; i++)
    {
        tBSPMipTexHeader* miptex = (tBSPMipTexHeader*)&buffer[offsetTable[i]];

        if (!readMiptex(*miptex, this->mContent->mTextures[i], buffer + offsetTable[i]))
        {
            if (buffer != NULL)
                delete []buffer;
            return false;
        }
    }
    delete []buffer;
    return true;
}

bool BSPReader::readMiptex(const tBSPMipTexHeader& header, tBSPTexture& texture, unsigned char* buffer)
{
    if (header.offsets[0] == 0)
    {
        // Load the texture from a wad file, we make this an empty texture for now
        strcpy(texture.name, header.name);
        texture.width = 32;//header.width;
        texture.height = 32;//header.height;
        texture.bpp = 3;
        texture.renderIndex = 0;
		texture.flags = 3;

        int dataSize = texture.width * texture.height * 3;
		texture.data = new unsigned char[dataSize];

        for (int i = 0; i < texture.width; i++)
		{
			for (int j = 0; j < texture.height; j++)
			{
				int pixel = i*texture.width + j * 3;
				texture.data[pixel] = 255;
				texture.data[pixel+1] = 255;
				texture.data[pixel+2] = 255;
			}
		}
    }
    else
    {
        return readTextureFromMiptex(header, texture, buffer);
    }
    return true;
}

bool BSPReader::readTextureFromMiptex(const tBSPMipTexHeader& header, tBSPTexture& texture, unsigned char* buffer)
{
    int s = header.width * header.height;
    int paletteOffset = header.offsets[0] + s + (s/4) + (s/16) + (s/64) + 2;

    unsigned char* source = buffer + header.offsets[0];
    unsigned char* palette = buffer + paletteOffset;

    strcpy(texture.name, header.name);
    texture.width = header.width;
    texture.height = header.height;
    texture.bpp = 4;
    texture.renderIndex = 0;
    texture.data = new unsigned char[s * texture.bpp];

    int j = 0;
    for (int i = 0; i < s; i++)
    {
        char r, g, b, a;
        r = palette[source[i]*3];
        g = palette[source[i]*3+1];
        b = palette[source[i]*3+2];
        a = 255;

        if (palette[source[i]*3] <= 5 && palette[source[i]*3+1] <= 5 && palette[source[i]*3+2] == 255)
            r = g = b = a = 0;

        texture.data[j++] = r;
        texture.data[j++] = g;
        texture.data[j++] = b;
        texture.data[j++] = a;
    }
    return true;
}

bool BSPReader::readFaces(const tBSPHeader& header, std::istream& bsp)
{
    tBSPFace* faces = NULL;
    tBSPVertex* vertices = NULL;
    tBSPTexInfo* texinfos = NULL;
    tBSPPlane* planes = NULL;
    tBSPEdge* edges = NULL;
    int* surfedges = NULL;
    unsigned char* lightData = NULL;
    int vertexCount, texinfoCount, planeCount, edgeCount, surfedgeCount;
    
    try
    {
        this->mContent->mFaceCount = readLump(header.lumps[HL1_BSP_FACELUMP], bsp, (unsigned char**)&faces, sizeof(tBSPFace));
        vertexCount = readLump(header.lumps[HL1_BSP_VERTEXLUMP], bsp, (unsigned char**)&vertices, sizeof(tBSPVertex));
        texinfoCount = readLump(header.lumps[HL1_BSP_TEXINFOLUMP], bsp, (unsigned char**)&texinfos, sizeof(tBSPTexInfo));
        planeCount = readLump(header.lumps[HL1_BSP_PLANELUMP], bsp, (unsigned char**)&planes, sizeof(tBSPPlane));
        edgeCount = readLump(header.lumps[HL1_BSP_EDGELUMP], bsp, (unsigned char**)&edges, sizeof(tBSPEdge));
        surfedgeCount = readLump(header.lumps[HL1_BSP_SURFEDGELUMP], bsp, (unsigned char**)&surfedges, sizeof(int));
        readLump(header.lumps[HL1_BSP_LIGHTINGLUMP], bsp, (unsigned char**)&lightData, sizeof(unsigned char));
    }
    catch (const char* error)
    {
        if (faces != NULL)
            delete []faces;
        setError(error);
        return false;
    }

    this->mContent->mRenderFaces = new tBSPRenderFace[this->mContent->mFaceCount];

    for (int i = 0; i < this->mContent->mFaceCount; i++)
    {
        tBSPRenderFace& renderFace = this->mContent->mRenderFaces[i];
        const tBSPFace& bspFace = faces[i];
        const tBSPTexInfo& texinfo = texinfos[bspFace.texinfo];
        const tBSPPlane& plane = planes[bspFace.planeIndex];

        renderFace.planeSide = bspFace.side;
        renderFace.planeNormal[0] = plane.normal[0];
        renderFace.planeNormal[1] = plane.normal[1];
        renderFace.planeNormal[2] = plane.normal[2];
        renderFace.planeDistance = plane.distance;
        renderFace.texture = &this->mContent->mTextures[texinfo.miptexIndex];
        renderFace.firstVertexIndex = this->mContent->mVertices.current();
        renderFace.vertexCount = bspFace.edgeCount;
        renderFace.faceFlags = texinfo.flags;
		
        switch (bspFace.edgeCount)
        {
            // TODO: make this from defines
            case 3: renderFace.faceType = 4; break;
            case 4: renderFace.faceType = 7; break;
            default: renderFace.faceType = 9; break;
        }

		float is = 1.0f / float(renderFace.texture->width);
		float it = 1.0f / float(renderFace.texture->height);

        float min[2];
        float max[2];

        // Load the light map for this face and calculate variable for texcoord
        CalcSurfaceExtents(bspFace, texinfo, surfedges, edges, vertices, min, max);
        renderFace.lightmap = ComputeLightmap(bspFace, min, max, lightData);

        for (int e = 0; e < bspFace.edgeCount; e++)
        {
            float v[7] = { 0 };
            int edgeIndex = surfedges[bspFace.firstEdge + e];
            if (edgeIndex < 0)
            {
                edgeIndex = -edgeIndex;
                const tBSPEdge& edge = edges[edgeIndex];
                v[0] = vertices[edge.vertex[1]].point[0];
                v[1] = vertices[edge.vertex[1]].point[1];
                v[2] = vertices[edge.vertex[1]].point[2];
            }
            else
            {
                const tBSPEdge& edge = edges[edgeIndex];
                v[0] = vertices[edge.vertex[0]].point[0];
                v[1] = vertices[edge.vertex[0]].point[1];
                v[2] = vertices[edge.vertex[0]].point[2];
            }

            float d[3] = { v[0], v[1], v[2] };
            float s = DotProduct(d, texinfo.vecs[0]) + texinfo.vecs[0][3];
            float t = DotProduct(d, texinfo.vecs[1]) + texinfo.vecs[1][3];
            v[3] = s*is;
            v[4] = t*it;
			
            // compute lightmap coords
            float midPolyS = (min[0] + max[0])/2.0f;
            float midPolyT = (min[1] + max[1])/2.0f;
            float midTexS = float(renderFace.lightmap->width) / 2.0f;
            float midTexT = float(renderFace.lightmap->height) / 2.0f;
            float lightmapS = midTexS + (s - midPolyS) / 16.0f;
            float lightmapT = midTexT + (t - midPolyT) / 16.0f;
            
            v[5] = lightmapS / float(renderFace.lightmap->width);
            v[6] = lightmapT / float(renderFace.lightmap->height);

            this->mContent->mVertices.add(v);
            this->mContent->mTexCoords.add(v+3);
            this->mContent->mLightTexCoords.add(v+5);
        }
    }

    return true;
}

BSPReader::Configuration::Configuration()
        : resources(0)
{
}

BSPReader::Configuration::~Configuration()
{
}
