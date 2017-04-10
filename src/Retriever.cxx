#include "AnalysisTools/Retriever.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/PhysicsObject.h"

namespace AnalysisTools {
  
  /// Set method(s).
  template<class T>
  void Retriever<T>::addInfo (const std::vector<std::string>& branches, const std::string& prefix) {
    addBranches_(branches, prefix);
    return;
  }
  
  template<class T>
  void Retriever<T>::addInfo (const std::string& name, const std::function< float(const T&) >& f) {
    if (m_infoFunctions.count(name) > 0) {
      WARNING("Info. '%s' has already been added. Overwriting.", name.c_str());
    }
    m_infoFunctions[name] = f;
    return;
  }
  
  template<class T>
  void Retriever<T>::setTree (TTree* tree) {
    m_tree = tree;
    clear();
    return;
  }

  
  /// Get method(s).
  // ...
  
  
  /// High-level method(s).
  template<class T>
  void Retriever<T>::clear () {
    m_formulas.clear();
    m_initialised = false;
  }

  template<class T>
  void Retriever<T>::rename (const std::string& name1, const std::string& name2) {
    if (name1 != name2) {
      m_rename[name1] = name2;
    }
    return;
  }
  
  template<class T>
  void Retriever<T>::retrieve () {
    DEBUG("Entering");

    // Check(s)
    if (!m_initialised) {
      initialise_();
    }

    // Clear storage container(s) to be filled below.
    clearCache_();

    // Has to be called for _each_ formula, to fill data. (?)
    for (const auto& f : m_formulas) { f->GetNdata(); }

    // Fill information.
    fillCache_();

    m_retrieved = true;

    DEBUG("Exiting");
    return;
  }


  /// Low-level method(s).
  template<class T>
  void Retriever<T>::addBranches_ (const std::vector<std::string>& branches, const std::string& prefix) {
    for (const std::string& branch : branches) {
      if (contains(m_branches, prefix + branch)) {
	WARNING("Branch '%s' already exists.", (prefix + branch).c_str());
      } else {
	m_branches.push_back(prefix + branch);
      }
      rename(prefix + branch, branch);
    }
    return;
  }

  template<class T>
  void Retriever<T>::initialise_ () {
    DEBUG("Entering");

    if (m_initialised) {
      WARNING("Has already been initialised.");
    }

    // @TODO: Check RetrieverMode (?) Only relevant for CollectionRetriever...
    if (!m_tree) {
      WARNING("No TTree set");
      return;
    }

    DEBUG("Clearing");
    clear();

    DEBUG("Setting up formulas.");
    for (const std::string& branch : m_branches) {
      m_formulas.push_back(makeUniqueMove<TTreeFormula>(new TTreeFormula(("f" + branch).c_str(), branch.c_str(), m_tree)));
      m_formulas.back()->SetQuickLoad(true);
    }

    // Branch-to-name hashing.
    for (unsigned i = 0; i < m_branches.size(); i++) {
      const std::string branch = m_branches[i];
      
      // Default name.
      std::string name = branch;

      // Check renaming requests, make sure no loops occur.
      std::vector<std::string> used;
      while (m_rename[name] != "") {
	if (contains(used, name)) {
	  WARNING("Cyclic loop:");
	  for (const auto& u : used) {
	    WARNING("  '%s' -> ", u.c_str());
	  }
	  WARNING("  '%s'", name.c_str());
	  break;
	}
	used.push_back(name);
	name = m_rename[name];
      }

      // Store possibly renamed name.
      m_branch_to_name[branch] = name;
    }

    m_initialised = true;

    DEBUG("Exiting");
    return;
  }

}

template class AnalysisTools::Retriever<AnalysisTools::Event>;
template class AnalysisTools::Retriever<AnalysisTools::PhysicsObject>;
