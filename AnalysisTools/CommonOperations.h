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
      return p.Pt() / 1000.; 
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
      return p.M() / 1000.;
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
