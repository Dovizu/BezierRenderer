//
//  Tessellation.h
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/5/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#ifndef __BezierSurfaceRenderer__Tessellation__
#define __BezierSurfaceRenderer__Tessellation__

#include "utilities.h"
#include "BEZParser.h"

typedef enum {
    UniformMesh,
    AdaptiveMesh
} MeshType;

typedef struct {
    MeshType type;
    size_t numOfVertices;
    size_t numOfIndices;
    //for uniform tessellation
    Vector *vertices;
    int *indices;
    //for adaptive tessellation
    vector<Vector> *adaptiveVertices;
} Mesh;

class Tessellation {
public:
    float error = 0.5;
    int divs = 16;
    
    void setDivDepth(int d);
    void setErrorRate(float e);
    virtual void tessellate(vector<BezierObject>& bezierObjects,
                            vector<Mesh>& meshes) = 0;
};



#endif /* defined(__BezierSurfaceRenderer__Tessellation__) */
