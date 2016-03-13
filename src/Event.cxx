#include "AnalysisTools/Event.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    // Set method(s).
    void Event::addInfo (const string& name, const double& val) {
        assert( m_info.count(name) == 0 );
        m_info[name] = val;
        return;
    }
    
    void Event::addCollection (const string& name, const PhysicsObjects& collection) {
        assert( m_collections.count(name) == 0 );
        m_collections[name] = collection;
        return;
    }
    
    void Event::addGRL (GRL* grl) {
        assert( grl );
        m_grl = grl;
        return;
    }
    
    
    // Get method(s).
    double Event::info (const string& name) {
        assert( m_info.count(name) > 0 );
        return m_info[name];
    }
    
    PhysicsObjects Event::collection (const string& name) {
        assert( m_collections.count(name) > 0 );
        return m_collections[name];
    }
    
    GRL* Event::grl () {
        assert( m_grl );
        return m_grl;
    }
    
    
    
    // High-level management method(s).
    // ...
    
}