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
        
        //template<class W>
        void addInfo (const string& name, const vector<double>* info);
        void addInfo (const string& name, const vector<float> * info);
        void addInfo (const string& name, const vector<int>   * info);
        void addInfo (const string& name, const vector<bool>  * info);
        /* @TODO: Do proper templating? */
        
        
        // Get method(s).
        template <class W>
        const vector<W>* info (const string& name);

        
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
        
        map<string, const vector<double>* > m_infoDouble;
        map<string, const vector<float>* >  m_infoFloat;
        map<string, const vector<int>* >    m_infoInt;
        map<string, const vector<bool>* >   m_infoBool;
        
        
    };

}

#endif
