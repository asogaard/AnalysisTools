#include "AnalysisTools/CollectionRetriever.h"

namespace AnalysisTools {
  
  /// High-level method(s).
  std::vector<PhysicsObject>* CollectionRetriever::result () {
    return &m_collection;
  }

  /// Low-level method(s).
  void CollectionRetriever::clearCache_ () {
    m_collection.clear();
    return;
  }

  void CollectionRetriever::fillCache_ () {

    // Get size of first kinematic array, i.e. number of objects in collection
    const unsigned N = m_formulas[0]->GetNdata();
    m_collection.resize(N);

    // Kinematics
    for (unsigned i = 0; i < N; i++) {
      PhysicsObject& p = m_collection.at(i);
      switch (m_mode) {
      case RetrieverMode::PxPyPzE :
	for (unsigned j = 1; j < 4; j++) { m_formulas[j]->GetNdata(); }
	p.SetPxPyPzE(m_formulas[0]->EvalInstance(i),
		     m_formulas[1]->EvalInstance(i),
		     m_formulas[2]->EvalInstance(i),
		     m_formulas[3]->EvalInstance(i));
	break;
      case RetrieverMode::PtEtaPhiE :
	for (unsigned j = 1; j < 4; j++) { m_formulas[j]->GetNdata(); }
	p.SetPtEtaPhiE(m_formulas[0]->EvalInstance(i),
		       m_formulas[1]->EvalInstance(i),
		       m_formulas[2]->EvalInstance(i),
		       m_formulas[3]->EvalInstance(i));
	break;
      case RetrieverMode::PtEtaPhiM :
	for (unsigned j = 1; j < 4; j++) { m_formulas[j]->GetNdata(); }
	p.SetPtEtaPhiM(m_formulas[0]->EvalInstance(i), 
		       m_formulas[1]->EvalInstance(i),
		       m_formulas[2]->EvalInstance(i),
		       m_formulas[3]->EvalInstance(i));
	break;
      case RetrieverMode::TLorentzVector :
	/* nop -- shouldn't happen */
	break;
      default:
	break;
      }
    }
    
    // Adding auxiliary infoformation from TTree branches
    unsigned i_info = (m_mode == RetrieverMode::TLorentzVector ? 1 : 4);
    for (; i_info < m_branches.size(); i_info++) {
      const std::string name = m_branch_to_name.at(m_branches[i_info]);
      for (unsigned i = 0; i < N; i++) {
 	m_collection[i].addInfo(name, m_formulas[i_info]->EvalInstance(i));
      }
    }

    // Adding auxiliary information from functions.
    for (const auto& pair : m_infoFunctions) {
      const std::string name = pair.first;
      const function< float(const PhysicsObject&) > f = pair.second;
      for (unsigned i = 0; i < N; i++) {
	m_collection[i].addInfo(name, f(m_collection[i]));
      }
    }

    return;
  }

}
