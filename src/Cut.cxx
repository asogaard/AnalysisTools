#include "AnalysisTools/Cut.h"

namespace AnalysisTools {
    
    // Constructor(s).
    // ...
    
    
    // Set method(s).
    template <class T>
    void Cut<T>::clearRanges () {
        m_ranges.clear();
        return;
    }
    
    template <class T>
    void Cut<T>::setRange (const Range& range) {
        clearRanges();
        addRange(range);
        return;
    }
    
    template <class T>
    void Cut<T>::setRange (const pair<double, double>& limits) {
        clearRanges();
        addRange(limits);
        return;
    }
    
    template <class T>
    void Cut<T>::setRange (const double& down, const double& up) {
        clearRanges();
        addRange(down, up);
        return;
    }
    
    template <class T>
    void Cut<T>::setRanges (const Ranges& ranges) {
        m_ranges = ranges;
        return;
    }
    
    template <class T>
    void Cut<T>::setRanges (const vector< pair<double, double> >& vec_limits) {
        clearRanges();
        addRanges(vec_limits);
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const Range& range) {
        m_ranges.push_back(range);
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const pair<double, double>& limits) {
        m_ranges.push_back(Range(limits));
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const double& down, const double& up) {
        m_ranges.push_back(Range(down, up));
        return;
    }
    
    template <class T>
    void Cut<T>::addRanges (const Ranges& ranges) {
        for (const Range& range : ranges) {
            addRange(range);
        }
        return;
    }
    
    template <class T>
    void Cut<T>::addRanges (const vector< pair<double, double> >& vec_limits) {
        for (const auto& limits : vec_limits) {
            addRange(Range(limits));
        }
        return;
    }
    
    template <class T>
    void Cut<T>::setFunction (function< double(T) > f) {
        m_function = f;
        return;
    }
    
    template <class T>
    void Cut<T>::clearPlots () {
        m_plots.clear();
        return;
    }
    
    template <class T>
    void Cut<T>::addPlot (IPlotMacro* plot) {
        m_plots.push_back(plot);
        return;
    }
    
    template <class T>
    void Cut<T>::setName    (const string& name) {
        m_name = name;
        return;
    }
    
    template <class T>
    void Cut<T>::prependName (const string& prefix) {
        m_name = prefix + m_name;
        return;
    }
      
    
    // Get method(s).
    template <class T>
    string Cut<T>::name () const {
        return m_name;
    }
  
    template <class T>
    vector< IPlotMacro* > Cut<T>::plots () {
        return m_plots;
    }
    
    template <class T>
    vector< TH1F* > Cut<T>::histograms () {
        vector< TH1F* > hists;
        for (auto& plot : plots()) {
            hists.push_back( plot->histogram() );
        }
        return hists;
    }
    
    
    // High-level management method(s).
    template <class T>
    bool Cut<T>::select (const T& obj) const {
        assert(m_function);
        bool passes = false;
        double val = m_function(obj);
        if (m_ranges.size()) {
            for (const Range& range : m_ranges) {
                passes |= range.contains(val);
                if (passes) { break; }
            }
        } else {
            //Debug("No ranges provided. Interpreting output of cut '" << m_name << "' to be boolean.");
            passes = (val == 1);
        }
        return passes;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Cut<T>::setDir (TDirectory* dir) {
        m_dir = dir;
        return;
    }
    
    template <class T>
    void Cut<T>::setBasePlots () {

        /*
         AnalysisTools::PlotMacro1D<T> precut  (m_basedir + m_name + "/Pre-cut");
         AnalysisTools::PlotMacro1D<T> postcut (m_basedir + m_name + "/Post-cut");
         */
        PlotMacro1D<T>* precut  = new PlotMacro1D<T> ("Pre-cut");
        PlotMacro1D<T>* postcut = new PlotMacro1D<T> ("Post-cut");
        
        precut ->histogram()->SetBins(m_nBins, m_axisDown, m_axisUp);
        postcut->histogram()->SetBins(m_nBins, m_axisDown, m_axisUp);
        
        precut->setVariable(m_variable);
        precut->setUnit    (m_unit);
        
        precut ->setFunction(m_function);
        postcut->setFunction(m_function);
        
        addPlot(precut);
        addPlot(postcut);

        
        return;
    }
    

    template <class T>
    void Cut<T>::grab (IPlotMacro* plot) {
        if (m_dir) {
            plot->setDir( m_dir );
        }
        return;
    }
    
    template <class T>
    void Cut<T>::write () {
        for (auto plot : plots()) {
            plot->write();
        }
        return;
    }
    
}

template class AnalysisTools::Cut<TLorentzVector>;