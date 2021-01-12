#include "resources.h"

#include <algorithm>
#include <spdlog/spdlog.h>
#include <stb_image.h>

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
    const std::string &wadRoot,
    const std::string &mod)
{
    _wadRoot = std::filesystem::path(wadRoot);
    _mod = mod;
}

ResourceManager::~ResourceManager() = default;

std::string ResourceManager::findFile(
    const std::string &name)
{
    std::filesystem::path result;

    if (findFileInFolder(_wadRoot / std::filesystem::path(_mod), name, result))
    {
        return result.string();
    }

    if (findFileInFolder(_wadRoot / std::filesystem::path("valve"), name, result))
    {
        return result.string();
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
    auto foundFile = findFile(filename);

    if (foundFile.empty())
    {
        return false;
    }

    const IData *file = openFile(foundFile);

    if (file == NULL)
    {
        return false;
    }

    auto extension = std::filesystem::path(foundFile).extension();

    if (extension == ".tga" || extension == ".bmp")
    {
        int x, y, n;
        unsigned char *data = stbi_load(foundFile.c_str(), &x, &y, &n, 0);
        if (data != nullptr)
        {
            texture.data[0] = data;
            texture.width = x;
            texture.height = y;
            texture.bpp = n;
            texture.repeat = false;

            closeFile(file);

            return true;
        }
    }

    closeFile(file);

    return false;
}

void ResourceManager::closeFile(
    const IData *file)
{
    ResourceFile *listFile = files[file->name];

    if (listFile == nullptr)
    {
        return;
    }

    listFile->ref--;

    if (listFile->ref == 0)
    {
        files.erase(file->name);
        delete listFile;
    }
}

bool iequals(
    const std::string &a,
    const std::string &b)
{
    unsigned int sz = a.size();

    if (b.size() != sz)
    {
        return false;
    }

    for (unsigned int i = 0; i < sz; ++i)
    {
        if (tolower(a[i]) != tolower(b[i]))
        {
            return false;
        }
    }

    return true;
}

bool ResourceManager::findFileInFolder(
    const std::filesystem::path &dir,
    const std::string &file,
    std::filesystem::path &result)
{
    std::string strFile = file;

    if (!std::filesystem::exists(dir))
    {
        return false;
    }

    for (auto &p : std::filesystem::directory_iterator(dir))
    {
        if (p.is_directory())
        {
            if (findFileInFolder(p.path(), file, result))
            {
                return true;
            }
        }
        else
        {
            std::string a = p.path().filename().string();
            std::string b = std::filesystem::path(file).filename().string();

            if (!iequals(a, b))
            {
                continue;
            }

            result = p.path();

            return true;
        }
    }

    return false;
}

unsigned int ResourceManager::addTexture(
    const Texture &texture)
{
    GLuint renderIndex = 0;
    GLuint format = GL_RGB;

    switch (texture.bpp)
    {
        default:
        case 3:
        {
            format = GL_RGB;
            break;
        }
        case 4:
        {
            format = GL_RGBA;
            break;
        }
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

    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data[0]);

    return renderIndex;
}
