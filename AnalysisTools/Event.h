#ifndef AnalysisTools_Event_h
#define AnalysisTools_Event_h

/**
 * @file Event.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <cassert> /* assert */
#include <memory> /* shared_ptr */

// ROOT include(s).
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/GRL.h"
#include "AnalysisTools/Logger.h"

using namespace std;

namespace AnalysisTools {

    class Event : public Logger {
        
    public:

        // Constructor(s).
        Event () {};
        
        // Destructor(s).
        ~Event () {};
        
        
    public:
        
        // Set method(s).
        void addInfo       (const string& name, const double&   val);
        void addCollection (const string& name, PhysicsObjects* collection);
        void addGRL        (GRL* grl);
        void setParticle   (const string& name, const PhysicsObject& particle);
        
        // Get method(s).
        bool  hasCollection (const string& name) const;

	const PhysicsObjectPtrs&        collection (const string& name) const;
      	      PhysicsObjectPtrs& mutableCollection (const string& name);

        double               info     (const string& name) const;
        const PhysicsObject& particle (const string& name) const;
        GRL*                 grl      ()                   const;
        
        
        // High-level management method(s).
        // ...
        
        
    private:
        
        map<string, double> m_info;
	map<string, PhysicsObjectPtrs > m_collections;
        map<string, PhysicsObject> m_particles;
        GRL* m_grl = nullptr;
        
    };

    using Events = vector<Event>;
    
}

#endif
