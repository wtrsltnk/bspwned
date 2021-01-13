/* 
 * File:   interfaces.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 9:12 PM
 */

#ifndef _INTERFACES_H
#define _INTERFACES_H

#include <iostream>
#include <string>

bool iequals(
    const std::string &a,
    const std::string &b);

class IData
{
public:
    virtual ~IData() = default;

    std::string name;

    virtual long DataSize() const = 0;

    virtual unsigned char *Data() const = 0;

    virtual IData *copy() const = 0;

    template <typename T>
    bool read(T *array, int count = 1, long offset = 0) const
    {
        // Check if the file is big enough to contain the data you want to read
        if ((sizeof(T) * count) + offset > DataSize())
            return false;

        // Cast the data to the new Type and jump to the right offset
        T *tmp = (T *)(Data() + offset);

        // Copy all the elements you need from the
        for (int i = 0; i < count; i++)
            array[i] = tmp[i];

        return true;
    }
};

#define MIPLEVELS 4

class Texture
{
public:
    Texture() = default;

    virtual ~Texture()
    {
        ClearData();
    }

    void ClearData()
    {
        for (int i = 0; i < MIPLEVELS; i++)
        {
            if (data[i] != nullptr)
            {
                delete[] data[i];
                data[i] = nullptr;
            }
        }
    }

    std::string name;
    int width = 0;
    int height = 0;
    int bpp = 0;
    bool repeat = true;
    unsigned char *data[MIPLEVELS] = {nullptr, nullptr, nullptr, nullptr};
    unsigned int glIndex = 0;
};

class IResources
{
public:
    virtual ~IResources() {}

    virtual unsigned int addTexture(
        const Texture &texture) = 0;

    virtual std::string findFile(
        const std::string &filename) = 0;

    virtual IData *openFile(
        const std::string &filename) = 0;

    virtual bool openFileAsTexture(
        Texture &texture,
        const std::string &filename) = 0;

    virtual void closeFile(
        const IData *file) = 0;
};

class IFrustum
{
public:
    virtual ~IFrustum() {}

    virtual bool cullBoundingBox(
        float mins[3],
        float maxs[3]) const = 0;
};

class IPhysics
{
public:
    virtual ~IPhysics() {}

    virtual void setLeafSpacesCount(
        int count) = 0;

    virtual void addToLeafSpace(
        int leafSpaceIndex,
        float *vertexData,
        int vertexCount,
        int *indexData,
        int indexCount) = 0;

    virtual void createPlane(
        const float plane[4]) = 0;

    virtual void *createSphere(
        float radius,
        float mass,
        const float position[3],
        const float force[3],
        const float ang[3]) = 0;

    virtual void getGeomPosition(
        void *geom,
        float position[3]) = 0;

    virtual void step(
        double time) = 0;
};

#endif /* _INTERFACES_H */
