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
//#include <array> /* std::array */
#include <tuple> /* std::tie */
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
	/**
	 * Add a physics collection to the given event selection category.
	 *
	 * The collections is added by name, rather than by pointer, since this allows for easier copying when the event selection is added to multiple analyses.
	 * 
	 * \param name The name of the physics object collection.
	 * \param objdef Name of the ObjectDefinition from which to acquire the collection.
	 * \param objdefCategory The ObjectDefinition category from which to acquire the collection. If left empty, will try to access 'Nominal'.
	 * \param category The category for which to add the collection. If left empty, added to all categories.
	 */
	//void addCollection (const string& name, const string& selection, const string& category = "");
	void addCollection (const string& name, const string& objdef, const string& objdefCategory = "Nominal", const string& category = "");

	void setInput (const Event* event);
	
                
        // Get method(s).
        // ...
	virtual inline bool passes (const std::string& category) const {
	  assert( this->hasCategory(category) );
	  return m_passes.at(category);
	}
        
        
        // High-level management method(s).
        virtual bool run ();
        
        bool result ();
        bool result (const string& category);

	virtual void print () const;
        
        
  private:
        
        // Internal methods(s)
	void cacheCollections_ ();
        

  private:
        
	const Event* m_input = nullptr;
        map< string, Event > m_events;
        map< string, bool >  m_passes;
	map< string, std::vector<std::tuple<string, string, string> > > m_collectionNames;
	map< string, map< string, PhysicsObjects* > >     m_collectionLinks;

	// Structure of m_collectionNamses:
	// vector[ (name-of-collection, name-of-objdef-from-which-to-get-collection, objdef-category) ]

	bool m_hasCachedCollections = false;

    };

}

#endif
