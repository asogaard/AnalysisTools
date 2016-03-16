#include "AnalysisTools/Operation.h"

namespace AnalysisTools {
    
    // Set method(s).
    template <class T>
    void Operation<T>::setFunction (const function< double(T&) >& f) {
        m_function = f;
        return;
    }
    
    template <class T> // @asogaard: Move to Localised? (clearChildren)
    void Operation<T>::clearPlots () {
        m_plots.clear();
        return;
    }
    
    template <class T>
    void Operation<T>::addPlot (const CutPosition& pos, const IPlotMacro& plot) {
        PlotMacro1D<T>* storePlot = new PlotMacro1D<T>( dynamic_cast< const PlotMacro1D<T>& >(plot) );
        m_plots.at(pos).push_back( storePlot ); // @asogaard: Remove, and switch to 'm_children instead?
        return;
    }
  
    
    // Get method(s).
    template <class T>
    vector< IPlotMacro* > Operation<T>::plots (const CutPosition& pos) const {
        return m_plots.at(pos);
    }

    template <class T>
    vector< IPlotMacro* > Operation<T>::plots () const {
        vector< IPlotMacro* > plotsOut = m_plots.at(CutPosition::Pre);
        plotsOut.insert( plotsOut.end(), m_plots.at(CutPosition::Post).begin(), m_plots.at(CutPosition::Post).end() );
        return plotsOut;
    }
    
    
    // High-level management method(s).
    template <class T>
    bool Operation<T>::apply (T& obj) {

        assert(m_function);
        if (!m_initialised) { init(); }
        
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
            passes = (bool) val;
        }
        
        // * Post-cut distributions.
        if (passes) {
            for (auto& plot : plots(CutPosition::Post)) {
                ((PlotMacro1D<T>*) plot)->fill(obj);
            }
        }
        
        m_trees[CutPosition::Pre]->Fill();
        if (passes) {
            m_trees[CutPosition::Post]->Fill();
        }
        return passes;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Operation<T>::init () {
        
        assert ( this->dir() );
        
        this->dir()->cd();
        
        this->m_trees[CutPosition::Pre]  = new TTree("Precut",  "TTree with (pre-)cut value distribution");
        this->m_trees[CutPosition::Post] = new TTree("Postcut", "TTree with (post-)cut value distribution");
        
        for (auto plot : plots(CutPosition::Pre))  { plot->setTree(m_trees[CutPosition::Pre]);  }
        for (auto plot : plots(CutPosition::Post)) { plot->setTree(m_trees[CutPosition::Post]); }
        
        m_initialised = true;
        
        return;
    }
    
}

template class AnalysisTools::Operation<TLorentzVector>;
template class AnalysisTools::Operation<AnalysisTools::PhysicsObject>;
template class AnalysisTools::Operation<AnalysisTools::Event>;