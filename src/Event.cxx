#include "AnalysisTools/Event.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    // Set method(s).
    void Event::addInfo (const string& name, const double& val) {
        if ( m_info.count(name) =! 0 ) {
	    ERROR("Info named '%s' already exists.", name.c_str());
	}
        m_info[name] = val;
        return;
    }
    
    void Event::addCollection (const string& name, PhysicsObjects* collection) {
        if ( m_collections.count(name) =! 0 ) {
	    ERROR("Collection named '%s' already exists.", name.c_str());
	}
	m_collections[name] = {};
	for (const PhysicsObject& p : *collection) {
	  m_collections[name].push_back(&p);
	}
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
    bool Event::hasCollection (const string& name) const {
      //INFO("Looking for '%s'", name.c_str());
        return m_collections.count(name) > 0;
    }

    const PhysicsObjectPtrs& Event::collection (const string& name) const {
        if ( m_collections.count(name) == 0 ) {
	    ERROR("No collection named '%s' exists.", name.c_str());
	}
        return m_collections.at(name);
    }
    
  PhysicsObjectPtrs& Event::mutableCollection (const string& name) {
        if ( m_collections.count(name) == 0 ) {
	    ERROR("No collection named '%s' exists.", name.c_str());
	}
        return m_collections.at(name);
    }
    
    double Event::info (const string& name) const {
        if (m_info.count(name) == 0) {
	    ERROR("No info named '%s' was found", name.c_str());
	}
        return m_info.at(name);
    }
    
    const PhysicsObject& Event::particle (const string& name) const {
        if ( m_particles.count(name) == 0 ) {
	    ERROR("No particle named '%s' exists.", name.c_str());
	}
        return m_particles.at(name);
    }
    
    GRL* Event::grl () const {
        assert( m_grl );
        return m_grl;
    }
    
    
    
    // High-level management method(s).
    // ...
    
}
