#ifndef Assignment2_preHeader_h
#define Assignment2_preHeader_h

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include "float.h"

#include <Eigen> //Eigen needs to go before CImg because X11 re-defines "Success" macro
#include <dirent.h>

using namespace std;
using namespace Eigen;

#include "utilities.cpp"

#ifndef DNDEBUG
#   define ASSERT(condition, message) \
do { \
if (! (condition)) { \
cerr << "Assertion `" #condition "` failed in " << __FILE__ \
<< " line " << __LINE__ << ": " << message << std::endl; \
exit(EXIT_FAILURE); \
} \
} while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


bool flag_testing = false;
#define nl << endl

#endif

