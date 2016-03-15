#include "AnalysisTools/PhysicsObject.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    // Set method(s).
    void PhysicsObject::addInfo (const string& name, const double& val) {
        assert( m_info.count(name) == 0 );
        m_info[name] = val;
        return;
    }
    
    
    // Get method(s).
    double PhysicsObject::info (const string& name) const {
        assert( m_info.count(name) > 0 );
        return m_info.at(name);
    }
    
    
    // High-level management method(s).
    // ...
 
    
     // PhysicsObject-specific utility functions.
    // -------------------------------------------------------------------
    
    // -- Perform overlap removal.
    bool OverlapRemoval(vector<PhysicsObject>* first,
                        vector<PhysicsObject>* second,
                        const double& R1,
                        const double& R2,
                        const function< bool(PhysicsObject, PhysicsObject) >& constraint) {
        /* Removing elements from 'first' if they overlap with either element in 'second' within 'R', possibly under an additional 'constaint' */
        for (unsigned int iFirst = first->size(); iFirst --> 0; ) {
            PhysicsObject& p1 = first->at(iFirst);
            for (unsigned int iSecond = 0; iSecond < second->size(); iSecond++) {
                PhysicsObject& p2 = second->at(iSecond);
                double dR = p1.DeltaR(p2);
                if (R1 < dR && dR < R2 && constraint(p1,p2)) {
                    first->erase(first->begin() + iFirst);
                    break;
                }
            }
        }
        return true;
    }
    
    bool OverlapRemoval(vector<PhysicsObject>* first,
                        vector<PhysicsObject>* second,
                        const double& R1,
                        const double& R2) {
        OverlapRemoval(first, second, R1, R2, [](PhysicsObject p1, PhysicsObject p2) -> bool { return true; });
        return true;
    }
    
    bool OverlapRemoval(vector<PhysicsObject>* first,
                        vector<PhysicsObject>* second,
                        const double& R) {
        OverlapRemoval(first, second, 0., R, [](PhysicsObject p1, PhysicsObject p2) -> bool { return true; });
        return true;
    }
    
    
    bool OverlapRemoval(vector<PhysicsObject>* first,
                        vector<PhysicsObject>* second,
                        const double& R,
                        const function< bool(PhysicsObject,PhysicsObject) >& constraint) {
        OverlapRemoval(first, second, 0., R, constraint);
        return true;
    }

}