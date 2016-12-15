#ifndef AnalysisTools_Utilities_h
#define AnalysisTools_Utilities_h

/**
 * @file Utilities.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <fstream>
#include <limits> /* std::numeric_limits<double>::infinity */
#include <algorithm> /* std::find, std::replace */
#include <iterator> /* std::distance */
#include <sstream> /* std::sstream */
#include <memory> /* std::unique_ptr */
#include <utility> /* std::move */
#include <sys/stat.h> /* struct stat */

using namespace std;

namespace AnalysisTools {
    
    const double inf =    std::numeric_limits<double>::infinity();
    const double eps = 1./std::numeric_limits<double>::max();
    const double pi  = 3.14159265359;
    
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

    // Trim string from start.
    static inline std::string& ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(),
				      std::not1(std::ptr_fun<int, int>(std::isspace))));
      return s;
    }

    // Trim string from end.
    static inline std::string& rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(),
			   std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
    }

    // Trim string from both ends.
    static inline std::string& trim(std::string &s) {
      return ltrim(rtrim(s));
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

    // Replace all instanace of 'from'-string with 'to'-string.
    inline string replaceAll(string str, const string& from, const string& to) {
      unsigned start_pos = 0;
      while ((start_pos = str.find(from, start_pos)) < str.length()) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
      }
      return str;
    }

    // Format a floating point number to a string.
    inline string formatNumber (const double&   f,			\
				const unsigned& precision = 2,		\
				const unsigned& leadingPlaces = 8) {
        const unsigned l = 100;
	char buffer [l];
	int cx = snprintf ( buffer, l, "%*.*f",				\
			    leadingPlaces,				\
			    (f == 0 ? 0 : max(int(precision) - 1 - int(log10(f/2. + eps)), 0)), \
			    f );
	string s = buffer;
	return s;
    }

    // Utility function for creating and moving unique pointer.
    template<typename T>
    inline  std::unique_ptr<T> makeUniqueMove (T* p) {
        return std::move(std::unique_ptr<T>(p));
    }
    
}

#endif
