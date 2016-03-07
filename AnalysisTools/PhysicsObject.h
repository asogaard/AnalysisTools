#ifndef AnalysisTools_PhysicsObject_h
#define AnalysisTools_PhysicsObject_h

/**
 * @file PhysicsObject.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h> /* assert */

// ROOT include(s).
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

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
    
}

#endif