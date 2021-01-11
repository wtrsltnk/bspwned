/* 
 * File:   resources.h
 * Author: wouter
 *
 * Created on May 25, 2009, 4:57 PM
 */

#ifndef _RESOURCES_H
#define	_RESOURCES_H

#include <dirent.h>
#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <map>
#include <stdio.h>
#include "opengl.h"
#include "stringcompare.h"
#include "interfaces.h"

class File;
typedef std::map<const char*, File*> FileList;

bool openTarga(Texture& texture, const IData* data, const char* filename);

class File : public IData
{
public:
    File(const char* filename)
                    : IData(), ref(0)
    {
        name = new char[strlen(filename) + 1];
        strcpy(name, filename);

        FILE* file = fopen(filename, "rb");
        if (file != 0)
        {
            fseek(file, 0, SEEK_END);
            this->dataSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            this->data = new unsigned char[this->dataSize];
            fread(this->data, this->dataSize, 1, file);

            fclose(file);
        }
        else throw "Cannot open file";
    }
    virtual ~File() { }
    int ref;
    
};

class ResourceManager : public IResources
{
protected:
    char wadRoot[256];
    FileList files;
    char findName[256];

public:
    ResourceManager(const char* wadRoot) { strcpy(this->wadRoot, wadRoot); }
    virtual ~ResourceManager() {}

    virtual const char* findFile(const char* name)
    {
        if (ResourceManager::findFileInFolder(wadRoot, name, findName))
        {
            return findName;
        }
        return NULL;
    }

    virtual const IData* openFile(const char* filename)
    {
        File* file = files[filename];
        if (file != NULL)
        {
            file->ref++;
            return file;
        }
        else
        {
            file = new File(filename);
            files.insert(make_pair(filename, file));

            file->ref++;
            return file;
        }
        return NULL;
    }

    virtual const bool openFileAsTexture(Texture& texture, const char* filename)
    {
        bool result = false;
        const char* fullname = findFile(filename);\
        
        if (fullname != NULL)
        {
            const IData* file = openFile(fullname);

            if (file != NULL)
            {
                str ext = { strstr(filename, ".") };

                if (ext == ".tga")
                {
                    result = openTarga(texture, file, filename);
                }
                closeFile(file);
                result = true;
            }
        }

        return result;
    }

    virtual void closeFile(const IData* file)
    {
        File* listFile = files[file->name];
        if (listFile != NULL)
        {
            listFile->ref--;
            if (listFile->ref == 0)
            {
                files.erase(file->name);
                delete listFile;
            }
        }
    }

    static bool findFileInFolder(const char* folder, const char* file, char* result)
    {
        bool found = false;
        DIR* dir = opendir(folder);
        string strFile = file;

        if (dir != 0)
        {
            struct dirent *item;
            while ((item = readdir(dir)) != 0)
            {
                if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0)
                    continue;

#ifdef WIN32
                struct stat statBuf;
                char full[256] = { 0 };
                strcpy(full, folder);
                strcat(full, "/");
                strcat(full, item->d_name);
                stat(full, &statBuf);
                if (S_ISDIR(statBuf.st_mode))
#else
                if (item->d_type == 4)	// FOLDER
#endif
                {
                    char newFolder[256] = { 0 };
                    strcpy(newFolder, folder);
                    strcat(newFolder, "/");
                    strcat(newFolder, item->d_name);
                    if (ResourceManager::findFileInFolder(newFolder, file, result))
                    {
                        found = true;
                        break;
                    }
                }
                else
                {
                    str l, r;
                    l.str = item->d_name;
                    r.str = file;
                    if (l == r)
                    {
                        strcpy(result, folder);
                        strcat(result, "/");
                        strcat(result, item->d_name);
                        found = true;
                        break;
                    }
                }
            }
            closedir(dir);
        }
        return found;
    }

    virtual unsigned int addTexture(const Texture& texture)
    {
        GLuint renderIndex = 0;
        GLuint format = GL_RGB;

        switch (texture.bpp)
        {
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
        }

	glGenTextures(1, &renderIndex);
	glBindTexture(GL_TEXTURE_2D, renderIndex);

        if (texture.repeat)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        
        if (texture.mipmapping)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        }


        glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data[0]);
        if (texture.mipmapping)
        {
            glTexImage2D(GL_TEXTURE_2D, 1, format, texture.width / 2, texture.height / 2, 0, format, GL_UNSIGNED_BYTE, texture.data[1]);
            glTexImage2D(GL_TEXTURE_2D, 2, format, texture.width / 8, texture.height / 8, 0, format, GL_UNSIGNED_BYTE, texture.data[2]);
            glTexImage2D(GL_TEXTURE_2D, 3, format, texture.width / 32, texture.height / 32, 0, format, GL_UNSIGNED_BYTE, texture.data[3]);
        }

        return renderIndex;
    }
};

#endif	/* _RESOURCES_H */

