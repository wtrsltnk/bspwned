#include "resources.h"

ResourceFile::ResourceFile(
    const std::string &filename)
{
    name = filename;
}

ResourceFile::~ResourceFile()
{
    if (data != nullptr)
    {
        delete[] data;
        data = nullptr;
    }
}

bool ResourceFile::Open()
{
    FILE *file = fopen(name.c_str(), "rb");

    if (file == 0)
    {
        throw "Cannot open file";
    }

    fseek(file, 0, SEEK_END);
    this->dataSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    this->data = new unsigned char[this->dataSize];
    fread(this->data, this->dataSize, 1, file);

    fclose(file);

    return true;
}

long ResourceFile::DataSize() const
{
    return dataSize;
}

unsigned char *ResourceFile::Data() const
{
    return data;
}

IData *ResourceFile::copy() const
{
    auto result = new ResourceFile(name);

    result->dataSize = dataSize;
    result->data = new unsigned char[dataSize];
    memcpy(result->data, data, dataSize);

    return result;
}

ResourceManager::ResourceManager(
    const std::string &wadRoot)
{
    _wadRoot = std::filesystem::path(wadRoot);
}

ResourceManager::~ResourceManager() = default;

std::string ResourceManager::findFile(
    const std::string &name)
{
    std::string findName;

    if (findFileInFolder(_wadRoot, name, findName))
    {
        return findName;
    }

    return "";
}

IData *ResourceManager::openFile(
    const std::string &filename)
{
    ResourceFile *file = files[filename];

    if (file != NULL)
    {
        file->ref++;

        return file;
    }
    else
    {
        file = new ResourceFile(filename.c_str());

        files.insert(make_pair(filename, file));

        file->ref++;

        file->Open();

        return file;
    }
}

bool ResourceManager::openFileAsTexture(
    Texture &texture,
    const std::string &filename)
{
    bool result = false;
    auto foundFile = std::filesystem::path(findFile(filename));

    if (foundFile.empty())
    {
        return false;
    }

    const IData *file = openFile(foundFile.generic_string());

    if (file == NULL)
    {
        return false;
    }

    if (foundFile.extension() == ".tga")
    {
        result = openTarga(texture, file, foundFile.generic_string());
    }

    closeFile(file);

    return true;
}

void ResourceManager::closeFile(
    const IData *file)
{
    ResourceFile *listFile = files[file->name];
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

bool ResourceManager::findFileInFolder(
    const std::filesystem::path &dir,
    const std::string &file,
    std::string &result)
{
    bool found = false;
    std::string strFile = file;

    if (!std::filesystem::exists(dir))
    {
        return false;
    }

    for (auto &p : std::filesystem::directory_iterator(dir))
    {
        if (p.is_directory())
        {
            if (findFileInFolder(p.path().generic_string(), file, result))
            {
                return true;
            }
        }
        else
        {
            if (p.path().filename().generic_string() == std::string(file))
            {
                result = p.path().generic_string();
                return true;
            }
        }
    }

    return false;
}

unsigned int ResourceManager::addTexture(const Texture &texture)
{
    GLuint renderIndex = 0;
    GLuint format = GL_RGB;

    switch (texture.bpp)
    {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
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
