/* 
 * File:   bsp_types.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:29 PM
 */

#ifndef _BSP_TYPES_H
#define	_BSP_TYPES_H

#define HL1_BSP_SIGNATURE 30
#define HL1_BSP_LUMPCOUNT 15

#define HL1_BSP_ENTITYLUMP 0
#define HL1_BSP_PLANELUMP 1
#define HL1_BSP_TEXTURELUMP 2
#define HL1_BSP_VERTEXLUMP 3
#define HL1_BSP_VISIBILITYLUMP 4
#define HL1_BSP_NODELUMP 5
#define HL1_BSP_TEXINFOLUMP 6
#define HL1_BSP_FACELUMP 7
#define HL1_BSP_LIGHTINGLUMP 8
#define HL1_BSP_CLIPNODELUMP 9
#define HL1_BSP_LEAFLUMP 10
#define HL1_BSP_MARKSURFACELUMP 11
#define HL1_BSP_EDGELUMP 12
#define HL1_BSP_SURFEDGELUMP 13
#define HL1_BSP_MODELLUMP 14

#define HL1_WAD_SIGNATURE "WAD3"

#define	MAX_MIP_LEVELS 4
#define MAX_MAP_HULLS 4
#define MAX_LIGHT_MAPS 4
#define MAX_AMBIENTS 4

// Plane types (0-2 are axial planes, 3-5 are non-axial planes snapped to the nearest)
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_ANYX 3
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

#pragma pack(push, 4)

/* BSP */
typedef struct sBSPLump
{
    int offset;
    int size;

} tBSPLump;

typedef struct sBSPHeader
{
    int signature;
    tBSPLump lumps[HL1_BSP_LUMPCOUNT];

} tBSPHeader;

typedef struct sBSPMipTexOffsetTable
{
    int miptexCount;
    int offsets[1];             /* an array with "miptexcount" number of offsets */

} tBSPMipTexOffsetTable;

typedef struct sBSPMipTexHeader
{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} tBSPMipTexHeader;


typedef struct sBSPModel
{
    float mins[3], maxs[3];
    float origin[3];
    int headnode[MAX_MAP_HULLS];
    int visLeafs;                       // not including the solid leaf 0
    int firstFace;
    int faceCount;

} tBSPModel;

typedef struct sBSPVertex
{
    float point[3];

} tBSPVertex;

typedef struct sBSPEdge
{
    unsigned short vertex[2];

} tBSPEdge;

typedef struct sBSPFace
{
    short planeIndex;
    short side;
    int firstEdge;
    short edgeCount;
    short texinfo;

    // lighting info
    unsigned char styles[MAX_LIGHT_MAPS];
    int lightOffset;                    // start of [numstyles*surfsize] samples

} tBSPFace;

typedef struct sBSPPlane
{
    float normal[3];
    float distance;
    int type;

} tBSPPlane;

typedef struct sBSPNode
{
    int planeIndex;
    short children[2];                  // negative numbers are -(leafs+1), not nodes
    short mins[3];                      // for sphere culling
    short maxs[3];
    unsigned short firstFace;
    unsigned short faceCount;            // counting both sides

} tBSPNode;

typedef struct sBSPClipNode
{
    int planeIndex;
    short children[2];                  // negative numbers are contents

} tBSPClipNode;

// This is the information for calculating the texture cooridnates on ta face
typedef struct sBSPTexInfo
{
    float    vecs[2][4];                // [s/t][xyz offset]
    int      miptexIndex;
    int      flags;
    
} tBSPTexInfo;

typedef struct sBSPLeaf
{
    int contents;
    int visofs;                         // -1 = no visibility info

    short mins[3];                      // for frustum culling
    short maxs[3];

    unsigned short firstMarkSurface;
    unsigned short markSurfacesCount;

    unsigned char ambientLevel[MAX_AMBIENTS];

} tBSPLeaf;

typedef struct sBSPIndexList
{
    int size;
    int* indices;
    
} tBSPIndexList;

typedef struct sWADHeader
{
    char signature[4];
    int lumpsCount;
    int lumpsOffset;

} tWADHeader;

typedef struct sWADLump
{
    int offset;
    int sizeOnDisk;
    int size;
    char type;
    char compression;
    char empty0;
    char empty1;
    char name[16];

} tWADLump;

typedef struct sEntityValue
{
    char* key;
    char* value;

} tEntityValue;

typedef struct sEntity
{
    char className[64];
    int valueCount;
    tEntityValue* values;

} tEntity;

typedef struct sVertex
{
    float xyz[3];		// The position of this vertex
    float st[2];		// The texture coordinates
    float lslt[2];		// The lightmap texture coordinates

} tVertex;

typedef struct sModel
{
    int fxMode;             // The mode of rendering this model: Color, Glow, Additive, Solid, Normal or Texture blending
    float fxAmount;         // The transparency amount
    float fxColor[3];       // The color to render this model in
    float origin[3];        // The origin of the model (don't use it in the displaylist)
    unsigned int displaylistIndex;   // The index of the displaylist contianing this model

} tModel;

// Forward declaration of the Texture class defined in interfaces.h
class Texture;

#define SHADER_FLAG_USETEXTURE              0x00000001
#define SHADER_FLAG_SPECIALTEXTURE          0x00000002
#define SHADER_FLAG_TRANSPARENTTEXTURE      0x00000004
#define SHADER_FLAG_WATERTEXTURE            0x00000008
#define SHADER_FLAG_SOLID                   0x00000010
#define SHADER_FLAG_ILLUSIONARY             0x00000020
#define SHADER_FLAG_BLEND                   0x00000200

typedef struct sShader
{
    int textureIndex;
    float fxAmount;
    float fxColor[3];
    int flags;
    int sourceBlend;
    int destBlend;
    float alphaTest;
    
} tShader;

typedef struct sFace
{
    int firstVertex;        // Index of the first vertex to render
    int vertexCount;        // Number of vertices to render
    int faceFlags;          // Flags for specifing special faces types
    int faceType;           // The type of face: GL_TRIANGLES, GL_QUADS, GL_POLYGONS

    int modelIndex;              // The index of the model it belongs to

    short planeSide;        // Side of the plane this face is on
    float planeNormal[3];   // Normal of the plane this face is on
    float planeDistance;    // Distance of the plane

    int textureIndex;       // The index of the texture for this face
    int shaderIndex;        // The index of the shader for this face
    Texture* lightmap;

} tFace;

#pragma pack(pop)

#endif	/* _BSP_TYPES_H */

