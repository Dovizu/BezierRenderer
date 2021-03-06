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

typedef Array2f ParametricPoint;

class AdaptiveTessellation : public Tessellation{
    
public:
    AdaptiveTessellation();
    void tessellate(vector<BezierObject>& bezierObjects,
                    vector<Mesh>& meshes);
private:
    void evaluateAdaptiveTriangle(ParametricPoint& A,
                                  ParametricPoint& B,
                                  ParametricPoint& C,
                                  const Vector *ctrPts,
                                  Mesh& mesh,
                                  int depth);
};

#endif /* defined(__BezierSurfaceRenderer__AdaptiveTessellation__) */
