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
#include <sstream> /* std::sstream */
#include <sys/stat.h> /* struct stat */

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
    
    // Check whether directory exists.
    inline bool dirExists (const string& dir) {
        struct stat statbuf;
        bool exists = false;
        if (stat(dir.c_str(), &statbuf) != -1) {
            if (S_ISDIR(statbuf.st_mode)) { exists = true; }
        }
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
    
    // Check whether a string ends with another string.
    inline bool endsWith (string const &full, string const &ending) {
        if (full.length() >= ending.length()) {
            return (0 == full.compare (full.length() - ending.length(), ending.length(), ending));
        }
        return false;
    }
    
    // Split a string by delimeter.
    inline vector<string>& split (const string& s, char delim, vector<string>& elems) {
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            if (item == "") { continue; }
            elems.push_back(item);
        }
        return elems;
    }
    
    inline vector<string> split (const string& s, char delim) {
        vector<string> elems;
        split(s, delim, elems);
        return elems;
    }
    
}

#endif