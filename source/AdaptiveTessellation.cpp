//
//  AdaptiveTessellation.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/9/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "AdaptiveTessellation.h"

AdaptiveTessellation::AdaptiveTessellation() {}

void AdaptiveTessellation::tessellate(vector<BezierObject>& bezierObjects,
                vector<Mesh>& meshes) {
    for (auto & object : bezierObjects) {
        Mesh mesh = {AdaptiveMesh, 0, 0, NULL, NULL, new vector<Vector>};
        //calculate fucking Bezier patches, getting all patchy patchy
        Vector *patchControlPoints;
        for (int currPatch=0; currPatch<object.numberOfPatches; ++currPatch) {
            patchControlPoints = object.controlPoints+currPatch*16;
            /*
            uv=(0,1)    Patch      uv=(1,1)           
                   D               C                  
                    +–––––––––––––+                   
                    |             |       v+          
                    |             |        |          
                    |             |        |          
                    |             |        |          
                    |             |        +–––––––––+
                    +–––––––––––––+                  u
                   A               B                  
                 uv=(0,0)       uv=(1,0)              
            */
            ParametricPoint A(0,0);
            ParametricPoint B(1,0);
            ParametricPoint C(1,1);
            ParametricPoint D(0,1);
        }
    }
}

Vector AdaptiveTessellation::evaluateBezierCurve(const Vector *ctrPts, const float &t) {
    float b0 = (1 - t) * (1 - t) * (1 - t);
    float b1 = 3 * t * (1 - t) * (1 - t);
    float b2 = 3 * t * t * (1 - t);
    float b3 = t * t * t;
    return ctrPts[0] * b0 + ctrPts[1] * b1 + ctrPts[2] * b2 + ctrPts[3] * b3;
}

Vector AdaptiveTessellation::evaluateBezierPatch(const Vector *controlPoints,
                                                const float &u,
                                                const float &v) {
    Vector uCurve[4];
    for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, u);
    return evaluateBezierCurve(uCurve, v);
}