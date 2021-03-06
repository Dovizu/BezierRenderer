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
        size_t numNormalsPerPatch = (divs+1)*(divs+1);
        size_t numElementsPerPatch = numVerticesPerPatch + numNormalsPerPatch;
        
        size_t numIndiciesPerPatch = divs*divs*6;
        //total list of vertices
        Vector *vertices = new Vector[numElementsPerPatch * object.numberOfPatches];
        //total list of indices
        int *indices = new int[numIndiciesPerPatch * object.numberOfPatches];
        
        //calculate Bezier patches
        Vector *patchControlPoints;
        for (int currPatch=0; currPatch<object.numberOfPatches; ++currPatch) {
            patchControlPoints = object.controlPoints+currPatch*16;
            //evaluate the divs*divs vertices on the grid
            int k = currPatch*numElementsPerPatch;
            for (int i=0; i<=divs; ++i) {
                for (int j=0; j<=divs; ++j) {
                    //evaluate vertex
                    vertices[k++] = evaluateBezierPatch(patchControlPoints,i/(float)divs,j/(float)divs);
                    ++numberOfVertices;
                    //evaluate normal
                    vertices[k++] = evaluateSurfaceNormal(patchControlPoints,i/(float)divs,j/(float)divs);
                    ++numberOfVertices;
                }
            }
            //connect faces by linking indices to vertices
            k = currPatch*numIndiciesPerPatch;
            int p = currPatch*numVerticesPerPatch;
            for (int j=0; j<divs; ++j) {
                for (int i=0; i<divs; ++i) {
                    //visualization: j axis horizontal, i axis vertical
                    //triangle 1
                    indices[k+0] = p + (divs+1)*(j+0) + (i+0); //left bottom
                    indices[k+1] = p + (divs+1)*(j+1) + (i+0); //right bottom
                    indices[k+2] = p + (divs+1)*(j+1) + (i+1); //right top
                
                    //triangle 2
                    indices[k+3] = p + (divs+1)*(j+0) + (i+0); //left bottom
                    indices[k+4] = p + (divs+1)*(j+1) + (i+1); //right top
                    indices[k+5] = p + (divs+1)*(j+0) + (i+1); //left top
                    k += 6;
                    numberOfIndicies+=6;
                    /* for debugging
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
        Mesh mesh = {UniformMesh, numberOfVertices, numberOfIndicies, vertices, indices, NULL};
        meshes.push_back(mesh);
        //printf("Biggest Index: %d\n", biggestIndex);
    }
}