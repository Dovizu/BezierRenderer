//
//  UniformTessellation.h
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/5/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#ifndef __BezierSurfaceRenderer__UniformTessellation__
#define __BezierSurfaceRenderer__UniformTessellation__

#include "utilities.h"
#include "Tessellation.h"

/* Reference:
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
 
 typedef struct {
 size_t numberOfPatches;
 Vector *controlPoints;
 } BezierObject;
 */

class UniformTessellation : public Tessellation{
    
public:
    UniformTessellation();
    void tessellate(vector<BezierObject>& bezierObjects,
                    vector<Mesh>& meshes);
};

#endif /* defined(__BezierSurfaceRenderer__UniformTessellation__) */
