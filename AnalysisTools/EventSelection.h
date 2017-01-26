#ifndef AnalysisTools_EventSelection_h
#define AnalysisTools_EventSelection_h

/**
 * @file EventSelection.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <algorithm> /* std::count_if */
#include <typeinfo> /* std::bad_cast */
#include <utility> /* std::make_pair */

// ROOT include(s).
// ..

// AnalysisTools include(s).
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Selection.h"
#include "AnalysisTools/Cut.h"
#include "AnalysisTools/Info.h"

using namespace std;

namespace AnalysisTools {

  class EventSelection : public Selection<Event, Event>, public BasicInfo {

    public:

        // Constructor(s).
        EventSelection (const string& name) :
            Selection<Event,Event>(name)
        {};

        // Destructor(s).
	~EventSelection () {};

        
    public:
        
        // Set method(s).
        /*
	  void addCollection (const string& name, PhysicsObjects* collection);
	*/
	void addCollection (const string& name, const string& selection, const string& category = "Nominal");

        //void addCollection (const string& name, vector<TLorentzVector>* collection);// @TODO: Add such a method?
        
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        bool run ();
        
        bool result ();
        bool result (const string& category);

	virtual void print () const;
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        

    private:
        
        map< string, Event >           m_events;
        /*
	  map< string, PhysicsObjects* > m_collections;
	*/
	map< string, std::pair<string, string> > m_collections;
        map< string, bool >            m_passes;

    };

}

#endif
