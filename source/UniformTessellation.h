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

//typedef struct {
//    size_t numOfVertices;
//    size_t numOfIndices;
//    Vector *vertices;
//    int *indices;
//} Mesh;

class UniformTessellation : public Tessellation{
    
public:
    UniformTessellation();
    void tessellate(vector<BezierObject>& bezierObjects,
                    vector<Mesh>& meshes);
private:
    Vector evaluateBezierCurve(const Vector *ctrPts, const float &t);
    Vector evaluateBezierPatch(const Vector *controlPoints,
                               const float &u,
                               const float &v);
};

#endif /* defined(__BezierSurfaceRenderer__UniformTessellation__) */
