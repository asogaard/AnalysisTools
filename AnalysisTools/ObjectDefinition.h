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
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Selection.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {

    template<class T>
    class ObjectDefinition : public Selection<T, PhysicsObject> {

    public:

        // Constructor(s).
        ObjectDefinition (const string& name) :
            Selection<T,PhysicsObject>(name)
        {};
        

        // Destructor(s).
        ~ObjectDefinition () {};

        
    public:
        
        // Set method(s).
        // ...
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        
        void run ();
        
        PhysicsObjects* result ();
        PhysicsObjects* result (const string& category);
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        
    private:
        

        map<string, PhysicsObjects> m_candidates;
        
        bool m_hasRun = false;
        int  m_branch = -1;
        
    };

}

#endif