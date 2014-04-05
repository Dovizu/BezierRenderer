#ifndef DVZUtil_H
#define DVZUtil_H
#include "utilities.h"


#pragma mark - Data Structures


bool verbose = false;

#pragma mark - Math
float sqr(float x) { return x*x;}

Vector makeVec(Point start, Point end) {
    return (end-start).matrix();
}


#pragma mark - Utilities

float minf(float num1, float num2) {return min<float>(num1, num2);}
float maxf(float num1, float num2) {return max<float>(num1, num2);}

string fileNameWithoutExt(const string& fileName) {
    size_t position = fileName.find(".");
    return (string::npos == position)? fileName : fileName.substr(0, position);
}

void println(string str){
    cout << str << endl;
}

void getCmdLineOptions(int argc, char *argv[], string options, vector<CmdLineOptResult> **results)
{
    vector<CmdLineOptResult> *resultsArray = new vector<CmdLineOptResult>();
    for (int i=1; i < argc; ++i)
    {
        string arg = string(argv[i]);
        size_t index = options.find(arg);
        if (index!=string::npos)
        {
            char c = ' ';
            while (c!='\0' && (c=options[index++])!='('){};
            string num = string();
            while (c!='\0' && (c=options[index])!=')') {
                num.append(1, options[index]);
                ++index;
            }
            if (c=='\0') {
                throw "Options string is incorrectly formatted";
            }
            int numOfArgs = atoi(num.c_str());
            //build result for this option
            vector<string> *arguments = new vector<string>(numOfArgs);
            CmdLineOptResult result = {arg, numOfArgs, arguments};
            for (int arg_index=0; arg_index<numOfArgs; ++arg_index) {
                (*arguments)[arg_index] = argv[(++i)];
            }
            resultsArray->push_back(result);
        }
    }
    *results = resultsArray;
}

#pragma mark - Conversion
float floatFromString(string str)
{
    return (float)atof(str.c_str());
}
int intFromString(string str)
{
    return (int)atoi(str.c_str());
}

#endif
