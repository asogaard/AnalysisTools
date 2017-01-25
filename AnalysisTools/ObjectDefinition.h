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
        void setInput  (const vector<T>* candidates);
        
        template<class W>
	inline void addInfo (const string& name, const vector<W>* info) {
	  // Fully tempalted functions need to be visible to compiler, hence implementation in header.
	  m_info.add<W>(name, info);
	  return;
	}
        
        
        // Get method(s).
        template <class W>
	inline const vector<W>* info (const string& name) {
	  // Fully tempalted functions need to be visible to compiler, hence implementation in header.
	  return m_info.get<W>(name);
	}

        
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
        
	VectorInfo m_info;
        
    };

}

#endif
