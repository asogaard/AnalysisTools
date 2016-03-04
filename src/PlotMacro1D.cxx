#include "AnalysisTools/PlotMacro1D.h"

namespace AnalysisTools {

    // Set method(s).
    template <class T>
    void PlotMacro1D<T>::setName      (const string& name) {
        m_name = name;
        m_hist.SetName((m_basedir + m_name).c_str());
        return;
    }
    
    template <class T>
    void PlotMacro1D<T>::setHistogram (const TH1& hist) {
        m_hist.GetXaxis()->Copy(*hist.GetXaxis());
        // Sufficient?
        return;
    }
    
    template <class T>
    void PlotMacro1D<T>::setFunction  (function< double(T) > f) {
        m_function = f;
        return;
    }
    
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

    
    // Get method(s).
    template <class T>
    string PlotMacro1D<T>::name () const {
        return m_name;
    }
    
    template <class T>
    TH1F* PlotMacro1D<T>::histogram () {
        return &m_hist;
    }
    
    template <class T>
    TDirectory* PlotMacro1D<T>::dir () {
        return m_dir;
    }
    
    
    // High-level management method(s).
    template <class T>
    void PlotMacro1D<T>::fill (const T& obj) {
        fill(obj, 1.);
        return;
    }
    
    template <class T>
    void PlotMacro1D<T>::fill (const T& obj, const double& weight) {
        assert(m_function);
        m_hist.Fill( m_function(obj) , weight);
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
        histogram()->Write();
        return;
    }
    
}

template class AnalysisTools::PlotMacro1D<TLorentzVector>;