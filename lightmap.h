/* 
 * File:   lightmap.h
 * Author: Wouter
 *
 * Created on April 10, 2009, 6:00 PM
 */

#ifndef _LIGHTMAP_H
#define	_LIGHTMAP_H

#include "types.h"
#include "interfaces.h"

void CalcSurfaceExtents(const tBSPFace& bspFace, const tBSPTexInfo& tex, int* surfedges, const tBSPEdge* edges, const tBSPVertex* vertices, float min[2], float max[2]);
Texture* ComputeLightmap(const tBSPFace& bspFace, float min[2], float max[2], const unsigned char* lightData);

#endif	/* _LIGHTMAP_H */

