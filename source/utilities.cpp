#ifndef DVZUtil_H
#define DVZUtil_H
#include "preHeader.h"

#pragma mark - Data Structures

typedef Array3f Color;
typedef Array3f Point;
typedef Vector3f Vector;
typedef Array2f Sample;
typedef Translation<float, 3> Translation3f;
typedef UniformScaling<float> Scalingf;
typedef DiagonalMatrix<float, 3> Scaling3f;
#define Scaling3f(X, Y, Z) Scaling((float)X, (float)Y, (float)Z)
typedef Transform<float, 3, Affine> Transform3fAffine;
typedef Transform<float, 3, Projective> Transform3fProjective;
#define IdentityTransform() Transform<float, 3, Affine>(UniformScaling<float>(1.0))
typedef enum {
    LightSourceDirectional,
    LightSourcePoint
} LightSourceType;

typedef struct {
    string optName;
    int numOfArgs;
    vector<string> *args;
} CmdLineOptResult;
bool verbose = false;

typedef struct {
    Point min;
    Point max;
} BoundingBox;

#pragma mark - Math
float sqr(float x) { return x*x;}

Vector makeVec(Point start, Point end) {
    return (end-start).matrix();
}

#pragma mark - Syntax Sugar

float RCh(Color c) {return c(0);}
float GCh(Color c) {return c(1);}
float BCh(Color c) {return c(2);}
float X(Array3f c) {return c(0);}
float Y(Array3f c) {return c(1);}
float Z(Array3f c) {return c(2);}
float X(Vector v) {return v(0);}
float Y(Vector v) {return v(1);}
float Z(Vector v) {return v(2);}


#pragma mark - Utilities

float minf(float num1, float num2) {return min<float>(num1, num2);}
float maxf(float num1, float num2) {return max<float>(num1, num2);}

string fileNameWithoutExt(const string& fileName) {
    size_t position = fileName.find(".");
    return (string::npos == position)? fileName : fileName.substr(0, position);
}

/**
 *  Prints one string using std::cout
 *
 *  @param str string to be printed
 *  @discussion does not support multiple strings concatenation
 */
void println(string str){
    cout << str << endl;
}

/**
 *  Parses command line options specified in "options"
 *
 *  @param argc    argc from main
 *  @param argv    argc from main
 *  @param options a specially formatted string specifying what you want to parse
 *  @param results a pointer to a pointer to the vector of parsed results
 *  @discussion
 *      The format of options: "argX(n)argY(m)", where argX argY are the name of the options,
 *      n and m are the number of arguments for each option
 *      for example, if my program takes "./program -file one two --copy bunny -k rgb"
 *      then my options string would be "-file(2)--copy(1)-k(1)"
 *
 *      The results pointer will be set to a vector, containing CmdLineOptResult's, which is typedef'd above.
 *      To-Do: catch incorrectly formatted arguments and inconsistent number of arguments error instead of crashing
 *  @return none
 *  @example
 *      vector<CmdLineOptResults> *results;
 *      getCmdLineOptions(argc, argv, "-ka(2)-kz(1)", &results);
 *      for (auto & result : results) {
 *          cout << result.optName << "\n";
 *          cout << result.args->at(0) <<" "<< result.args-at(1);
 *      }
 */
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
