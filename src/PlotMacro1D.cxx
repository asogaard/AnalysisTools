#include "AnalysisTools/PlotMacro1D.h"

namespace AnalysisTools {

    // Set method(s).
    template <class T>
    void PlotMacro1D<T>::setFunction  (function< double(T) > f) {
        m_function = f;
        return;
    }
    
    /*
    template <class T>
    void PlotMacro1D<T>::setVariable  (const string& variable) {
        m_variable = variable;
        return;
    }

    template <class T>
    void PlotMacro1D<T>::setUnit  (const string& unit) {
        m_unit = unit;
        return;
    }
     */
    
    
    // Get method(s).
    // ...
    
    
    // High-level management method(s).
    template <class T>
    void PlotMacro1D<T>::fill (const T& obj) {
        assert( m_tree );
        m_value = m_function(obj);
        return;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void PlotMacro1D<T>::setTree (TTree* tree) {
        assert( tree );
        // @TODO: if m_tree is already set, remove branch before making a new one?
        m_tree = tree;
        tree->Branch((m_name != "" ? m_name : "value").c_str(), &m_value);
        return;
    }
    
    template <class T>
    TTree* PlotMacro1D<T>::tree () {
        return m_tree;
    }
    
}

template class AnalysisTools::PlotMacro1D<TLorentzVector>;
template class AnalysisTools::PlotMacro1D<AnalysisTools::PhysicsObject>;
template class AnalysisTools::PlotMacro1D<AnalysisTools::Event>;