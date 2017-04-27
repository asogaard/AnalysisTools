#include "AnalysisTools/EventRetriever.h"
#include "AnalysisTools/Type.h"

namespace AnalysisTools {
  
  /// High-level method(s).
  Event* EventRetriever::result () {
    return &m_event;
  }


  /// Low-level method(s).
  void EventRetriever::clearCache_ () {
    m_event.clear();
    return;
  }

  void EventRetriever::fillCache_ () {
    // Adding auxiliary information from TTree branches
    for (unsigned i = 0; i < m_branches.size(); i++) {
      const std::string name = m_branch_to_name.at(m_branches[i]);

      // If string-type branch.
      if (m_formulas[i]->IsString()) {
	for (unsigned j = 0; j < m_formulas[i]->GetNdata(); j++) {
	  // Store each string as a boolean flag.
	  m_event.addInfo(m_formulas[i]->EvalStringInstance(j), true);
	}
      } else {
	// Otherwise, evaluate as a _single_ floating point value
	m_event.addInfo(name, m_formulas[i]->EvalInstance());
      }
    }

    // Adding auxiliary information from functions.
    for (const auto& pair : m_infoFunctions) {
      const std::string name = pair.first;
      const function< float(const Event&) > f = pair.second;
      m_event.addInfo(name, f(m_event));
    }

    return;
  }

}
