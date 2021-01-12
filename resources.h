/* 
 * File:   resources.h
 * Author: wouter
 *
 * Created on May 25, 2009, 4:57 PM
 */

#ifndef _RESOURCES_H
#define _RESOURCES_H

#include "opengl.h"

#include "interfaces.h"

#include <filesystem>
#include <map>
#include <stdio.h>
#include <string>

bool openTarga(
    Texture &texture,
    const IData *data,
    const std::string &filename);

class ResourceFile : public IData
{
public:
    ResourceFile(
        const std::string &filename);

    virtual ~ResourceFile();

    virtual long DataSize() const;

    virtual unsigned char *Data() const;

    virtual IData *copy() const;

    bool Open();

    int ref = 0;

private:
    long dataSize = 0;
    unsigned char *data = nullptr;
};

typedef std::map<std::string, ResourceFile *> FileList;

class ResourceManager : public IResources
{
public:
    ResourceManager(
        const std::string &wadRoot,
        const std::string &mod);

    virtual ~ResourceManager();

    virtual std::string findFile(
        const std::string &name);

    virtual IData *openFile(
        const std::string &filename);

    virtual bool openFileAsTexture(
        Texture &texture,
        const std::string &filename);

    virtual void closeFile(
        const IData *file);

    static bool findFileInFolder(
        const std::filesystem::path &dir,
        const std::string &file,
        std::filesystem::path &result);

    virtual unsigned int addTexture(const Texture &texture);

protected:
    std::filesystem::path _wadRoot;
    std::string _mod;
    FileList files;
};

#endif /* _RESOURCES_H */
