//
//  BEZParser.cpp
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/4/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#include "BEZParser.h"

BEZParser::BEZParser ()
{
}

void BEZParser::setScheme(TessellationScheme s) {
    scheme = s;
}
void BEZParser::setErrorRate(float e) {
    error = e;
}
void BEZParser::setDivDepth(uint32_t d) {
    divs = d;
}

void BEZParser::getFileNamesOfDirectory
(const string& basePath,
 vector<string>& filenames,
 paramMap& pMap)
{
    dirent *de;
    DIR *dirp = opendir(basePath.c_str());
    if (dirp) {
        while (true) {
            errno = 0;
            de = readdir(dirp);
            if (de==NULL) break;
            string name = string(de->d_name);
            if (name.find(".bez") != string::npos) {
                pMap[fileNameWithoutExt(basePath+name)] = "";
                filenames.push_back(basePath+name);
            }
            if (name.find(".param") != string::npos) {
                pMap[fileNameWithoutExt(basePath+name)] = basePath+name;
            }
        }
        closedir(dirp);
    }
}

void BEZParser::parseDirectory
(string dir,
 float **indices,
 uint32_t **vertices)
{
    
}