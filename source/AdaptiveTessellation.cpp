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
//            Vector v1 = evaluateBezierPatch(patchControlPoints, A);
//            Vector v2 = evaluateBezierPatch(patchControlPoints, B);
//            Vector v3 = evaluateBezierPatch(patchControlPoints, C);
//            Vector v4 = evaluateBezierPatch(patchControlPoints, D);
//            
//            if ((v1-v2).norm()==0) {
//                A = A + 0.01;
//                B(0) = B(0) - 0.01;
//                B(1) = B(1) + 0.01;
//            }else if ((v2-v3).norm()==0) {
//                B(0) = B(0) - 0.01;
//                B(1) = B(1) + 0.01;
//                C = C - 0.01;
//            }else if ((v3-v4).norm()==0) {
//                C = C + 0.01;
//                D(0) = D(0) + 0.01;
//                D(1) = D(1) - 0.01;
//            }else if ((v4-v1).norm()==0) {
//                D(0) = D(0) + 0.01;
//                D(1) = D(1) - 0.01;
//                A = A + 0.01;
//            }
            evaluateAdaptiveTriangle(A,B,C, patchControlPoints, mesh, 0);
            evaluateAdaptiveTriangle(A,C,D, patchControlPoints, mesh, 0);
        }
        //calculate indices (dummies)
        mesh.indices = new int[mesh.numOfIndices];
        for (int index=0; index < mesh.numOfIndices; index++) {
            mesh.indices[index] = index;
        }
        meshes.push_back(mesh);
    }
}

void AdaptiveTessellation::evaluateAdaptiveTriangle(ParametricPoint& A,
                                                    ParametricPoint& B,
                                                    ParametricPoint& C,
                                                    const Vector *ctrPts,
                                                    Mesh& mesh,
                                                    int depth)
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
    bool degenerate12 = (v1-v2).norm()==0;
    bool degenerate13 = (v1-v3).norm()==0;
    bool degenerate23 = (v2-v3).norm()==0;
    if (degenerate23 || degenerate12 || degenerate13) {
        A = A + 0.01;
        B = B + 0.01;
        C = C + 0.01;
    }
//    if (degenerate12) {
//        while (degenerate12) {
//            A = A + 0.1;
//            B = B - 0.1;
//            degenerate12 = (evaluateBezierPatch(ctrPts, A)-evaluateBezierPatch(ctrPts, B)).norm()==0;
//        }
//    }
//    if (degenerate13) {
//        while (degenerate13) {
//            A = A + 0.1;
//            C = C + 0.1;
//            degenerate13 = (evaluateBezierPatch(ctrPts, A)-evaluateBezierPatch(ctrPts, C)).norm()==0;
//        }
//    }
//    if (degenerate23) {
//        while (degenerate23) {
//            B = B - 0.1;
//            C = C + 0.1;
//            degenerate23 = (evaluateBezierPatch(ctrPts, C)-evaluateBezierPatch(ctrPts, B)).norm()==0;
//        }
//    }
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
//    if (depth > 50)
//        binary = 0b000;
    if (binary == 0b000) {
        v1norm = evaluateSurfaceNormal(ctrPts, A);
        v2norm = evaluateSurfaceNormal(ctrPts, B);
        v3norm = evaluateSurfaceNormal(ctrPts, C);
    }
    
    switch (binary) {
        case 0b001: //e1 not flat
            evaluateAdaptiveTriangle(e1pt, B, A, ctrPts, mesh, depth+1);
            evaluateAdaptiveTriangle(C, B, e1pt, ctrPts, mesh, depth+1);
            break;
        case 0b010: //e2 not flat
            evaluateAdaptiveTriangle(C, e2pt, A, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(C, B, e2pt, ctrPts, mesh, ++depth);
            break;
        case 0b100: //e3 not flat
            evaluateAdaptiveTriangle(A, e3pt, B, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(C, e3pt, A, ctrPts, mesh, ++depth);
            break;
        case 0b011: //e1 e2 not flat
            evaluateAdaptiveTriangle(C, B, e2pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(C, e2pt, e1pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e1pt, e2pt, A, ctrPts, mesh, ++depth);
            break;
        case 0b110: //e2 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, A, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(A, e3pt, e2pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e2pt, e3pt, B, ctrPts, mesh, ++depth);
            break;
        case 0b101: //e1 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, e1pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e1pt, e3pt, B, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e1pt, B, A, ctrPts, mesh, ++depth);
            break;
        case 0b111: //e1 e2 e3 not flat
            evaluateAdaptiveTriangle(C, e3pt, e1pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e3pt, B, e2pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e1pt, e3pt, e2pt, ctrPts, mesh, ++depth);
            evaluateAdaptiveTriangle(e1pt, e2pt, A, ctrPts, mesh, ++depth);
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
//    float newU=u, newV=v;
    for (int i = 0; i < 4; ++i) partialU[i] = evaluateTangent(controlPoints + 4 * i, u);
    for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, u);
    
    Vector normal = (evaluateBezierCurve(partialU, v).cross(evaluateTangent(uCurve, v))).normalized();
//    while (normal.norm() == 0 || isnan(normal.norm())) {
//        for (int i = 0; i < 4; ++i) partialU[i] = evaluateTangent(controlPoints + 4 * i, newU+=0.01);
//        for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, newU);
//        normal = (evaluateBezierCurve(partialU, newV+=0.01).cross(evaluateTangent(uCurve, newV))).normalized();
//    }
    return normal;
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