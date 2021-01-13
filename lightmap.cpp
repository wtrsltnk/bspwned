#include "lightmap.h"

#include <math.h>
#include <memory.h>
//
// the following computations are based on:
// PolyEngine (c) Alexey Goloshubin and Quake I source by id Software
//

void CalcSurfaceExtents(
    const tBSPFace &bspFace,
    const tBSPTexInfo &texinfo,
    int *surfedges,
    const tBSPEdge *edges,
    const tBSPVertex *vertices,
    float min[2],
    float max[2])
{
    const tBSPVertex *vertex;

    min[0] = min[1] = 999999;
    max[0] = max[1] = -999999;

    for (int i = 0; i < bspFace.edgeCount; i++)
    {
        int edgeIndex = surfedges[bspFace.firstEdge + i];

        if (edgeIndex >= 0)
            vertex = &vertices[edges[edgeIndex].vertex[0]];
        else
            vertex = &vertices[edges[-edgeIndex].vertex[1]];

        for (int j = 0; j < 2; j++)
        {
            float value = (vertex->point[0] * texinfo.vecs[j][0]) +
                          (vertex->point[1] * texinfo.vecs[j][1]) +
                          (vertex->point[2] * texinfo.vecs[j][2]) +
                          texinfo.vecs[j][3];

            // Save the boundings
            if (value < min[j]) min[j] = value;
            if (value > max[j]) max[j] = value;
        }
    }
}

Texture *ComputeLightmap(
    const tBSPFace &bspFace,
    float min[2],
    float max[2],
    const unsigned char *lightData)
{
    Texture *result = new Texture;

    // compute lightmap size
    int size[2];
    for (int c = 0; c < 2; c++)
    {
        float tmin = floorf(min[c] / 16.0f);
        float tmax = ceilf(max[c] / 16.0f);

        size[c] = (int)(tmax - tmin);
    }

    result->width = size[0] + 1;
    result->height = size[1] + 1;
    result->bpp = 3;

    int dataSize = result->width * result->height * result->bpp; // RGB buffer size

    result->data[0] = new unsigned char[dataSize];

    memcpy(result->data[0], lightData + bspFace.lightOffset, dataSize);

    float light_adjust = 1.0f; // - config.brightness;

    // scale lightmap value...
    for (int i = 0; i < dataSize; i++)
    {
        float f = powf((result->data[0][i] + 1) / 256.0f, light_adjust);
        int inf = f * 255.0f + 0.5f;

        // clamp between 0 and 255
        if (inf < 0) inf = 0;
        if (inf > 255) inf = 255;

        result->data[0][i] = inf;
    }
    return result;
}
