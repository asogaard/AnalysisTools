#ifndef AnalysisTools_CommonOperations_h
#define AnalysisTools_CommonOperations_h

/**
 * @file CommonOperations.h
 * @author Andreas Sogaard
 */

// STL include(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Cut.h"

namespace AnalysisTools {

  /**
   * Cut on the transverse momentum, pT, of a physics object, in GeV.
   */
  Cut<PhysicsObject> cut_pt ("pt", [](const PhysicsObject& p) { 
      return p.Pt(); 
    });

  /**
   * Cut on the pseudorapidity, eta, of a physics object.
   */
  Cut<PhysicsObject> cut_eta ("eta", [](const PhysicsObject& p) { 
      return p.Eta();
    });

  /**
   * Cut on the mass, m, of a physics object, in GeV.
   */
  Cut<PhysicsObject> cut_m ("m", [](const PhysicsObject& p) { 
      return p.M();
    });

  /**
   * Cut on the number of entries in some collection.
   */
  Cut<Event> get_cut_num(const std::string& name) {
    Cut<Event> cut_num ("Num" + name, [name](const Event& e) { 
	return e.collection(name).size();
      });
    return cut_num;
  }

  /**
   * Cut on the value of auxiliary information variable of Event
   */
  inline Cut<Event> get_cut_event_info (const std::string& name, const float& scale = 1.) {
    Cut<Event> cut (name, [name, scale](const Event& e) {
        return e.info(name) * scale;
      });
    return cut;
  }

  /**
   * Cut on whether the auxiliary information variable is available to Event
   */
  inline Cut<Event> get_cut_event_hasInfo (const std::string& name) {
    Cut<Event> cut (name, [name](const Event& e) {
        return e.hasInfo(name);
      });
    return cut;
  }

  /**
   * Cut on the value of auxiliary information variable of leading PhysicsObject in some collection.
   */
  inline Cut<Event> get_cut_event_leading_info (const std::string& collection, const std::string& name, const float& scale = 1.) {
    Cut<Event> cut ("leading_" + collection + "_" + name, [collection, name, scale](const Event& e) {
	if (e.collection(collection).size() == 0) {
	  return -9999.;
	}
        return e.collection(collection).at(0)->info(name) * scale;
      });
    return cut;
  }

  /**
   * Cut on the value of auxiliary information variable of named particle.
   */
  inline Cut<Event> get_cut_event_particle_info (const std::string& particle, const std::string& name, const float& scale = 1.) {
    Cut<Event> cut (particle + "_" + name, [particle, name, scale](const Event& e) {
	if (!e.hasParticle(particle)) {
	  return -9999.;
	}
        return e.particle(particle).info(name) * scale;
      });
    return cut;
  }

  /**
   * Cut on the value of auxiliary information variable of PhysicsObject
   */
  inline Cut<PhysicsObject> get_cut_object_info (const std::string& name, const float& scale = 1.) {
    Cut<PhysicsObject> cut (name, [name, scale](const PhysicsObject& p) {
        return p.info(name) * scale;
      });
    return cut;
  }

} // namespace

#endif
