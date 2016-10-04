#ifndef AnalysisTools_EventSelection_h
#define AnalysisTools_EventSelection_h

/**
 * @file EventSelection.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm> /* std::count_if */

// ROOT include(s).
// ..

// AnalysisTools include(s).
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Selection.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {

    class EventSelection : public Selection<Event, Event> {

    public:

        // Constructor(s).
        EventSelection (const string& name) :
            Selection<Event,Event>(name)
        {};
        

        // Destructor(s).
        ~EventSelection () {};

        
    public:
        
        // Set method(s).
        void addCollection (const string& name, PhysicsObjects* collection);
        
        template <class U>
        void addInfo (const string& name, const U& info);
  
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        bool run ();
        
        bool result ();
        bool result (const string& category);
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        

    private:
        
        map< string, Event >           m_events;
        map< string, PhysicsObjects* > m_collections;
        map< string, bool >            m_passes;

        map< string, double > m_infoDouble;
        map< string, float >  m_infoFloat;
        map< string, int >    m_infoInt;
        map< string, bool >   m_infoBool;

        
    };

}

#endif
