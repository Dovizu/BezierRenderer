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
#include "UnitTest.cpp"
#include "BEZParser.h"
#include "Tessellation.h"
#include "UniformTessellation.h"
#include "AdaptiveTessellation.h"

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

void rasterizeMeshes(vector<Mesh>& meshes, vector<RasterMesh>& rasters);
void renderToOpenGL3(vector<RasterMesh>& meshes);
void renderToOpenGL(int argc, char *argv[]);

vector<RasterMesh> rasterMeshes;
vector<BezierObject> objects;
vector<Mesh> meshes;
vector<Matrix4f> transformations;

bool keyStates[256] = {false}; // Create an array of boolean values of length 256 (0-255)

bool shadeToggle = false;
ShadeMode shadeMode = SMOOTH; //enum SMOOTH or FLAT
bool fillToggle = false;
FillMode fillMode = FILLED; //enum FILLED or WIREFRAME
bool gTransChanged = false;
bool adaptive = false;
bool opengl3 = false;

//Global transformation and menu select
int currentObj = 0;
#define gTrans transformations.at(currentObj)
int menuHandle = -1;

#endif /* defined(__BezierSurfaceRenderer__renderBezier__) */
