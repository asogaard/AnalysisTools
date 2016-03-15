#ifndef AnalysisTools_Utilities_h
#define AnalysisTools_Utilities_h

/**
 * @file Utilities.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <fstream> /* ifstream */
#include <limits> /* std::numeric_limits<double>::infinity */
#include <algorithm> /* std::find */
#include <iterator> /* std::distance */

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
    
    // Match element to other in vector, and return index of other.
    template <class T>
    inline int getMatchIndex (const T& p, vector<T>* vec) {
        typename vector<T>::iterator it = std::find(vec->begin(), vec->end(), p);
        int idx = std::distance(vec->begin(), it);
        if (idx == vec->size()) { idx = -1; }
        return idx;
    }
}

#endif