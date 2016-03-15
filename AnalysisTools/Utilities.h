#ifndef AnalysisTools_Utilities_h
#define AnalysisTools_Utilities_h

/**
 * @file Utilities.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <fstream> /* ifstream */
#include <limits> /* std::numeric_limits<double>::infinity */

using namespace std;

namespace AnalysisTools {
    
    const double inf =    std::numeric_limits<double>::infinity();
    const double eps = 1./std::numeric_limits<double>::max();
    
    // Check whether file exists.
    inline bool fileExists (const string& filename) {
        ifstream f(filename.c_str());
        bool exists = f.good();
        f.close();
        return exists;
    }
    
}

#endif