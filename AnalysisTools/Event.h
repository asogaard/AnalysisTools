#ifndef AnalysisTools_Event_h
#define AnalysisTools_Event_h

/**
 * @file Event.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h> /* assert */
#include <memory> /* shared_ptr */

// ROOT include(s).
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/GRL.h"

using namespace std;

namespace AnalysisTools {

    class Event : public TLorentzVector {
        
    public:

        // Constructor(s).
        Event () {};
        
        // Destructor(s).
        ~Event () {};
        
        
    public:
        
        // Set method(s).
        void addInfo       (const string& name, const double&              val);
        void addCollection (const string& name, shared_ptr<PhysicsObjects> collection);
        void addGRL        (GRL* grl);

        
        // Get method(s).
        double                     info       (const string& name);
        shared_ptr<PhysicsObjects> collection (const string& name);
        GRL*                       grl        ();
        
        
        // High-level management method(s).
        // ...
        
        
    private:
        
        map<string, double> m_info;
        map<string, shared_ptr<PhysicsObjects> > m_collections;
        GRL* m_grl = nullptr;
        
    };

    using Events = vector<Event>;
    
}

#endif