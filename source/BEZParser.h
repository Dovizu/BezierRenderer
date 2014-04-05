//
//  BEZParser.h
//  BezierSurfaceRenderer
//
//  Created by Donny Reynolds on 4/4/14.
//  Copyright (c) 2014 Leo C & Donny R. All rights reserved.
//

#ifndef __BezierSurfaceRenderer__BEZParser__
#define __BezierSurfaceRenderer__BEZParser__

#include "utilities.h"

typedef map<string, string> paramMap;
typedef struct {
    size_t numberOfPatches;
    Vector *controlPoints;
} BezierObject;

class BEZParser {
    
public:
    BEZParser();
    void parseDirectory(string dir, vector<BezierObject>& objects);
    
private:
    void getFileNamesOfDirectory(const string& basePath, vector<string>& filenames, paramMap& pMap);
    
};

#endif /* defined(__BezierSurfaceRenderer__BEZParser__) */
