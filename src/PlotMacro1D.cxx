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
    template <class T>
    TNtuple* PlotMacro1D<T>::ntuple () {
        return m_ntuple;
    }
    
    // High-level management method(s).
    template <class T>
    void PlotMacro1D<T>::fill (const T& obj) {
        cout << "<PlotMacro1D<T>::fill> Entering." << endl;
        if (!m_ntuple) {
            cout << "<PlotMacro1D<T>::fill>   Creating new directory." << endl;
            cout << "<PlotMacro1D<T>::fill>   this->m_dir is null? " << (this->m_dir == nullptr ? "Yes" : "No") << endl;
            if (this->parentDir()) {
                gDirectory = this->parentDir();
            }
            //m_ntuple = new TNtuple(m_name.c_str(), m_variable.c_str(), "value");
            cout << "<PlotMacro1D<T>::fill>   Name: '" << this->m_name << "'" << endl;
            m_ntuple = new TNtuple(this->m_name.c_str(), "", "value");
        }
        cout << "<PlotMacro1D<T>::fill>   Filling." << endl;
        m_ntuple->Fill( m_function(obj) );
        cout << "<PlotMacro1D<T>::fill> Entering." << endl;
        return;
    }
    
    
    // Low-level management method(s).
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