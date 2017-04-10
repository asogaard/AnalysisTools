#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T>
    void ObjectDefinition<T>::setInput (const vector<T>* input) {
        m_input = input;
        return;
    }
    
    
    // Get method(s).
    // ...

    
    // High-level management method(s).
  /**
   * Specialise for (1) template<TLorentzVector> and (2) template<PhysicsObject>
   * - if (1), have to create PhysicsObjects in run
   * - if (2), don't need to, and shouldn't be allowed to add info
   */
    template <class T>
    bool ObjectDefinition<T>::run () {
        DEBUG("Entering '%s'.", this->name().c_str());
        assert( this->m_input );
        /* *
         * Check that input- and info containers have same length.
         */
        for (const auto& category : this->m_categories) {
            m_candidates[category].clear();
        }
        
        // * Set up PhysicsObject candidates. [Within a private 'init' function?]
	prepareCandidates_();

        float weight = 1.;
        if (this->m_weight) {
            weight = *this->m_weight;
        }
        if (this->m_sum_weights && *this->m_sum_weights != 0) {
            weight /= *this->m_sum_weights;
        }
        
        // * Run selection.
        for (const auto& category : this->m_categories) {
            if (!m_candidates[category].size()) { continue; }
            if (!this->hasCutflow(category)) { this->setupCutflow(category); }
            unsigned int iCut = 0;
            this->m_cutflow[category]->Fill(iCut++, this->m_candidates[category].size() * weight);
            for (IOperation* iop : this->operations(category)) { 
                // [Make use of branching?]
                
                // Loop candidates.
                for (unsigned i = this->m_candidates[category].size(); i --> 0; ) {
                    bool passes = false;
		    if        (iop->operationType() == OperationType::Operation) {
		      Operation<PhysicsObject>* op  = static_cast< Operation<PhysicsObject>* >(iop);
		      passes = op->apply(this->m_candidates[category].at(i), weight);
                    } else if (iop->operationType() == OperationType::Cut) {
		      Cut<PhysicsObject>*       cut = static_cast< Cut<PhysicsObject>* >(iop);
		      passes = cut->apply(this->m_candidates[category].at(i), weight);
                    } else {
		      WARNING("Operation could not be cast to any known type.");
                    }
                    
                    if (!passes) {
                        this->m_candidates[category].erase(this->m_candidates[category].begin() + i);
                    }
                }

                if (iop->operationType() != OperationType::Cut) { continue; }
                this->m_cutflow[category]->Fill(iCut++, this->m_candidates[category].size() * weight);
            }

	    DEBUG("Number of candidates in '%s' after full selection: %d", this->name().c_str(), this->m_candidates[category].size());
	    //std::cout << " [" << &this->m_candidates[category] << "]" << std::endl;
        }

        this->m_hasRun = true;

	DEBUG("Exiting.");

        return true; /* Always true for ObjectDefinition (i.e. cannot break the analysis pipeline). */
    }
  
    template <class T>
    PhysicsObjects* const ObjectDefinition<T>::result () {
        /* Make more general. */
        if (this->nCategories() == 0) {
            this->addCategory("Nominal");
        }
        this->lockCategories();
        assert( this->nCategories() == 1);
        return &this->m_candidates[this->m_categories.front()];
    }
    
    template <class T>
    PhysicsObjects* const ObjectDefinition<T>::result (const string& category) {
        assert( this->hasCategory(category) );
        return &this->m_candidates[category];
    }
    
    template <class T>
    void ObjectDefinition<T>::print () const {
      INFO("  Configuration for object definition '%s':", this->name().c_str());
      for (const auto& cat_ops : this->m_operations) {
	INFO("    Category '%s':", cat_ops.first.c_str());
	for (const auto& iop : cat_ops.second) {
	  iop->print();
	}
      }
      return;
    }

    // Low-level management method(s).
    // ...
  template <>
  void ObjectDefinition<TLorentzVector>::prepareCandidates_ () {
    for (unsigned i = 0; i < this->m_input->size(); i++) {
      for (const auto& category : this->m_categories) {
	PhysicsObject p ((TLorentzVector) this->m_input->at(i));
	for (const auto& name_val : this->infoContainer<double>()) {
	  p.addInfo(name_val.first, (double) name_val.second->at(i));
	}
	for (const auto& name_val : this->infoContainer<float>()) {
	  p.addInfo(name_val.first, (double) name_val.second->at(i));
	}
	for (const auto& name_val : this->infoContainer<bool>()) {
	  p.addInfo(name_val.first, (double) name_val.second->at(i));
	}
	for (const auto& name_val : this->infoContainer<int>()) {
	  p.addInfo(name_val.first, (double) name_val.second->at(i));
	}
	
	m_candidates[category].emplace_back(p);
      }
    }
    return;
  }

  template <>
  void ObjectDefinition<PhysicsObject>::prepareCandidates_ () {
    for (const auto& category : this->m_categories) {
      m_candidates[category] = *this->m_input;
    }
    return;
  }


}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;
template class AnalysisTools::ObjectDefinition<AnalysisTools::PhysicsObject>;
