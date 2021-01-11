/* 
 * File:   interfaces.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 9:12 PM
 */

#ifndef _INTERFACES_H
#define	_INTERFACES_H

#include <string.h>
#include <iostream>
using namespace std;

class IData
{
public:
    IData() : name(0), data(0), dataSize(0) { }
    virtual ~IData()
    {
        if (name != 0) delete []name;
        if (data != 0) delete []data;
    }

    char* name;
    long dataSize;
    unsigned char* data;

    template<typename T>
    bool read(T* array, int count = 1, long offset = 0) const
    {
        // Check if the file is big enough to contain the data you want to read
        if ((sizeof(T) * count) + offset > dataSize)
            return false;

        // Cast the data to the new Type and jump to the right offset
        T* tmp = (T*)(data + offset);

        // Copy all the elements you need from the
        for (int i = 0; i < count; i++)
            array[i] = tmp[i];

        return true;
    }

    IData* copy() const
    {
        IData* result = new IData();
        result->name = new char[strlen(name) + 1];
        strcpy(result->name, name);
        result->dataSize = dataSize;
        result->data = new unsigned char[dataSize];
        memcpy(result->data, data, dataSize);
        return result;
    }
};

#define MIPLEVELS 4

class Texture
{
public:
    Texture() : name(0), width(0), height(0), bpp(0), mipmapping(false), repeat(true)
    {
        for (int i = 0; i < MIPLEVELS; i++)
            data[i] = 0;
    }
    virtual ~Texture()
    {
        if (name != 0) delete []name;
        for (int i = 0; i < MIPLEVELS; i++)
            if (data[i] != 0)
                delete []data[i];
    }

    char* name;
    int width;
    int height;
    int bpp;
    bool mipmapping;
    bool repeat;
    unsigned char* data[4];
    unsigned int glIndex;

};

class IResources
{
public:
    virtual ~IResources() {}

    virtual unsigned int addTexture(const Texture& texture) = 0;

    virtual const char* findFile(const char* filename) = 0;
    virtual const IData* openFile(const char* filename) = 0;
    virtual const bool openFileAsTexture(Texture& texture, const char* filename) = 0;
    virtual void closeFile(const IData* file) = 0;
	
};

class IFrustum
{
public:
    virtual ~IFrustum() {}

    virtual bool cullBoundingBox(float mins[3], float maxs[3]) const = 0;
};

class IPhysics
{
public:
    virtual ~IPhysics() {}
    
    virtual void setLeafSpacesCount(int count) = 0;
    virtual void addToLeafSpace(int leafSpaceIndex, float* vertexData, int vertexCount, int* indexData, int indexCount) = 0;
    virtual void createPlane(const float plane[4]) = 0;
    virtual void* createSphere(float radius, float mass, const float position[3], const float force[3], const float ang[3]) = 0;
    virtual void getGeomPosition(void* geom, float position[3]) = 0;

    virtual void step(double time) = 0;

};

#endif	/* _INTERFACES_H */

