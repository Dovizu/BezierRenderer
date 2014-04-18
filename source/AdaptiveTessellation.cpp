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
            uv=(0,1)    Patch    uv=(1,1)           
                   D             C
                    +–––––––––––+
                    |         x |       v+
                    |       x   |        |
                    |     x     |        |
                    |   x       |        |
                    | x         |        +–––––––––+
                    +–––––––––––+                  u
                   A             B
                 uv=(0,0)     uv=(1,0)
            */
            ParametricPoint A(0,0);
            ParametricPoint B(1,0);
            ParametricPoint C(1,1);
            ParametricPoint D(0,1);
            evaluateAdaptiveTriangle(A,B,C, patchControlPoints, mesh);
            evaluateAdaptiveTriangle(A,C,D, patchControlPoints, mesh);
        }
        //calculate indices (dummies)
        mesh.indices = new int[mesh.numOfIndices];
        for (int index=1; index<=mesh.numOfIndices; index++) {
            mesh.indices[index-1] = index;
        }
        meshes.push_back(mesh);
    }
}

void AdaptiveTessellation::evaluateAdaptiveTriangle(const ParametricPoint& A,
                                                    const ParametricPoint& B,
                                                    const ParametricPoint& C,
                                                    const Vector *ctrPts,
                                                    Mesh& mesh)
{
    /*
    (v1)           (v2)
      A     e2      B  
       +––––0–––––+X
       |         X
       |       X       
    e1 0     0
       |   X  e3       
       + X             
       X               
      C                
     (v3)              

    */
    //limit surface vertices
    Vector v1 = evaluateBezierPatch(ctrPts, A);
    Vector v2 = evaluateBezierPatch(ctrPts, B);
    Vector v3 = evaluateBezierPatch(ctrPts, C);
    Vector v1norm;
    Vector v2norm;
    Vector v3norm;
    
    //errors on the midpoints on edges
    bool e1, e2, e3;
    ParametricPoint e1pt = (A+C)/2.0;
    ParametricPoint e2pt = (A+B)/2.0;
    ParametricPoint e3pt = (B+C)/2.0;
    e1 = abs((evaluateBezierPatch(ctrPts, e1pt)-(v1+v3)/2).norm()) > error;
    e2 = abs((evaluateBezierPatch(ctrPts, e2pt)-(v1+v2)/2).norm()) > error;
    e3 = abs((evaluateBezierPatch(ctrPts, e3pt)-(v2+v3)/2).norm()) > error;
    unsigned int binary = e1 | e2 << 1 | e3 << 2;

    if (binary == 0b000) {
        v1norm = evaluateSurfaceNormal(ctrPts, A);
        v2norm = evaluateSurfaceNormal(ctrPts, B);
        v3norm = evaluateSurfaceNormal(ctrPts, C);
    }
    
    switch (binary) {
        case 0b001: //e1 not flat
            evaluateAdaptiveTriangle(e1pt, B, A, ctrPts, mesh);
            evaluateAdaptiveTriangle(C, B, e1pt, ctrPts, mesh);
            break;
        case 0b010: //e2 not flat
            evaluateAdaptiveTriangle(C, e2pt, A, ctrPts, mesh);
            evaluateAdaptiveTriangle(C, B, e2pt, ctrPts, mesh);
            break;
        case 0b100: //e3 not flat
            evaluateAdaptiveTriangle(A, e3pt, B, ctrPts, mesh);
            evaluateAdaptiveTriangle(C, e3pt, A, ctrPts, mesh);
            break;
        case 0b011: //e1 e2 not flat
            evaluateAdaptiveTriangle(C, B, e2pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(C, e2pt, e1pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e1pt, e2pt, A, ctrPts, mesh);
            break;
        case 0b110: //e2 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, A, ctrPts, mesh);
            evaluateAdaptiveTriangle(A, e3pt, e2pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e2pt, e3pt, B, ctrPts, mesh);
            break;
        case 0b101: //e1 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, e1pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e1pt, e3pt, B, ctrPts, mesh);
            evaluateAdaptiveTriangle(e1pt, B, A, ctrPts, mesh);
            break;
        case 0b111: //e1 e2 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, e1pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e3pt, B, e2pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e1pt, e3pt, e2pt, ctrPts, mesh);
            evaluateAdaptiveTriangle(e1pt, e2pt, A, ctrPts, mesh);
            break;
        case 0b000: //all flat
            mesh.adaptiveVertices->push_back(v1);
            mesh.adaptiveVertices->push_back(v1norm);
            mesh.adaptiveVertices->push_back(v2);
            mesh.adaptiveVertices->push_back(v2norm);
            mesh.adaptiveVertices->push_back(v3);
            mesh.adaptiveVertices->push_back(v3norm);
            mesh.numOfVertices += 6;
            mesh.numOfIndices += 3;
            break;
        default: //something's wrong
            ASSERT(false, "AdaptiveTessellation encountered unknown value");
            break;
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

Vector AdaptiveTessellation::evaluateBezierPatch(const Vector *controlPoints,
                                                 const ParametricPoint& UV) {
    return evaluateBezierPatch(controlPoints, UV(0), UV(1));
}


Vector AdaptiveTessellation::evaluateSurfaceNormal(const Vector *controlPoints,
                                                  const float &u,
                                                  const float &v)
{
    Vector partialU[4];
    Vector uCurve[4];
    
    for (int i = 0; i < 4; ++i) partialU[i] = evaluateTangent(controlPoints + 4 * i, u);
    for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, u);
    
    return (evaluateBezierCurve(partialU, v).cross(evaluateTangent(uCurve, v))).normalized();
}

Vector AdaptiveTessellation::evaluateTangent(const Vector *ctrPts, const float &t) {
    float b0 = -3 * (1 - t) * (1 - t);
    float b1 = 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
    float b2 = 6 * t * (1 - t) - 3 * t * t;
    float b3 = 3 * t * t;
    return ctrPts[0] * b0 + ctrPts[1] * b1 + ctrPts[2] * b2 + ctrPts[3] * b3;
}

Vector AdaptiveTessellation::evaluateSurfaceNormal(const Vector *controlPoints,
                                                   const ParametricPoint& UV) {
    return evaluateSurfaceNormal(controlPoints, UV(0), UV(1));
}