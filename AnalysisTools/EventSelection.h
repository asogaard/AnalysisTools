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
        {
            this->setName(name);
        };
        

        // Destructor(s).
        ~EventSelection () {};

        
    public:
        
        // Set method(s).
        void addCollection (const string& name, const PhysicsObjects& collection);
        
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        void run ();
        
        bool result ();
        bool result (const string& category);
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        

    private:
        
        map<string, Event>          m_events;
        map<string, PhysicsObjects> m_collections;
        map<string, bool>           m_passes;
        
        bool m_hasRun = false;
        int  m_branch = -1;
        
    };

}

#endif