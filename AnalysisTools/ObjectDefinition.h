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
#include <memory> /* shared_ptr */

// ROOT include(s).
// ..

// AnalysisTools include(s).
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Selection.h"
#include "AnalysisTools/Cut.h"
#include "AnalysisTools/Info.h"

using namespace std;

namespace AnalysisTools {

    template<class T>
      class ObjectDefinition : public Selection<T, PhysicsObject>, public VectorInfo {

    public:

        // Constructor(s).
        ObjectDefinition (const string& name) :
            Selection<T,PhysicsObject>(name)
        {};        

        // Destructor(s).
	~ObjectDefinition () {};
        

    public:
        
        // Set method(s).
        void setInput  (const vector<T>* candidates);
        
        // High-level management method(s).
        bool run ();
        
        PhysicsObjects* const result ();
        PhysicsObjects* const result (const string& category);

	virtual void print () const;
     
        
    protected:
        
        // Low-level management method(s).
        // ...
        
        
    private:

        map<string, PhysicsObjects> m_candidates;
        
        bool m_hasRun = false;
        
        const vector<T>* m_input = nullptr; /* Universal; not category-specific. */
        
    };

}

#endif
