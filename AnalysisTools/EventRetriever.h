#ifndef AnalysisTools_EventRetriever_h
#define AnalysisTools_EventRetriever_h

/**
 * @file EventRetriever.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <memory> /* std::unique_ptr */
#include <functional> /* std::function */

// ROOT include(s).
#include "TLorentzVector.h"
#include "TTreeFormula.h"

// AnalysisTools include(s).
#include "AnalysisTools/Logger.h"
#include "AnalysisTools/Retriever.h"
#include "AnalysisTools/Event.h"

namespace AnalysisTools {

  /**
   * Retriever of Events, constructuted from TTree branches.
   */
  class EventRetriever : public Retriever<Event> {

  public:

    /// Constructor(s)
    EventRetriever (const std::vector<std::string>& branches, const std::string& prefix = "") {
      addBranches_(branches, prefix);
    };
    
 
  public:
    /// High-level method(s).    
    // Return retrieved Event
    Event* result ();


  private:

    /// Low-level method(s)
    virtual void clearCache_ ();
    virtual void fillCache_  ();
    

  private:
    
    /// Data member(s)
    // Stored Event.
    Event m_event;
        
  };
  
} // namespace

#endif
