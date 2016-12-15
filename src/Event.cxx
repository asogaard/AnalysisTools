#include "AnalysisTools/Event.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    // Set method(s).
    void Event::addInfo (const string& name, const double& val) {
      //WARNING("Adding info '%s'", name.c_str());
        assert( m_info.count(name) == 0 );
        m_info[name] = val;
        return;
    }
    
    void Event::addCollection (const string& name, PhysicsObjects* collection) {
      //INFO("Adding collection '%s'", name.c_str());
        assert( m_collections.count(name) == 0 );
        m_collections[name] = collection;
        return;
    }
    
    void Event::addGRL (GRL* grl) {
        assert( grl );
        m_grl = grl;
        return;
    }
    
    void Event::setParticle (const string& name, const PhysicsObject& particle) {
        m_particles[name] = particle;
        return;
    }
    
    
    // Get method(s).
    double Event::info (const string& name) const {
      //INFO("Looking for '%s'", name.c_str());
        assert( m_info.count(name) > 0 );
        return m_info.at(name);
    }
    
    PhysicsObjects* Event::collection (const string& name) const {
      //INFO("Trying to access collection '%s'", name.c_str());
        assert( m_collections.count(name) > 0 );
        return m_collections.at(name);
    }
    
    const PhysicsObject& Event::particle (const string& name) const {
        assert( m_particles.count(name) > 0 );
        return m_particles.at(name);
    }
    
    GRL* Event::grl () const {
        assert( m_grl );
        return m_grl;
    }
    
    
    
    // High-level management method(s).
    // ...
    
}
