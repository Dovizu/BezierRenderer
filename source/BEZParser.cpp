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

void BEZParser::parseDirectory(string dir,
                    size_t *numberOfPatches,
                    Vector* *controlPoints)  //ptr to array
{
    ASSERT(dir.back()=='/', "Directory name incorrect, expected: dirname/");
    const int MAX_CHARS_PER_LINE = 100;
    const int MAX_TOKENS_PER_LINE = 100;
    const char* const DELIMITER = " ";
    
    vector<string> filenames;
    paramMap pMap;
    getFileNamesOfDirectory(dir, filenames, pMap);
    
    for (auto& filename : filenames) {
        size_t patchNum = 0;
        size_t currPatch = 0;
        size_t currGroup = 0;
        ifstream fin;
        fin.open(filename); // open a file
        while (!fin.eof()) {
            // read an entire line into memory
            char buf[MAX_CHARS_PER_LINE];
            fin.getline(buf, MAX_CHARS_PER_LINE);
            // parse the line into blank-delimited tokens
            int n = 0; // a for-loop index
            // array to store memory addresses of the tokens in buf
            const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
            // parse the line
            token[0] = strtok(buf, DELIMITER); // first token
            if (token[0]) { // zero if line is blank
                for (n = 1; n < MAX_TOKENS_PER_LINE; n++) {
                    token[n] = strtok(NULL, DELIMITER); // subsequent tokens
                    if (!token[n]) break; // no more tokens
                }
            }
            if (token[0]) {
                //First line tells you number of patches
                if (!patchNum) {
                    ASSERT(token[1]==NULL, "Incorrectly formatted bez file, first line is not integer: number of patches");
                    string firstToken = stripNewLine(token[0]);
                    patchNum = intFromString(firstToken);
                    *numberOfPatches = patchNum;
                    *controlPoints = new Vector[patchNum*16];
                    continue; //this line should only contain one number
                }
                
                if (isNewLine(token[0])) {
                    ++currPatch;
                    currGroup = 0;
                    continue;
                }
                
                if (currPatch < patchNum) {
                    ASSERT(token[0]!=NULL && token[11]!=NULL,
                           "Incorrectly formatted bez file, each line should contain 12 numbers, 3 dims for each of the 4 control points");
                    //Obtain 4 control points in one parametric direction
                    float x1 = floatFromString(string(token[0]));
                    float y1 = floatFromString(string(token[1]));
                    float z1 = floatFromString(string(token[2]));
                    float x2 = floatFromString(string(token[3]));
                    float y2 = floatFromString(string(token[4]));
                    float z2 = floatFromString(string(token[5]));
                    float x3 = floatFromString(string(token[6]));
                    float y3 = floatFromString(string(token[7]));
                    float z3 = floatFromString(string(token[8]));
                    float x4 = floatFromString(string(token[9]));
                    float y4 = floatFromString(string(token[10]));
                    float z4 = floatFromString(stripNewLine(string(token[11])));
                    
                    (*controlPoints)[currPatch*16+currGroup*4+0] = Vector(x1, y1, z1);
                    (*controlPoints)[currPatch*16+currGroup*4+1] = Vector(x2, y2, z2);
                    (*controlPoints)[currPatch*16+currGroup*4+2] = Vector(x3, y3, z3);
                    (*controlPoints)[currPatch*16+currGroup*4+3] = Vector(x4, y4, z4);
                    
                    currGroup++; //next four
                }
                
            }
        }
    }
}