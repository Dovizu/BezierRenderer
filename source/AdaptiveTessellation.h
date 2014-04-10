//
//  AdaptiveTessellation.h
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/9/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#ifndef __BezierSurfaceRenderer__AdaptiveTessellation__
#define __BezierSurfaceRenderer__AdaptiveTessellation__

#include "utilities.h"
#include "Tessellation.h"

/* Reference:
 typedef struct {
 size_t numOfVertices;
 size_t numOfIndices;
 Vector *vertices;
 int *indices;
 } Mesh;
 
 typedef struct {
 size_t numberOfPatches;
 Vector *controlPoints;
 } BezierObject;
 */

class AdaptiveTessellation : public Tessellation{
    
public:
    AdaptiveTessellation();
    void tessellate(vector<BezierObject>& bezierObjects,
                    vector<Mesh>& meshes);
private:
    Vector evaluateBezierCurve(const Vector *ctrPts, const float &t);
    Vector evaluateBezierPatch(const Vector *controlPoints,
                               const float &u,
                               const float &v);
};

#endif /* defined(__BezierSurfaceRenderer__AdaptiveTessellation__) */
