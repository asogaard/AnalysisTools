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
    
    void Event::addCollection (const string& name, shared_ptr<PhysicsObjects> collection) {
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
        cout << "<Event::setParticle> Setting particle '" << name << "'." << endl;
        m_particles[name] = particle;
        return;
    }
    
    
    // Get method(s).
    double Event::info (const string& name) {
        assert( m_info.count(name) > 0 );
        return m_info[name];
    }
    
    shared_ptr<PhysicsObjects> Event::collection (const string& name) {
        assert( m_collections.count(name) > 0 );
        return m_collections[name];
    }
    
    PhysicsObject& Event::particle (const string& name) {
        cout << "<Event::particle> Trying to access particle '" << name << "'." << endl;
        assert( m_particles.count(name) > 0 );
        cout << "<Event::particle> Got it!" << endl;
        return m_particles[name];
    }
    
    GRL* Event::grl () {
        assert( m_grl );
        return m_grl;
    }
    
    
    
    // High-level management method(s).
    // ...
    
}