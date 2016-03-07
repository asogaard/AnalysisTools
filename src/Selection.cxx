#include "AnalysisTools/Selection.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T, class U>
    void Selection<T,U>::setName (const string& name) {
        assert( !m_locked );
        m_name = name;
        return;
    }
    
    
    // Get method(s).
    template <class T, class U>
    bool Selection<T,U>::locked () {
        return m_locked;
    }
    
    template <class T, class U>
    string Selection<T,U>::name    () {
        return m_name;
    }
    
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
        assert( !m_locked );
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
        assert( !m_locked );
        cout << "<Selection<T,U>::addCut> Entering." << endl;
        cout << "<Selection<T,U>::addCut>   Number of categories: " << m_cuts.size() << endl;
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& cat_cuts : m_cuts) {
            addCut(cut, cat_cuts.first);
        }
        cout << "<Selection<T,U>::addCut> Exiting." << endl;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (Cut<U>* cut, const string& category) {
        assert( !m_locked );
        assert( hasCategory(category) );
        m_cuts[category].push_back( new Cut<U>(*cut) );
        grab( category, m_cuts[category].back() );
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::setInput (const vector<T>* input) {
        cout << "<Selection<T,U>::setInput> Entering." << endl;
        cout << "<Selection<T,U>::setInput>   input->size() = " << input->size() << endl;
        m_input = input;
        cout << "<Selection<T,U>::setInput>   m_input: " << m_input << endl;
        cout << "<Selection<T,U>::setInput> Exiting." << endl;
        return;
    }

    template <class T, class U>
    void Selection<T,U>::setInput (const vector<T>  input) {
        cout << "<Selection<T,U>::setInput> Entering." << endl;
        cout << "<Selection<T,U>::setInput>   input.size() = " << input.size() << endl;
        m_input = &input;
        cout << "<Selection<T,U>::setInput>   m_input: " << m_input << endl;
        cout << "<Selection<T,U>::setInput> Exiting." << endl;
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
    vector< TH1F* > Selection<T,U>::histograms () {
        cout << "<Selection<T,U>::histograms> Entering." << endl;
        vector< TH1F* > hists;
        cout << "<Selection<T,U>::histograms>   Number of cuts: " << m_cuts.size() << endl;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                for (auto& hist : cut->histograms()) {
                    hists.push_back( hist );
                }
            }
        }
        cout << "<Selection<T,U>::histograms> Exiting." << endl;
        return hists;
    }
    
    template <class T, class U>
    vector< ICut* > Selection<T,U>::listCuts () {
        cout << "<Selection<T,U>::listCuts> Entering." << endl;
        CutsPtr cutsList;
        for (auto& cuts : m_cuts) {
            for (auto& cut : cuts.second) {
                cutsList.push_back( cut );
            }
        }
        cout << "<Selection<T,U>::listCuts> Exiting." << endl;
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
    void Selection<T,U>::setDir (TDirectory* dir) {
        m_dir = dir;
        return;
    }
    
    template <class T, class U>
    bool Selection<T,U>::hasCategory (const string& category) {
        return find(m_categories.begin(), m_categories.end(), category) != m_categories.end();
    }
    
    template <class T, class U>
    bool Selection<T,U>::canAddCategories () {
        return !m_locked && !m_categoriesLocked;
    }
    
    template <class T, class U>
    void Selection<T,U>::lockCategories () {
        m_categoriesLocked = true;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::lock () {
        m_locked = true;
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::grab (const string& category, ICut* cut) {
        cout << "<Selection<T,U>::grab> Entering." << endl;
        cout << "<Selection<T,U>::grab>   Category: '" << category << "'" << endl;
        if (m_dir) {
            assert (cut);
            //m_dir->cd();
            TDirectory* categoryDir = m_dir;
            categoryDir->cd();
            if (!categoryDir->cd(category.c_str())) {
                categoryDir = categoryDir->mkdir(category.c_str());
            } else {
                categoryDir = gDirectory;
            }
            cout << "<Selection<T,U>::grab>   categoryDir->GetName(): " << categoryDir->GetName() << endl;
            cut->setDir( categoryDir->mkdir(cut->name().c_str()) );
            cout << "<Selection<T,U>::grab>   cut->dir()->GetName(): " << endl;
            for (auto plot : cut->plots()) {
                ((Cut<U>*) cut)->grab(plot);
            }
        }
        cout << "<Selection<T,U>::grab> Exiting." << endl;
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