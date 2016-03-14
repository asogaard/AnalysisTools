#include "AnalysisTools/Selection.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...
    
    
    // Get method(s).
    template <class T, class U>
    unsigned               Selection<T,U>::nCategories      () {
        return m_categories.size();
    }
    
    template <class T, class U>
    vector<string>         Selection<T,U>::categories       () {
        return m_categories;
    }
    
    template <class T, class U>
    bool                   Selection<T,U>::categoriesLocked () {
        return m_categoriesLocked;
    }
    
    template <class T, class U>
    map< string, vector< Cut<U>* > > Selection<T,U>::cuts             () {
        return m_cuts;
    }
    
    
    // High-level management method(s).
    template <class T, class U>
    void Selection<T,U>::addCategory (const string& category) {
        assert( !locked() );
        assert( canAddCategories() );
        m_categories.push_back(category);
        m_cuts[category] = vector< Cut<U>* >(); //; CutsPtr();
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
        m_categories.clear();
        m_cuts.clear();
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (Cut<U>* cut) {
        assert( !locked() );
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& cat_cuts : m_cuts) {
            addCut(cut, cat_cuts.first);
        }
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (Cut<U>* cut, const string& category) {
        assert( !locked() );
        assert( hasCategory(category) );
        m_cuts[category].push_back( new Cut<U>(*cut) );
        //grab( category, m_cuts[category].back() );
        this->grab( m_cuts[category].back(), category );
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::setInput (const vector<T>* input) {
        m_input = input;
        return;
    }

    template <class T, class U>
    void Selection<T,U>::setInput (const vector<T>  input) {
        m_input = &input;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addInfo (const string& name, const vector<float>* info) {
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoFloat[name] = info;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addInfo (const string& name, const vector<int>* info) {
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoInt[name] = info;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addInfo (const string& name, const vector<bool>* info) {
        assert( m_infoFloat.count(name) == 0);
        assert( m_infoInt  .count(name) == 0);
        assert( m_infoBool .count(name) == 0);
        m_infoBool[name] = info;
        return;
    }
    
    template <class T, class U>
    template <class W>
    const vector<W>* Selection<T,U>::info (const string& name) {
        if (m_infoFloat.count(name) > 0) {
            return m_infoFloat(name);
        }
        if (m_infoInt.count(name) > 0) {
            return m_infoInt(name);
        }
        if (m_infoBool.count(name) > 0) {
            return m_infoBool(name);
        }
        return nullptr;
    }
    
    
    template <class T, class U>
    vector< TNtuple* > Selection<T,U>::ntuples () {
        vector< TNtuple* > ntuples;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                for (auto& ntuple : cut->ntuples()) {
                    ntuples.push_back( ntuple );
                }
            }
        }
        return ntuples;
    }
    
    template <class T, class U>
    vector< ICut* > Selection<T,U>::listCuts () {
        CutsPtr cutsList;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                cutsList.push_back( cut );
            }
        }
        return cutsList;
    }
    
    
    template <class T, class U>
    vector< ICut* > Selection<T,U>::cuts (const string& category) {
        vector< ICut* > out;
        for (Cut<U>* cut : m_cuts[category]) {
            out.push_back( (ICut*) cut );
        
        }
        return out;
    }
    
    
    // Low-level management method(s).
    template <class T, class U>
    bool Selection<T,U>::hasCategory (const string& category) {
        return find(m_categories.begin(), m_categories.end(), category) != m_categories.end();
    }
    
    template <class T, class U>
    bool Selection<T,U>::canAddCategories () {
        return !this->m_locked && !m_categoriesLocked;
    }
    
    template <class T, class U>
    void Selection<T,U>::lockCategories () {
        m_categoriesLocked = true;
        return;
    }
      
    template <class T, class U>
    void Selection<T,U>::write () {
        for (auto cut : listCuts()) {
            cut->write();
        }
        return;
    }
    
}

template class AnalysisTools::Selection<TLorentzVector, AnalysisTools::PhysicsObject>;
template class AnalysisTools::Selection<AnalysisTools::Event, AnalysisTools::Event>;