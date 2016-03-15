#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...

    
    // High-level management method(s).
    template <class T>
    bool ObjectDefinition<T>::run () {
        assert( this->m_input );
        /* *
         * Check that input- and info containers have same length.
         */
        for (const auto& category : this->m_categories) {  
            m_candidates[category].clear();
        }
        
        // * Set up PhysicsObject candidates. [Within a private 'init' function?]
        for (unsigned i = 0; i < this->m_input->size(); i++) {
            for (const auto& category : this->m_categories) {
                PhysicsObject p ((T) this->m_input->at(i));
                for (const auto& name_val : this->m_infoFloat) {
                    p.addInfo(name_val.first, (double) name_val.second->at(i));
                }
                for (const auto& name_val : this->m_infoInt) {
                    p.addInfo(name_val.first, (double) name_val.second->at(i));
                }
                for (const auto& name_val : this->m_infoBool) {
                    p.addInfo(name_val.first, (double) name_val.second->at(i));
                }

                m_candidates[category].push_back(p);
            }
        }
        
        // * Run selection.
        for (const auto& category : this->m_categories) {
            if (!m_candidates[category].size()) { continue; }
            if (!this->hasCutflow(category)) { this->setupCutflow(category); }
            unsigned int iCut = 0;
            this->m_cutflow[category]->Fill(iCut++, this->m_candidates[category].size());
            for (IOperation* iop : this->m_operations[category]) {
                // [Make use of branching?]
                
                // Loop candidates.
                for (unsigned i = this->m_candidates[category].size(); i --> 0; ) {
                    bool passes = false;
                    if        (Operation<PhysicsObject>* op  = dynamic_cast< Operation<PhysicsObject>* >(iop)) {
                        passes = op->apply(this->m_candidates[category].at(i));
                    } else if (Cut<PhysicsObject>*       cut = dynamic_cast< Cut<PhysicsObject>* >(iop)) {
                        passes = cut->apply(this->m_candidates[category].at(i));
                    } else {
                        cout << "<ObjectDefinition::run> Operation could not be cast to any known type." << endl;
                    }
                    
                    if (!passes) {
                        this->m_candidates[category].erase(this->m_candidates[category].begin() + i);
                    }
                }
                // dynamic_cast< Cut<T> > != NULL
                this->m_cutflow[category]->Fill(iCut++, this->m_candidates[category].size());
            }
        }
        this->m_hasRun = true;
        return true; /* Always true for ObjectDefinition (i.e. cannot break the analysis pipeline). */
    }
    
    template <class T>
    shared_ptr<PhysicsObjects> ObjectDefinition<T>::result () {
        assert( this->nCategories() == 1);
        return shared_ptr<PhysicsObjects>(&this->m_candidates[this->m_categories.front()]);
    }
    
    template <class T>
        shared_ptr<PhysicsObjects> ObjectDefinition<T>::result (const string& category) {
        assert( this->hasCategory(category) );
        return shared_ptr<PhysicsObjects>(&this->m_candidates[category]);
    }
    
    
    // Low-level management method(s).
    // ...

}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;
//template class AnalysisTools::ObjectDefinition<AnalysisTools::PhysicsObject>;