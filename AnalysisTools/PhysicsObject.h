#ifndef AnalysisTools_PhysicsObject_h
#define AnalysisTools_PhysicsObject_h

/**
 * @file PhysicsObject.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h> /* assert */
#include <memory> /* shared_ptr */

// ROOT include(s).
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

/*
 * Make templated? Derive from same base class as Event? (Both have 'info' member and related methods.)
 */

using namespace std;

namespace AnalysisTools {

    class PhysicsObject : public TLorentzVector {
        
    public:

        // Constructor(s).
        PhysicsObject () {};
        PhysicsObject (const TLorentzVector& other) :
            TLorentzVector(other)
        {};
        
        // Destructor(s).
        ~PhysicsObject () {};
        
        
    public:
        
        // Set method(s).
        void addInfo (const string& name, const double& val);

        
        // Get method(s).
        double info (const string& name);
        
        
        // High-level management method(s).
        // ...
        
        
    private:
        
        map<string, double> m_info;
        
    };

    using PhysicsObjects = vector<PhysicsObject>;
    
    // PhysicsObject-specific utility functions.
    // -------------------------------------------------------------------
    
    // -- Perform overlap removal.
    bool OverlapRemoval(shared_ptr< vector<PhysicsObject> > first, shared_ptr< vector<PhysicsObject> > second, const double& R1, const double& R2, function< bool(PhysicsObject,PhysicsObject) > constraint);
    bool OverlapRemoval(shared_ptr< vector<PhysicsObject> > first, shared_ptr< vector<PhysicsObject> > second, const double& R1, const double& R2);
    bool OverlapRemoval(shared_ptr< vector<PhysicsObject> > first, shared_ptr< vector<PhysicsObject> > second, const double& R);
    bool OverlapRemoval(shared_ptr< vector<PhysicsObject> > first, shared_ptr< vector<PhysicsObject> > second, const double& R, function< bool(PhysicsObject,PhysicsObject) > constraint);

}

#endif