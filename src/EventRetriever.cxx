#include "AnalysisTools/EventRetriever.h"

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
      m_event.addInfo(name, m_formulas[i]->EvalInstance());
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
