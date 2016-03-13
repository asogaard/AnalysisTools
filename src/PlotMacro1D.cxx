#include "AnalysisTools/PlotMacro1D.h"

namespace AnalysisTools {

    // Set method(s).
    template <class T>
    void PlotMacro1D<T>::setName      (const string& name) {
        m_name = name;
        if (m_ntuple) {
            m_ntuple->SetName((m_basedir + m_name).c_str());
        }
        return;
    }
    
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
    template <class T>
    string PlotMacro1D<T>::name () const {
        return m_name;
    }
    

    template <class T>
    TNtuple* PlotMacro1D<T>::ntuple () {
        return m_ntuple;
    }
    
    template <class T>
    TDirectory* PlotMacro1D<T>::dir () {
        return m_dir;
    }
    
    
    // High-level management method(s).
    template <class T>
    void PlotMacro1D<T>::fill (const T& obj) {
        if (!m_ntuple) {
            gDirectory = m_dir;
            //m_ntuple = new TNtuple(m_name.c_str(), m_variable.c_str(), "value");
            m_ntuple = new TNtuple(m_name.c_str(), "", "value");
        }
        m_ntuple->Fill( m_function(obj) );
        return;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void PlotMacro1D<T>::setDir (TDirectory* dir) {
        m_dir = dir;
        return;
    }
    
    template <class T>
    void PlotMacro1D<T>::write () {
        if (m_dir) { m_dir->cd(); } else {
            cout << "<PlotMacro1D<T>::write> No directory was provided. Writing to current (default) location." << endl;
        }
        //m_ntuple->Write(); // Not necessary. Leads to duplications.
        return;
    }
    
}

template class AnalysisTools::PlotMacro1D<TLorentzVector>;
template class AnalysisTools::PlotMacro1D<AnalysisTools::PhysicsObject>;
template class AnalysisTools::PlotMacro1D<AnalysisTools::Event>;