#ifndef AnalysisTools_CommonPlots_h
#define AnalysisTools_CommonPlots_h

/**
 * @file CommonPlots.h
 * @author Andreas Sogaard
 */

// STL include(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/PlotMacro1D.h"

namespace AnalysisTools {

   // Standard kinematics of PhysicsObjects.
  // ---------------------------------------------------------------------------

  // Physics object pT.
  PlotMacro1D<PhysicsObject> plot_object_pt ("pt", [](const PhysicsObject& p) {
      return p.Pt();
    });

  // Physics object m.
  PlotMacro1D<PhysicsObject> plot_object_m ("m", [](const PhysicsObject& p) {
      return p.M();
    });

  // Physics object E.
  PlotMacro1D<PhysicsObject> plot_object_E ("E", [](const PhysicsObject& p) {
      return p.E();
    });

  // Physics object eta.
  PlotMacro1D<PhysicsObject> plot_object_eta ("eta", [](const PhysicsObject& p) {
      return p.Eta();
    });

  // Physics object phi.
  PlotMacro1D<PhysicsObject> plot_object_phi ("phi", [](const PhysicsObject& p) {
      return p.Phi();
    });


   // Standard kinematic of leading PhysicsObject in collection in Event 
  // ---------------------------------------------------------------------------

  // Leading PhysicsObject pT.
  inline PlotMacro1D<Event> get_plot_event_leading_pt (const std::string& collection, const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_pt");
    plot.setFunction([collection, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->Pt();
      });
    return plot;
  }


  // Leading PhysicsObject m.
  inline PlotMacro1D<Event> get_plot_event_leading_m (const std::string& collection, const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_m");
    plot.setFunction([collection, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->M();
      });
    return plot;
  }


  // Leading PhysicsObject E.
  inline PlotMacro1D<Event> get_plot_event_leading_E (const std::string& collection, const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_E");
    plot.setFunction([collection, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->E();
      });
    return plot;
  }


  // Leading PhysicsObject eta.
  inline PlotMacro1D<Event> get_plot_event_leading_eta (const std::string& collection, const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_eta");
    plot.setFunction([collection, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->Eta();
      });
    return plot;
  }


  // Leading PhysicsObject phi.
  inline PlotMacro1D<Event> get_plot_event_leading_phi (const std::string& collection, const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_phi");
    plot.setFunction([collection, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->Phi();
      });
    return plot;
  }


   // Standard kinematic of named PhysicsObject in Event 
  // ---------------------------------------------------------------------------

  // Particle PhysicsObject pT.
  inline PlotMacro1D<Event> get_plot_event_particle_pt (const std::string& particle) {
    PlotMacro1D<Event> plot (particle + "_pt");
    plot.setFunction([particle](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).Pt();
      });
    return plot;
  }


  // Particle PhysicsObject m.
  inline PlotMacro1D<Event> get_plot_event_particle_m (const std::string& particle) {
    PlotMacro1D<Event> plot (particle + "_m");
    plot.setFunction([particle](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).M();
      });
    return plot;
  }


  // Particle PhysicsObject E.
  inline PlotMacro1D<Event> get_plot_event_particle_E (const std::string& particle) {
    PlotMacro1D<Event> plot (particle + "_E");
    plot.setFunction([particle](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).E();
      });
    return plot;
  }


  // Particle PhysicsObject eta.
  inline PlotMacro1D<Event> get_plot_event_particle_eta (const std::string& particle) {
    PlotMacro1D<Event> plot (particle + "_eta");
    plot.setFunction([particle](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).Eta();
      });
    return plot;
  }


  // Particle PhysicsObject phi.
  inline PlotMacro1D<Event> get_plot_event_particle_phi (const std::string& particle) {
    PlotMacro1D<Event> plot (particle + "_phi");
    plot.setFunction([particle](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).Phi();
      });
    return plot;
  }


   // Auxiliary information of PhysicsObject
  // ---------------------------------------------------------------------------

  // ...
  inline PlotMacro1D<PhysicsObject> get_plot_object_info (const std::string& name, const float& scale = 1.) {
    PlotMacro1D<PhysicsObject> plot (name, [name, scale](const PhysicsObject& p) {
	return p.info(name) * scale;
      });
    return plot;
  }


   // Auxiliary information of Event
  // ---------------------------------------------------------------------------

  // ...
  inline PlotMacro1D<Event> get_plot_event_info (const std::string& name, const float& scale = 1.) {
    PlotMacro1D<Event> plot (name, [name, scale](const Event& e) {
	return e.info(name) * scale;
      });
    return plot;
  }


   // Auxiliary information for (sub-)leading or named PhysicsObject in collection in Event 
  // ---------------------------------------------------------------------------

  // ...
  inline PlotMacro1D<Event> get_plot_event_leading_info (const std::string& collection, const std::string& name, const float& scale = 1., const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("leading_" + collection + "_" + name);
    plot.setFunction([collection, name, scale, fallback](const Event& e) {
	if (e.collection(collection).size() < 1) { return fallback; }
	return (float) e.collection(collection).at(0)->info(name) * scale;
      });
    return plot;
  }

  // ...
  inline PlotMacro1D<Event> get_plot_event_subleading_info (const std::string& collection, const std::string& name, const float& scale = 1., const float& fallback = -9999.) {
    PlotMacro1D<Event> plot ("subleading_" + collection + "_" + name);
    plot.setFunction([collection, name, scale, fallback](const Event& e) {
	if (e.collection(collection).size() < 2) { return fallback; }
	return (float) e.collection(collection).at(1)->info(name) * scale;
      });
    return plot;
  }

  // ...
  inline PlotMacro1D<Event> get_plot_event_particle_info (const std::string& particle, const std::string& name, const float& scale = 1.) {
    PlotMacro1D<Event> plot (particle + "_" + name);
    plot.setFunction([particle, name, scale](const Event& e) {
	if (!e.hasParticle(particle)) { return float(-9999.); }
	return (float) e.particle(particle).info(name) * scale;
      });
    return plot;
  }




} // namespace

#endif
