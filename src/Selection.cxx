#include "AnalysisTools/Selection.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...
    
    
    // Get method(s).
    template <class T, class U>
    unsigned Selection<T,U>::nCategories () {
        return this->m_categories.size();
    }
    
    template <class T, class U>
    vector<string> Selection<T,U>::categories () {
        return this->m_categories;
    }
    
    template <class T, class U>
    bool Selection<T,U>::categoriesLocked () {
        return this->m_categoriesLocked;
    }
    
    template <class T, class U>
    OperationsPtr Selection<T,U>::operations (const string& category) {
        assert( hasCategory(category) );
        return this->m_operations.at(category);
    }
    
    template <class T, class U>
    OperationsPtr Selection<T,U>::allOperations () {
        OperationsPtr operationsList;
        for (auto& cat_ops :  this->m_operations) {
            for (auto& op : cat_ops.second) {
                operationsList.push_back( op );
            }
        }
        return operationsList;
    }
    
    template <class T, class U>
    TH1F* Selection<T,U>::cutflow (const string& category) {
        assert( hasCategory(category) );
        return this->m_cutflow.at(category);
    }
    
    template <class T, class U>
    bool Selection<T,U>::hasRun () {
        return this->m_hasRun;
    }
    
    
    // High-level management method(s).
    template <class T, class U>
    void Selection<T,U>::addCategory (const string& category) {
        assert( !locked() );
        assert( canAddCategories() );
        this->m_categories.push_back(category);
        this->m_operations[category] = OperationsPtr();
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
        this->m_categories.clear();
        this->m_operations.clear();
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const Cut<U>& cut) {
        assert( !locked() );
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& category : this->m_categories) {
            addCut(cut, category);
        }
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const Cut<U>& cut, const string& category) {
        assert( !locked() );
        assert( hasCategory(category) );
        this->m_operations[category].push_back( new Cut<U>(cut) );
        this->grab( this->m_operations[category].back(), category );
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f) {
        Cut<U> cut(name);
        cut.setFunction(f);
        addCut(cut);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const string& category) {
        Cut<U> cut(name);
        cut.setFunction(f);
        addCut(cut, category);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max) {
        Cut<U> cut(name);
        cut.setFunction(f);
        cut.addRange(min,max);
        addCut(cut);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max, const string& category) {
        Cut<U> cut(name);
        cut.setFunction(f);
        cut.addRange(min,max);
        addCut(cut, category);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const Operation<U>& operation) {
        assert( !locked() );
        if (nCategories() == 0) {
            addCategory("Nominal");
        }
        lockCategories();
        for (const auto& category : this->m_categories) {
            addOperation(operation, category);
        }
        return;

    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const Operation<U>& operation, const string& category) {
        assert( !locked() );
        assert( hasCategory(category) );
        this->m_operations[category].push_back( new Operation<U>(operation) );
        this->grab( this->m_operations[category].back(), category );
        return;
    }
    
    
    
    
    template <class T, class U>
    void Selection<T,U>::addPlot (CutPosition pos, const PlotMacro1D<U>& plot) {
        /* Will only add this plot to the existing cuts. */
        for (IOperation* iop : allOperations()) {
            // @TODO: if (dynamic_cast< Cut<U>* > (iop) == nullptr) { continue; }
            if (Cut<U>* cut = dynamic_cast< Cut<U>* > (iop)) {
                cut->addPlot(pos, new PlotMacro1D<U>(plot));
            }
        }
        
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
    
    
    // Low-level management method(s).
    template <class T, class U>
    bool Selection<T,U>::hasCategory (const string& category) {
        return find(this->m_categories.begin(), this->m_categories.end(), category) != this->m_categories.end();
    }
    
    template <class T, class U>
    bool Selection<T,U>::canAddCategories () {
        return !this->m_locked && !this->m_categoriesLocked;
    }
    
    template <class T, class U>
    void Selection<T,U>::lockCategories () {
        this->m_categoriesLocked = true;
        return;
    }
    
    
    template <class T, class U>
    bool Selection<T,U>::hasCutflow (const string& category) {
        return (m_cutflow.count(category) > 0);
    }
    
    template <class T, class U>
    void Selection<T,U>::setupCutflow (const string& category) {
        this->dir()->cd(category.c_str());
        assert( hasCategory(category) );
        
        unsigned int nCuts = this->m_operations[category].size();
        m_cutflow[category] = new TH1F("Cutflow", "", nCuts + 1, -0.5, nCuts + 0.5);
        
        // * Set bin labels.
        m_cutflow[category]->GetXaxis()->SetBinLabel(1, "All");
        unsigned int iCut = 1;
        for (IOperation* iop : this->m_operations[category]) {
            if (dynamic_cast< Cut<T>* >(iop) == nullptr) { continue; }
            m_cutflow[category]->GetXaxis()->SetBinLabel(++iCut, iop->name().c_str());
        }
        return;
    }
    
}

template class AnalysisTools::Selection<TLorentzVector, AnalysisTools::PhysicsObject>;
template class AnalysisTools::Selection<AnalysisTools::Event, AnalysisTools::Event>;