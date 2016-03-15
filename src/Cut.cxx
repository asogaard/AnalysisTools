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
    void Cut<T>::setFunction (function< double(const T&) > f) {
        m_function = f;
        return;
    }
    
    template <class T> // @asogaard: Move to Localised? (clearChildren)
    void Cut<T>::clearPlots () {
        this->m_plots.clear();
        return;
    }
    
    template <class T>
    void Cut<T>::addPlot (CutPosition pos, IPlotMacro* plot) {
        this->m_plots.at(pos).push_back(plot); // @asogaard: Remove, and switch to 'm_children instead?
        return;
    }
  
    
    // Get method(s).
    template <class T>
    vector< IPlotMacro* > Cut<T>::plots (const CutPosition& pos) const {
        return this->m_plots.at(pos);
    }

    template <class T>
    vector< IPlotMacro* > Cut<T>::plots () const {
        vector< IPlotMacro* > plotsOut = this->m_plots.at(CutPosition::Pre);
        plotsOut.insert( plotsOut.end(), this->m_plots.at(CutPosition::Post).begin(), this->m_plots.at(CutPosition::Post).end() );
        return plotsOut;
    }
    
    // High-level management method(s).
    template <class T>
    bool Cut<T>::apply (const T& obj) {

        assert(m_function);
        if (!this->m_initialised) { init(); }
        
        // * Pre-cut distributions.
        for (auto& plot : plots(CutPosition::Pre)) {
            ((PlotMacro1D<T>*) plot)->fill(obj);
        }
        
        // * Selection.
        bool passes = false;
        double val = m_function(obj);
        if (m_ranges.size()) {
            for (const Range& range : m_ranges) {
                passes |= range.contains(val);
                if (passes) { break; }
            }
        } else {
            //Debug("No ranges provided. Interpreting output of cut '" << m_name << "' to be boolean.");
            passes = (bool) val;
        }
        
        // * Post-cut distributions.
        if (passes) {
            for (auto& plot : plots(CutPosition::Post)) {
                ((PlotMacro1D<T>*) plot)->fill(obj);
            }
        }
        
        this->m_trees[CutPosition::Pre]->Fill();
        if (passes) {
            this->m_trees[CutPosition::Post]->Fill();
        }
        return passes;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Cut<T>::init () {
        
        assert ( this->dir() );
        
        this->dir()->cd();
        
        this->m_trees[CutPosition::Pre]  = new TTree("Precut",  "TTree with (pre-)cut value distribution");
        this->m_trees[CutPosition::Post] = new TTree("Postcut", "TTree with (post-)cut value distribution");
        
        if (m_variable == "") {
            m_variable = "CutVariable";
        }
        
        PlotMacro1D<T>* precut  = new PlotMacro1D<T> (m_variable);
        PlotMacro1D<T>* postcut = new PlotMacro1D<T> (m_variable);
        
        precut ->setFunction(m_function);
        postcut->setFunction(m_function);
        
        addPlot(CutPosition::Pre,  precut);
        addPlot(CutPosition::Post, postcut);
        
        for (auto plot : plots(CutPosition::Pre))  { plot->setTree(this->m_trees[CutPosition::Pre]);  }
        for (auto plot : plots(CutPosition::Post)) { plot->setTree(this->m_trees[CutPosition::Post]); }
        
        this->m_initialised = true;
        
        return;
    }
    
}

template class AnalysisTools::Cut<TLorentzVector>;
template class AnalysisTools::Cut<AnalysisTools::PhysicsObject>;
template class AnalysisTools::Cut<AnalysisTools::Event>;