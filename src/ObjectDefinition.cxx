#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...

    
    // High-level management method(s).
    /* *
     * This is not very well done. We're duplicating the code from the Selection class.
     *
     */
    /*
    template <class T>
    void ObjectDefinition<T>::addCut (Cut<PhysicsObject>* cut) {
        assert( !this->m_locked );
        cout << "<ObjectDefinition<T>::addCut> Entering." << endl;
        cout << "<ObjectDefinition<T>::addCut>   Number of categories: " << this->m_cuts.size() << endl;
        if (this->nCategories() == 0) {
            cout << "<ObjectDefinition<T>::addCut>   Adding category 'Nominal'." << endl;
            this->addCategory("Nominal");
        }
        this->lockCategories();
        for (const auto& cat_cuts : m_cuts) {
            this->addCut(cut, cat_cuts.first);
        }
        cout << "<ObjectDefinition<T>::addCut> Exiting." << endl;
        return;
    }
    
    template <class T>
    void ObjectDefinition<T>::addCut (Cut<PhysicsObject>* cut, const string& category) {
        assert( !this->m_locked );
        assert( this->hasCategory(category) );
        m_cuts[category].push_back( new Cut<PhysicsObject>(*cut) );
        this->grab( category, this->m_cuts[category].back() );
        return;
    }
     */
    
    template <class T>
    void ObjectDefinition<T>::run () {

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
        return;
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