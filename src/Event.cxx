#include "AnalysisTools/Event.h"

namespace AnalysisTools {

    // Constructor(s).
    // ...
    
    // Set method(s).
    void Event::addInfo (const string& name, const float& val) {
        if ( m_info.count(name) != 0 ) {
	    ERROR("Info named '%s' already exists.", name.c_str());
	}
        m_info[name] = val;
        return;
    }
    
    void Event::addCollection (const string& name, PhysicsObjects* collection) {
        if ( m_collections.count(name) != 0 ) {
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
    
    float Event::info (const string& name) const {
        assert(hasInfo(name));
        return m_info.at(name);
    }
    
    bool Event::hasInfo (const string& name) const {
        return m_info.count(name) > 0;
    }
    
    const PhysicsObject& Event::particle (const string& name) const {
        if (!hasParticle(name)) {
	    ERROR("No particle named '%s' exists.", name.c_str());
	}
        return m_particles.at(name);
    }
    
    bool Event::hasParticle (const string& name) const {
         return m_particles.count(name) > 0;
    }
    
    GRL* Event::grl () const {
        assert( m_grl );
        return m_grl;
    }
    
    
    
    // High-level management method(s).
    void Event::clear () {
      m_info.clear();
      m_collections.clear();
      m_particles.clear();
      m_grl = nullptr;
      return;
    }
    
}
