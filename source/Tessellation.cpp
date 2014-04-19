//
//  Tessellation.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/5/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "Tessellation.h"

void Tessellation::setDivDepth(int d) {divs = d;}
void Tessellation::setErrorRate(float e) {error = e;}

Vector Tessellation::evaluateTangent(const Vector *ctrPts, const float &t) {
    float b0 = -3 * (1 - t) * (1 - t);
    float b1 = 3 * (1 - t) * (1 - t) - 6 * t * (1 - t);
    float b2 = 6 * t * (1 - t) - 3 * t * t;
    float b3 = 3 * t * t;
    return ctrPts[0] * b0 + ctrPts[1] * b1 + ctrPts[2] * b2 + ctrPts[3] * b3;
}

Vector Tessellation::evaluateSurfaceNormal(const Vector *controlPoints,
                                                  const float &u,
                                                  const float &v)
{
    Vector partialU[4];
    Vector uCurve[4];
    Vector normal;
    float newU = u, newV = v;
    do {
        for (int i = 0; i < 4; ++i) partialU[i] = evaluateTangent(controlPoints + 4 * i, newU);
        for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, newU);
        newU+=0.01; newV+=0.01;
        normal = (evaluateBezierCurve(partialU, newV).cross(evaluateTangent(uCurve, newV))).normalized();
    } while (normal.norm() <= 0.1);
    return normal.normalized();
}

Vector Tessellation::evaluateSurfaceNormal(const Vector *controlPoints,
                                                   const ParametricPoint& UV) {
    return evaluateSurfaceNormal(controlPoints, UV(0), UV(1));
}


Vector Tessellation::evaluateBezierCurve(const Vector *ctrPts, const float &t) {
    float b0 = (1 - t) * (1 - t) * (1 - t);
    float b1 = 3 * t * (1 - t) * (1 - t);
    float b2 = 3 * t * t * (1 - t);
    float b3 = t * t * t;
    return ctrPts[0] * b0 + ctrPts[1] * b1 + ctrPts[2] * b2 + ctrPts[3] * b3;
}

Vector Tessellation::evaluateBezierPatch(const Vector *controlPoints,
                                                 const float &u,
                                                 const float &v) {
    Vector uCurve[4];
    for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, u);
    return evaluateBezierCurve(uCurve, v);
}

Vector Tessellation::evaluateBezierPatch(const Vector *controlPoints,
                                                 const ParametricPoint& UV) {
    return evaluateBezierPatch(controlPoints, UV(0), UV(1));
}