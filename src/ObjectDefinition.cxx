#include "AnalysisTools/ObjectDefinition.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...

    
    // High-level management method(s).
    // ...
    
    template <class T>
    void ObjectDefinition<T>::run () {
        for (const auto& category : this->m_categories) {
            if (!m_candidates[category].size()) { continue; }
            for (const ICut* cutPtr : this->m_cuts[category]) {
                // Make use of branching?
                // Reduce candidates list.
                const Cut<T>& cut = dynamic_cast< const Cut<T>& >(*cutPtr);
                cut.select(*this->m_candidates[category].at(0));
                
            }
        }
        return;
    }
    
    template <class T>
    vector< T* > ObjectDefinition<T>::result () {
        if (!this->m_hasRun) { run(); }
        if (this->nCategories() > 1) {
            //AnalysisTools::Warning("Call is ambiguous, since more than one category has been registered.");
            return vector< T* >();
        }
        return (*this->m_candidates.begin()).second;
    }
    
    template <class T>
    vector< T* > ObjectDefinition<T>::result (const string& category) {
        if (!this->m_hasRun) { run(); }
        assert( this->hasCategory(category) );
        return this->m_candidates[category];
    }
    
    
    // Low-level management method(s).
    // ...

}

template class AnalysisTools::ObjectDefinition<TLorentzVector>;