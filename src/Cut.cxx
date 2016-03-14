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
    
    template <class T> // @asogaard: Move to Localised? (clearChildren)
    void Cut<T>::clearPlots () {
        m_plots.clear();
        return;
    }
    
    template <class T>
    void Cut<T>::addPlot (CutPosition pos, IPlotMacro* plot) {
        plot->setTree(m_trees[pos]);
        m_plots.push_back(plot); // @asogaard: Remove, and switch to 'm_children instead?
        return;
    }
  
    
    template <class T>
    void Cut<T>::prependName (const string& prefix) {
        this->m_name = prefix + this->m_name;
        return;
    }
    /* @TODO: If we should have this function, move to Localised? */
      
    
    // Get method(s).
    template <class T> // @asogaard: Remove? Switch to 'm_children'?
    vector< IPlotMacro* > Cut<T>::plots () const {
        return m_plots;
    }

    
    // High-level management method(s).
    template <class T>
    bool Cut<T>::select (const T& obj) {

        assert(m_function);
        if (!m_initialised) { init(); }
        
        // * Pre-cut distributions.
        for (auto& plot : plots()) {
            if (plot->tree() == m_trees[CutPosition::Pre]) {
                ((PlotMacro1D<T>*) plot)->fill(obj);
            }
        }
        /*
        / * @TODO: Loop branches (somehow)... * /
        for (auto& plot : plots()) {
            if (plot->name().find("Pre-cut") == string::npos) {
                continue;
            }
            ((PlotMacro1D<T>*) plot)->fill(obj);
        }
        */
        
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
            for (auto& plot : plots()) {
                if (plot->tree() == m_trees[CutPosition::Post]) {
                    ((PlotMacro1D<T>*) plot)->fill(obj);
                }
            }
            /* @TODO: Loop branches (somehow)... */
            /*
            for (auto& plot : plots()) {
                if (plot->name().find("Pre-cut") != string::npos) {
                    continue;
                }
                ((PlotMacro1D<T>*) plot)->fill(obj);
            }
             */
        }
        
        m_trees[CutPosition::Pre ]->Fill();
        if (passes) {
            m_trees[CutPosition::Post]->Fill();
        }
        return passes;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Cut<T>::init () {
        
        assert ( this->dir() );
        this->dir()->cd();
        
        m_trees[CutPosition::Pre]  = new TTree("Precut",  "TTree with (pre-)cut value distribution");
        m_trees[CutPosition::Post] = new TTree("Postcut", "TTree with (post-)cut value distribution");
        
        PlotMacro1D<T>* precut  = new PlotMacro1D<T> ("Pre-cut");
        PlotMacro1D<T>* postcut = new PlotMacro1D<T> ("Post-cut");
        
        precut ->setFunction(m_function);
        postcut->setFunction(m_function);
        
        addPlot(CutPosition::Pre,  precut);
        addPlot(CutPosition::Post, postcut);
        
        m_initialised = true;
        
        return;
    }
    
}

template class AnalysisTools::Cut<TLorentzVector>;
template class AnalysisTools::Cut<AnalysisTools::PhysicsObject>;
template class AnalysisTools::Cut<AnalysisTools::Event>;