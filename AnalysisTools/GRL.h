#ifndef AnalysisTools_GRL_h
#define AnalysisTools_GRL_h

/**
 * @file GRL.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <vector>
#include <map>
#include <iostream> /* std::cout */
#include <fstream> /* std::ifstream */
#include <assert.h> /* assert */

// ROOT include(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {

    class GRL {
        
    public:

        // Constructor(s).
        GRL () {};
        GRL (const string& path) {
            loadXML(path);
        };
        
        // Destructor(s).
        ~GRL () {};
        
        
    public:
        
        // Set method(s).
        void loadXML (const string& path);

        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        bool contains (const int& run, const int& LB) const;
        
        
    private:
    
        bool m_hasXML = false;
        map< int, Ranges > m_goodRuns;
        
    };

}

#endif