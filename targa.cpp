#include "resources.h"

#pragma pack(1)

typedef struct sTGAHeader
{
    unsigned char idLength;
    unsigned char colormapType;
    unsigned char imageType;
    unsigned short colormapIndex;
    unsigned short colormapLength;
    unsigned char colormapSize;
    unsigned short xOrigin, yOrigin;
    unsigned short width, height;
    unsigned char pixelSize;
    unsigned char attributes;
} tTGAHeader;

#pragma pack()

bool openTarga(
    Texture& texture,
    const IData* data,
    const std::string& filename)
{
    tTGAHeader* header = (tTGAHeader*)data->Data();

    texture.bpp = header->pixelSize >> 3;
    texture.width = header->width;
    texture.height = header->height;
    int newDataSize = header->width * header->height * texture.bpp;
    texture.data[0] = new unsigned char[newDataSize];
    memset(texture.data[0], 155, newDataSize);
    unsigned char* imageData = data->Data() + sizeof(tTGAHeader) + header->idLength;

    if (header->imageType == 9 || header->imageType == 10 || header->imageType == 11)
    {
/*			// Compressed data
        unsigned char* scanline = new unsigned char[header->width * bpp];
        int offset = 0;

        for (int i = 0; i < header->height; i++)
        {
            if (TGADecodeScanLine(scanline, header->width, bpp, imageData) < 0)
                return NULL;

            memcpy(&data[offset], scanline, header->width * bpp);
            offset += (header->width * bpp);
        }

        free(scanline);
*/
    }
    else
    {
        // Uncompressed data
        if (data->DataSize() >= sizeof(tTGAHeader) + header->idLength + newDataSize)
        {
            memcpy(texture.data[0], imageData, newDataSize);
        }
        else
        {
            printf("Size of TGA to small for image from TGA header\n");
            return 0;
        }
    }

    if (texture.bpp > 1)
    {
          for (unsigned int c = 0; c < newDataSize; c += texture.bpp)
          {
             unsigned char t = texture.data[0][c];
             texture.data[0][c] = texture.data[0][c+2];
             texture.data[0][c+2] = t;
          }
    }

    //if (header->attributes & 0x20)  // bottom up, need to swap scanlines
    {
        unsigned char *temp = new unsigned char[header->width * texture.bpp];

        for (int i = 0; i < header->height/2; i++)
        {
            memcpy(temp, texture.data[0] + i*header->width*texture.bpp, header->width*texture.bpp);
            memcpy(texture.data[0] + i*header->width*texture.bpp,
                    texture.data[0] + (header->height-i-1)*header->width*texture.bpp, header->width*texture.bpp);
            memcpy(texture.data[0] + (header->height-i-1)*header->width*texture.bpp, temp, header->width*texture.bpp);
        }

        delete []temp;
    }

    return false;
}
