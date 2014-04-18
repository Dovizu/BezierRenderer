//
//  renderBezier.h
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/3/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#ifndef __BezierSurfaceRenderer__renderBezier__
#define __BezierSurfaceRenderer__renderBezier__

#include "utilities.h"

typedef struct {
    size_t numOfIndices;
    size_t numOfVertices;
    float *vertices;
    int *indices;
} RasterMesh;

typedef enum {
    FLAT,
    SMOOTH
} ShadeMode;

typedef enum {
    FILLED,
    WIREFRAME
} FillMode;

#endif /* defined(__BezierSurfaceRenderer__renderBezier__) */
