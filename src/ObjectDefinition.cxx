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
        
        // Set up PhysicsObject candidates. [Within a private 'init' function?]
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
        
        // Run selection.

        for (const auto& category : this->m_categories) {
            if (!m_candidates[category].size()) { continue; }
            for (auto* cut : this->m_cuts[category]) {
                // [Make use of branching?]
                
                // Loop candidates.
                for (unsigned i = this->m_candidates[category].size(); i --> 0; ) {
                    bool passes = cut->select(this->m_candidates[category].at(i));
                    if (!passes) {
                        this->m_candidates[category].erase(this->m_candidates[category].begin() + i);
                    }
                }
                
            }
        }
        this->m_hasRun = true;
        return true; /* Always true for ObjectDefinition (i.e. cannot break the analysis pipeline). */
    }
    
    template <class T>
    PhysicsObjects* ObjectDefinition<T>::result () {
        //if (!this->m_hasRun) { run(); }
        if (this->nCategories() > 1) {
            //AnalysisTools::Warning("Call is ambiguous, since more than one category has been registered.");
            return nullptr; //PhysicsObjects();
        }
        return &this->m_candidates.begin()->second;
    }
    
    template <class T>
    PhysicsObjects* ObjectDefinition<T>::result (const string& category) {
        //if (!this->m_hasRun) { run(); }
        assert( this->hasCategory(category) );
        return &this->m_candidates[category];
    }
    
    
    // Low-level management method(s).
    // ...

}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;
//template class AnalysisTools::ObjectDefinition<AnalysisTools::PhysicsObject>;