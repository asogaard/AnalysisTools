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
    double PhysicsObject::info (const string& name) {
        assert( m_info.count(name) > 0 );
        return m_info[name];
    }
    
    
    // High-level management method(s).
    // ...
    
}