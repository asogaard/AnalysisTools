#include "AnalysisTools/Selection.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T>
    void Selection<T>::setName (const string& name) {
        assert( !m_locked );
        m_name = name;
        return;
    }
    
    
    // Get method(s).
    template <class T>
    bool Selection<T>::locked () {
        return m_locked;
    }
    
    template <class T>
    string Selection<T>::name    () {
        return m_name;
    }
    
    template <class T>
    unsigned               Selection<T>::nCategories      () {
        return m_categories.size();
    }
    
    template <class T>
    vector<string>         Selection<T>::categories       () {
        return m_categories;
    }
    
    template <class T>
    bool                   Selection<T>::categoriesLocked () {
        return m_categoriesLocked;
    }
    
    template <class T>
    map< string, CutsPtr > Selection<T>::cuts             () {
        return m_cuts;
    }
    
    
    // High-level management method(s).
    template <class T>
    void Selection<T>::addCategory (const string& category) {
        assert( !m_locked );
        assert( canAddCategories() );
        m_categories.push_back(category);
        m_cuts[category] = CutsPtr();
        return;
    }
    
    template <class T>
    void Selection<T>::addCategories (const vector<string>& categories) {
        for (const string& category : categories) {
            addCategory(category);
        }
        return;
    }

    template <class T>
    void Selection<T>::setCategories (const vector<string>& categories) {
        clearCategories();
        addCategories(categories);
        return;
    }
    
    template <class T>
    void Selection<T>::clearCategories () {
        m_categories.clear();
        m_cuts.clear();
        return;
    }
    
    template <class T>
    void Selection<T>::addCut (ICut* cut) {
        assert( !m_locked );
        cout << "<Selection<T>::addCut> Entering." << endl;
        cout << "<Selection<T>::addCut>   Number of categories: " << m_cuts.size() << endl;
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& cat_cuts : m_cuts) {
            addCut(cut, cat_cuts.first);
        }
        cout << "<Selection<T>::addCut> Exiting." << endl;
        return;
    }
    
    template <class T>
    void Selection<T>::addCut (ICut* cut, const string& category) {
        assert( !m_locked );
        assert( hasCategory(category) );
        grab( cut );
        m_cuts[category].push_back(cut);
        return;
    }
    
    template <class T>
    vector< TH1F* > Selection<T>::histograms () {
        cout << "<Selection<T>::histograms> Entering." << endl;
        vector< TH1F* > hists;
        cout << "<Selection<T>::histograms>   Number of cuts: " << m_cuts.size() << endl;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                for (auto& hist : cut->histograms()) {
                    hists.push_back( hist );
                }
            }
        }
        cout << "<Selection<T>::histograms> Exiting." << endl;
        return hists;
    }
    
    template <class T>
    CutsPtr Selection<T>::listCuts () {
        cout << "<Selection<T>::listCuts> Entering." << endl;
        CutsPtr cutsList;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                cutsList.push_back( cut );
            }
        }
        cout << "<Selection<T>::listCuts> Exiting." << endl;
        return cutsList;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Selection<T>::setDir (TDirectory* dir) {
        m_dir = dir;
        return;
    }
    
    template <class T>
    bool Selection<T>::hasCategory (const string& category) {
        return find(m_categories.begin(), m_categories.end(), category) != m_categories.end();
    }
    
    template <class T>
    bool Selection<T>::canAddCategories () {
        return !m_locked && !m_categoriesLocked;
    }
    
    template <class T>
    void Selection<T>::lockCategories () {
        m_categoriesLocked = true;
        return;
    }
    
    template <class T>
    void Selection<T>::lock () {
        m_locked = true;
        return;
    }
    
    template <class T>
    void Selection<T>::grab (ICut* cut) {

        cout << "<Selection<T>::grab> Entering." << endl;
        if (m_dir) {
            cut->setDir( m_dir->mkdir(cut->name().c_str()) );
            for (auto plot : cut->plots()) {
                cut->grab(plot);
            }
        }
        cout << "<Selection<T>::grab> Exiting." << endl;
        return;
    }
    
    
    template <class T>
    void Selection<T>::write () {
        for (auto cut : listCuts()) {
            cut->write();
        }
        return;
    }
    
}

template class AnalysisTools::Selection<TLorentzVector>;