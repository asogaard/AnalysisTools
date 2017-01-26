#include "AnalysisTools/Selection.h"

namespace AnalysisTools {

    // Set method(s).
    template <class T, class U>
    void Selection<T,U>::setWeight (const float* weight) {
        m_weight = weight;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCategory (const string& category) {
        assert( !locked() );
        assert( canAddCategories() );
        this->m_categories.emplace_back(category);
        this->m_operations.insert(std::make_pair(category, OperationPtrs()));
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCategories (const vector<string>& categories) {
        for (const string& category : categories) {
            addCategory(category);
        }
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::setCategories (const vector<string>& categories) {
        clearCategories();
        addCategories(categories);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::clearCategories () {
        this->m_categories.clear();
        this->m_operations.clear();
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const Cut<U>& cut) {
        assert( !locked() );
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& category : this->m_categories) {
            addCut(cut, category, true);
        }
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const Cut<U>& cut, const string& pattern, const bool& common) {
        assert( !locked() );
        
        std::regex pieces_regex(pattern);
        std::smatch pieces_match;
        bool hasMatch = false;
        for (const auto& category : this->m_categories) {
            if (std::regex_match(category, pieces_match, pieces_regex)) {
                
                assert( hasCategory(category) );
                if (!common && m_branch < 0) { m_branch = (int) m_operations[category].size(); }
                this->m_operations[category].emplace_back( makeUniqueMove( new Cut<U>(cut) ) );
                this->grab( this->m_operations[category].back().get(), category );
                hasMatch = true;
            }
        }
        
        if (!hasMatch) {
	  WARNING("No category was found to match pattern '%s'.", pattern.c_str());
        }
        
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f) {
        Cut<U> cut(name);
        cut.setFunction(f);
        addCut(cut);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const string& category) {
        Cut<U> cut(name);
        cut.setFunction(f);
        addCut(cut, category);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max) {
        Cut<U> cut(name);
        cut.setFunction(f);
        cut.addRange(min,max);
        addCut(cut);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max, const string& category) {
        Cut<U> cut(name);
        cut.setFunction(f);
        cut.addRange(min,max);
        addCut(cut, category);
        return;
    }

    template <class T, class U>
    void Selection<T,U>::removeCut (const unsigned& loc) {
      for (auto& cat_vec : this->m_operations) {
	removeCut(loc, cat_vec.first);
      }
      return;
    }

    template <class T, class U>
    void Selection<T,U>::removeCut (const unsigned& loc, const string& category) {
      assert(this->m_operations.count(category) > 0);
      assert(loc < this->m_operations[category].size());
      assert(dynamic_cast<Cut<U>*>(this->m_operations[category].at(loc).get()));
      this->m_operations[category].erase(this->m_operations[category].begin() + loc);
      return;
    }

    template <class T, class U>
    void Selection<T,U>::removeCut (const string& name) {
      for (auto& cat_vec : this->m_operations) {
	removeCut(name, cat_vec.first);
      }
      return;
    }
    
    template <class T, class U>
    void Selection<T,U>::removeCut (const string& name, const string& category) {
      DEBUG("Trying to remove operation '%s' from category '%s'", name.c_str(), category.c_str());
      assert(this->m_operations.count(category) > 0);
      auto it = this->m_operations[category].begin();
      for (; it != this->m_operations[category].end(); it++) {
	if ((*it)->name() == name && dynamic_cast<Cut<U>*>((*it).get())) {
	  this->m_operations[category].erase(it);
	  return;
	}
      }
      WARNING("No operation with name '%s' could be found.", name.c_str());
      return;
    }

    template <class T, class U>
    void Selection<T,U>::removeOperation (const unsigned& loc) {
      for (auto& cat_vec : this->m_operations) {
	removeOperation(loc, cat_vec.first);
      }
      return;
    }

    template <class T, class U>
    void Selection<T,U>::removeOperation (const unsigned& loc, const string& category) {
      assert(this->m_operations.count(category) > 0);
      assert(loc < this->m_operations[category].size());
      this->m_operations[category].erase(this->m_operations[category].begin() + loc);
      return;
    }

    template <class T, class U>
    void Selection<T,U>::removeOperation (const string& name) {
      for (auto& cat_vec : this->m_operations) {
	removeOperation(name, cat_vec.first);
      }
      return;
    }
    
    template <class T, class U>
    void Selection<T,U>::removeOperation (const string& name, const string& category) {
      DEBUG("Trying to remove operation '%s' from category '%s'", name.c_str(), category.c_str());
      assert(this->m_operations.count(category) > 0);
      auto it = this->m_operations[category].begin();
      for (; it != this->m_operations[category].end(); it++) {
	if ((*it)->name() == name) {
	  this->m_operations[category].erase(it);
	  return;
	}
      }
      WARNING("No operation with name '%s' could be found.", name.c_str());
      return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const Operation<U>& operation) {
        assert( !locked() );
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& category : this->m_categories) {
            addOperation(operation, category, true);
        }
        return;
        
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const Operation<U>& operation, const string& pattern, const bool& common) {
      /*
        assert( !locked() );
        assert( hasCategory(category) );
        if (!common && m_branch < 0) { m_branch = (int) m_operations[category].size(); }
        this->m_operations[category].emplace_back( makeUniqueMove( new Operation<U>(operation) ) );
        this->grab( this->m_operations[category].back().get(), category );
        return;
      */
      assert( !locked() );

      std::regex pieces_regex(pattern);
      std::smatch pieces_match;
      bool hasMatch = false;
      for (const auto& category : this->m_categories) {
	if (std::regex_match(category, pieces_match, pieces_regex)) {

	  assert( hasCategory(category) );
	  if (!common && m_branch < 0) { m_branch = (int) m_operations[category].size(); }
	  this->m_operations[category].emplace_back( makeUniqueMove( new Operation<U>(operation) ) );
	  this->grab( this->m_operations[category].back().get(), category );
	  hasMatch = true;
	}
      }

      if (!hasMatch) {
	WARNING("No category was found to match pattern '%s'.", pattern.c_str());
      }
      return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const string& name, const function< double(U&) >& f) {
        Operation<U> operation(name);
        operation.setFunction(f);
        addOperation(operation);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const string& name, const function< double(U&) >& f, const string& category) {
        Operation<U> operation(name);
        operation.setFunction(f);
        addOperation(operation, category);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addPlot (const CutPosition& pos, const PlotMacro1D<U>& plot) {
        /* Will only add this plot to the existing cuts. */
        for (IOperation* iop : allOperations()) { 
	    if (Cut<U>* cut = dynamic_cast< Cut<U>* > (iop)) {
	        cut->addPlot(pos, plot); //PlotMacro1D<U>(plot));
	    }
        }
        
        return;
    }
    
    
    // Get method(s).
    template <class T, class U>
    unsigned Selection<T,U>::nCategories () {
        return this->m_categories.size();
    }
    
    template <class T, class U>
    vector<string> Selection<T,U>::categories () {
      if (nCategories() == 0) {
	assert( !locked() );
	addCategory("Nominal");
      }
        return this->m_categories;
    }
    
    template <class T, class U>
    bool Selection<T,U>::categoriesLocked () {
        return this->m_categoriesLocked;
    }

  template <class T, class U>
  Cut<U>* Selection<T,U>::cut (const unsigned& loc, const string& category) {
    assert(this->m_operations.count(category) > 0);
    assert(loc < this->m_operations[category].size());
    if (Cut<U>* cut = dynamic_cast<Cut<U>*>(this->m_operations[category].at(loc).get())) {
      return cut;
    }
    WARNING("Operation %d/%d couldn't be cast as Cut<U>*.", loc + 1, this->m_operations[category].size());
    return nullptr;
  }

  template <class T, class U>
  Cut<U>* Selection<T,U>::cut (const string& name, const string& category) {
    assert(this->m_operations.count(category) > 0);
    auto it = this->m_operations[category].begin();
    for (; it != this->m_operations[category].end(); it++) {
      if ((*it)->name() == name) {
	if (Cut<U>* cut = dynamic_cast<Cut<U>*>((*it).get())) {
	  return cut;
	}
      }
    }
    WARNING("No cut with name '%s' could be found.", name.c_str());
    return nullptr;
  }
    
  template <class T, class U>
  IOperation* Selection<T,U>::operation (const unsigned& loc, const string& category) {
    assert(this->m_operations.count(category) > 0);
    assert(loc < this->m_operations[category].size());
    return this->m_operations[category].at(loc).get();
  }

  template <class T, class U>
  IOperation* Selection<T,U>::operation (const string& name, const string& category) {
    assert(this->m_operations.count(category) > 0);
    auto it = this->m_operations[category].begin();
    for (; it != this->m_operations[category].end(); it++) {
      if ((*it)->name() == name) {
	return (*it).get();
      }
    }
    WARNING("No operation with name '%s' could be found.", name.c_str());
    return nullptr;
  }
    
    template <class T, class U>
    std::vector<IOperation*> Selection<T,U>::operations (const string& category) const {
        assert( hasCategory(category) );
	std::vector<IOperation*> operationsList;
	for (auto& op : this->m_operations.at(category)) {
	  operationsList.emplace_back( op.get() );
	}
	return operationsList;
        //return this->m_operations.at(category);
    }
    
    template <class T, class U>
    std::vector<IOperation*> Selection<T,U>::allOperations () {
      std::vector<IOperation*> operationsList;
        for (auto& cat_ops :  this->m_operations) {
            for (auto& op : cat_ops.second) {
	        operationsList.emplace_back( op.get() );
            }
        }
        return operationsList;
    }
    
    template <class T, class U>
    TH1F* Selection<T,U>::cutflow (const string& category) {
        assert( hasCategory(category) );
        return this->m_cutflow.at(category).get();
    }
    
    template <class T, class U>
    bool Selection<T,U>::hasRun () {
        return this->m_hasRun;
    }
    
    
    // High-level management method(s).
    // ...
    
    
    // Low-level management method(s).
    template <class T, class U>
    bool Selection<T,U>::hasCategory (const string& category) const {
        return find(this->m_categories.begin(), this->m_categories.end(), category) != this->m_categories.end();
    }
    
    template <class T, class U>
    bool Selection<T,U>::canAddCategories () const {
        return !this->m_locked && !this->m_categoriesLocked;
    }
    
    template <class T, class U>
    void Selection<T,U>::lockCategories () {
        this->m_categoriesLocked = true;
        return;
    }
    
    
    template <class T, class U>
    bool Selection<T,U>::hasCutflow (const string& category) const {
        return (m_cutflow.count(category) > 0);
    }
    
    template <class T, class U>
    void Selection<T,U>::setupCutflow (const string& category) {
        this->dir()->cd(category.c_str());
        assert( hasCategory(category) );
        
        unsigned int nCuts = 0;
        for (auto& iop : this->m_operations[category]) { // IOperation* iop
	    if (dynamic_cast< Cut<U>* >(iop.get()) == nullptr) { continue; }
            nCuts++;
        }

        //m_cutflow[category] = makeUniqueMove( new TH1F("Cutflow", "", nCuts + 1, -0.5, nCuts + 0.5) );
	m_cutflow.insert(std::make_pair(category, std::unique_ptr<TH1F>( new TH1F("Cutflow", "", nCuts + 1, -0.5, nCuts + 0.5) )));
        
        // * Set bin labels.
        m_cutflow[category]->GetXaxis()->SetBinLabel(1, "All");
        unsigned int iCut = 1;
        for (auto& iop : this->m_operations[category]) { // IOperation*
	    if (dynamic_cast< Cut<U>* >(iop.get()) == nullptr) { continue; }
            m_cutflow[category]->GetXaxis()->SetBinLabel(++iCut, iop->name().c_str());
        }
        return;
    }
    
}

template class AnalysisTools::Selection<TLorentzVector, AnalysisTools::PhysicsObject>;
template class AnalysisTools::Selection<AnalysisTools::Event, AnalysisTools::Event>;
