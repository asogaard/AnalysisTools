#include "AnalysisTools/Selection.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T, class U>
    void Selection<T,U>::setWeight (const float* weight) {
        m_weight = weight;
        return;
    }
    
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
            addCut(cut, category, true);
        }
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addCut (const Cut<U>& cut, const string& pattern, const bool& common) {
        assert( !locked() );
        
        std::regex pieces_regex(pattern);
        std::smatch pieces_match;
        bool hasMatch = false;
        for (const auto& category : this->m_categories) {
            if (std::regex_match(category, pieces_match, pieces_regex)) {
                
                assert( hasCategory(category) );
                if (!common && m_branch < 0) { m_branch = (int) m_operations[category].size(); }
                this->m_operations[category].push_back( new Cut<U>(cut) );
                this->grab( this->m_operations[category].back(), category );
                hasMatch = true;
            }
        }
        
        if (!hasMatch) {
            cout << "<Selection<T,U>::addCut> Warning: No category was found to match pattern '" << pattern << "'.";
        }
        
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
            addOperation(operation, category, true);
        }
        return;
        
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const Operation<U>& operation, const string& category, const bool& common) {
        assert( !locked() );
        assert( hasCategory(category) );
        if (!common && m_branch < 0) { m_branch = (int) m_operations[category].size(); }
        this->m_operations[category].push_back( new Operation<U>(operation) );
        this->grab( this->m_operations[category].back(), category );
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const string& name, const function< double(U&) >& f) {
        Operation<U> operation(name);
        operation.setFunction(f);
        addOperation(operation);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addOperation (const string& name, const function< double(U&) >& f, const string& category) {
        Operation<U> operation(name);
        operation.setFunction(f);
        addOperation(operation, category);
        return;
    }
    
    template <class T, class U>
    void Selection<T,U>::addPlot (const CutPosition& pos, const PlotMacro1D<U>& plot) {
        /* Will only add this plot to the existing cuts. */
        for (IOperation* iop : allOperations()) {
            if (Cut<U>* cut = dynamic_cast< Cut<U>* > (iop)) {
                cut->addPlot(pos, PlotMacro1D<U>(plot));
            }
        }
        
        return;
    }
    
    
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
    // ...
    
    
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
        
        unsigned int nCuts = 0;
        for (IOperation* iop : this->m_operations[category]) {
            if (dynamic_cast< Cut<U>* >(iop) == nullptr) { continue; }
            nCuts++;
        }

        m_cutflow[category] = new TH1F("Cutflow", "", nCuts + 1, -0.5, nCuts + 0.5);
        
        // * Set bin labels.
        m_cutflow[category]->GetXaxis()->SetBinLabel(1, "All");
        unsigned int iCut = 1;
        for (IOperation* iop : this->m_operations[category]) {
            if (dynamic_cast< Cut<U>* >(iop) == nullptr) { continue; }
            m_cutflow[category]->GetXaxis()->SetBinLabel(++iCut, iop->name().c_str());
        }
        return;
    }
    
}

template class AnalysisTools::Selection<TLorentzVector, AnalysisTools::PhysicsObject>;
template class AnalysisTools::Selection<AnalysisTools::Event, AnalysisTools::Event>;