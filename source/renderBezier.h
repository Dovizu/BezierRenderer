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

GLuint loadShaderFromFile( std::string path, GLenum shaderType );

#endif /* defined(__BezierSurfaceRenderer__renderBezier__) */
