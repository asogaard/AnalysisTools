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
        // Set up PhysicsObject candidates. [Within a private 'init' function?]
        cout << "------------------------------------" << endl;
        cout << "<ObjectDefinition<T>::run> Entering." << endl;
        assert( this->m_input );
        cout << "<ObjectDefinition<T>::run>   m_input: " << this->m_input << endl;
        cout << "<ObjectDefinition<T>::run>   Storing candidates as PhysicsObjects." << endl;
        cout << "<ObjectDefinition<T>::run>     Number of candidates: " << this->m_input->size() << endl;
        /* *
         * Check that input- and info containers have same length.
         */
        m_candidates.clear();
        for (unsigned i = 0; i < this->m_input->size(); i++) {
            cout << "<ObjectDefinition<T>::run>     Candidate " << i + 1 << "/" << this->m_input->size() << endl;
            for (const auto& category : this->m_categories) {
                cout << "<ObjectDefinition<T>::run>       Category: " << category << endl;
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
        cout << "<ObjectDefinition<T>::run>   Performing selection." << endl;
        for (const auto& category : this->m_categories) {
            cout << "<ObjectDefinition<T>::run>     Category: " << category << endl;
            cout << "<ObjectDefinition<T>::run>       Number of candidates: " << m_candidates[category].size() << endl;
            if (!m_candidates[category].size()) { continue; }
            for (const auto* cutPtr : this->m_cuts[category]) {
                // Make use of branching?
                // Reduce candidates list.
                //const Cut<T>& cut = dynamic_cast< const Cut<T>& >(*cutPtr);
                //cut.select(this->m_candidates[category].at(0));
                cutPtr->select(this->m_candidates[category].at(0));
                
            }
        }
        cout << "<ObjectDefinition<T>::run> Exiting." << endl;
        cout << "------------------------------------" << endl;
        return;
    }
    
    template <class T>
    PhysicsObjects ObjectDefinition<T>::result () {
        if (!this->m_hasRun) { run(); }
        if (this->nCategories() > 1) {
            //AnalysisTools::Warning("Call is ambiguous, since more than one category has been registered.");
            return PhysicsObjects();
        }
        return this->m_candidates.begin()->second;
    }
    
    template <class T>
    PhysicsObjects ObjectDefinition<T>::result (const string& category) {
        if (!this->m_hasRun) { run(); }
        assert( this->hasCategory(category) );
        return this->m_candidates[category];
    }
    
    
    // Low-level management method(s).
    // ...

}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;
//template class AnalysisTools::ObjectDefinition<AnalysisTools::PhysicsObject>;