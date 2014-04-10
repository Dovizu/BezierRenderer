//
//  renderBezier.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/3/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "renderBezier.h"
#include "UnitTest.cpp"
#include "BEZParser.h"
#include "Tessellation.h"
#include "UniformTessellation.h"
#include "AdaptiveTessellation.h"

int main(int argc, char *argv[]) {
    vector<CmdLineOptResult> *results;
    string basePath;
    string options = "--testOpenGL(0)"; //test OpenGL, GLEW, and GLFW
    options.append("--testBEZParser(0)"); //test BEZParser for files at "bezFiles/"
    options.append("--testUniform(0)"); //test UniformTessellation
    
    options.append("-f(2)"); //render file, param
    options.append("-d(2)"); //render directory of files, param
    options.append("-a(0)"); //use adaptive tessellation
    
    getCmdLineOptions(argc, argv, options, &results);
    
    float param = 0.0;
    string directoryName;
    string fileName;
    bool adaptive = false;
    
    for (auto & result : *results) {
        if (result.optName.compare("--testOpenGL")==0) {
            testOpenGL();
        }
        if (result.optName.compare("--testBEZParser")==0) {
            testBEZParser();
        }
        if (result.optName.compare("--testUniform")==0) {
            testUniform();
        }
        if (result.optName.compare("-f")==0) {
            fileName = result.args->at(0);
            param = floatFromString(result.args->at(1));
        }
        if (result.optName.compare("-d")==0) {
            directoryName = result.args->at(0);
            param = floatFromString(result.args->at(1));
        }
        if (result.optName.compare("-a")==0) {
            adaptive = true;
        }
    }
    
    //Render
    if ((!directoryName.empty() || !fileName.empty()) && param > 0.0) {
        BEZParser *parser = new BEZParser();
        Tessellation *tessellator;
        vector<BezierObject> objects;
        vector<Mesh> meshes;
        
        if (adaptive) {
            tessellator = new AdaptiveTessellation();
            tessellator->setErrorRate(param);
        } else {
            tessellator = new UniformTessellation();
            tessellator->setDivDepth((int)(1/param));
        }
        if (!directoryName.empty()) {
            parser->parseDirectory(directoryName, objects);
        }
        if (!fileName.empty()) {
            parser->parseFile(fileName, objects);
        }
        tessellator->tessellate(objects, meshes);
        //render mesh openGL
    }
    
    return 0;
}