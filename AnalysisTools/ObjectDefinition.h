#ifndef AnalysisTools_ObjectDefinition_h
#define AnalysisTools_ObjectDefinition_h

/**
 * @file ObjectDefinition.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h>

// ROOT include(s).
// ..

// AnalysisTools include(s).
#include "AnalysisTools/Selection.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {

    template<class T>
    class ObjectDefinition : public Selection<T> {

    public:

        // Constructor(s).
        ObjectDefinition (const string& name) :
            Selection<T>(name)
        {
            this->setName(name);
        };
        

        // Destructor(s).
        ~ObjectDefinition () {};

        
    public:
        
        // Set method(s).
        // ...
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        // ...
        
        void run ();
        
        vector< T* > result ();
        vector< T* > result (const string& category);
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        
    private:
        
        map< string, vector< T* > > m_candidates;
        
        bool m_hasRun = false;
        int  m_branch = -1;
        
    };

}

#endif