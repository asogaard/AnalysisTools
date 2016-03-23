#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T>
    void ObjectDefinition<T>::setInput (const vector<T>* input) {
        m_input = input;
        return;
    }
    
    template <class T>
    void ObjectDefinition<T>::addInfo (const string& name, const vector<double>* info) {
        assert( m_infoDouble.count(name) == 0);
        assert( m_infoFloat .count(name) == 0);
        assert( m_infoInt   .count(name) == 0);
        assert( m_infoBool  .count(name) == 0);
        m_infoDouble[name] = info;
        return;
    }
    
    template <class T>
    void ObjectDefinition<T>::addInfo (const string& name, const vector<float>* info) {
        assert( m_infoDouble.count(name) == 0);
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoFloat[name] = info;
        return;
    }
    
    template <class T>
    void ObjectDefinition<T>::addInfo (const string& name, const vector<int>* info) {
        assert( m_infoDouble.count(name) == 0);
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoInt[name] = info;
        return;
    }
    
    template <class T>
    void ObjectDefinition<T>::addInfo (const string& name, const vector<bool>* info) {
        assert( m_infoDouble.count(name) == 0);
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoBool[name] = info;
        return;
    }
    
    
    // Get method(s).
    template <class T>
    template <class W>
    const vector<W>* ObjectDefinition<T>::info (const string& name) {
        if (m_infoDouble.count(name) > 0) {
            return m_infoDouble(name);
        }
        if (m_infoInt.count(name) > 0) {
            return m_infoInt(name);
        }
        if (m_infoBool.count(name) > 0) {
            return m_infoBool(name);
        }
        return nullptr;
    }

    
    
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
                for (const auto& name_val : this->m_infoDouble) {
                    p.addInfo(name_val.first, (double) name_val.second->at(i));
                }
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
        
        float weight = 1.;
        if (this->m_weight) {
            weight = *this->m_weight;
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
                        passes = op->apply(this->m_candidates[category].at(i), weight);
                    } else if (Cut<PhysicsObject>*       cut = dynamic_cast< Cut<PhysicsObject>* >(iop)) {
                        passes = cut->apply(this->m_candidates[category].at(i), weight);
                    } else {
                        cout << "<ObjectDefinition::run> Operation could not be cast to any known type." << endl;
                    }
                    
                    if (!passes) {
                        this->m_candidates[category].erase(this->m_candidates[category].begin() + i);
                    }
                }

                if (dynamic_cast< Cut<PhysicsObject>* >(iop) == nullptr) { continue; }
                this->m_cutflow[category]->Fill(iCut++, this->m_candidates[category].size());
            }
        }
        this->m_hasRun = true;
        return true; /* Always true for ObjectDefinition (i.e. cannot break the analysis pipeline). */
    }
  
    template <class T>
    PhysicsObjects* ObjectDefinition<T>::result () {
        /* Make more general. */
        if (this->nCategories() == 0) {
            this->addCategory("Nominal");
        }
        this->lockCategories();
        assert( this->nCategories() == 1);
        return &this->m_candidates[this->m_categories.front()];
    }
    
    template <class T>
    PhysicsObjects* ObjectDefinition<T>::result (const string& category) {
        assert( this->hasCategory(category) );
        return &this->m_candidates[category];
    }
    
    
    // Low-level management method(s).
    // ...

}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;
//template class AnalysisTools::ObjectDefinition<AnalysisTools::PhysicsObject>;