//
//  UniformTessellation.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/5/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "UniformTessellation.h"

UniformTessellation::UniformTessellation() {}

void UniformTessellation::tessellate(vector<BezierObject>& bezierObjects, vector<Mesh>& meshes) {
    for (auto & object : bezierObjects) {
        //int biggestIndex = 0;
        size_t numberOfVertices = 0;
        size_t numberOfIndicies = 0;
        size_t numVerticesPerPatch = (divs+1)*(divs+1);
        size_t numIndiciesPerPatch = divs*divs*4;
        //total list of vertices
        Vector *vertices = new Vector[numVerticesPerPatch * object.numberOfPatches];
        //total list of indices
        int *indices = new int[numIndiciesPerPatch * object.numberOfPatches];
        
        //calculate fucking Bezier patches, getting all patchy patchy
        Vector *patchControlPoints;
        for (int currPatch=0; currPatch<object.numberOfPatches; ++currPatch) {
            patchControlPoints = object.controlPoints+currPatch*16;
            //evaluate the divs*divs vertices on the grid
            int k = currPatch*numVerticesPerPatch;
            for (int i=0; i<=divs; ++i) {
                for (int j=0; j<=divs; ++j) {
                    vertices[k++] = evaluateBezierPatch(patchControlPoints,i/(float)divs,j/(float)divs);
                    ++numberOfVertices;
                }
            }
            //connect faces by linking indices to vertices
            k = currPatch*numIndiciesPerPatch;
            for (int j=0; j<divs; ++j) {
                for (int i=0; i<divs; ++i) {
                    int p = currPatch*numVerticesPerPatch;
                    //visualization: j axis horizontal, i axis vertical
                    indices[k+0] = p + (divs+1)*(j+0) + (i+0); //left bottom
                    indices[k+1] = p + (divs+1)*(j+1) + (i+0); //right bottom
                    indices[k+2] = p + (divs+1)*(j+1) + (i+1); //right top
                    indices[k+3] = p + (divs+1)*(j+0) + (i+1); //left top
                    ++k;
                    numberOfIndicies+=4;
                    /*
                    if (indices[k+0] > biggestIndex) {
                        biggestIndex = indices[k+0];
                    }
                    if (indices[k+1] > biggestIndex) {
                        biggestIndex = indices[k+1];
                    }
                    if (indices[k+2] > biggestIndex) {
                        biggestIndex = indices[k+2];
                    }
                    if (indices[k+3] > biggestIndex) {
                        biggestIndex = indices[k+3];
                    }
                     */
                }
            }
        }
        Mesh mesh = {numberOfVertices, numberOfIndicies, vertices, indices};
        meshes.push_back(mesh);
        //printf("Biggest Index: %d\n", biggestIndex);
    }
}

Vector UniformTessellation::evaluateBezierCurve(const Vector *ctrPts, const float &t) {
    float b0 = (1 - t) * (1 - t) * (1 - t);
    float b1 = 3 * t * (1 - t) * (1 - t);
    float b2 = 3 * t * t * (1 - t);
    float b3 = t * t * t;
    return ctrPts[0] * b0 + ctrPts[1] * b1 + ctrPts[2] * b2 + ctrPts[3] * b3;
}

Vector UniformTessellation::evaluateBezierPatch(const Vector *controlPoints,
                                                const float &u,
                                                const float &v) {
    Vector uCurve[4];
    for (int i = 0; i < 4; ++i) uCurve[i] = evaluateBezierCurve(controlPoints + 4 * i, u);
    return evaluateBezierCurve(uCurve, v);
}