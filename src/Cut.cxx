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
        m_ranges.emplace_back(range);
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const pair<double, double>& limits) {
        m_ranges.emplace_back(Range(limits));
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const double& down, const double& up) {
        m_ranges.emplace_back(Range(down, up));
        return;
    }
    
    template <class T>
    void Cut<T>::addRange (const double& value) {
        m_ranges.emplace_back(Range(value - eps, value + eps));
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
    void Cut<T>::setFunction (const function< double(const T&) >& f) {
        m_function = f;
        return;
    }
    
    template <class T> // @asogaard: Move to Localised? (clearChildren)
    void Cut<T>::clearPlots () {
        this->m_plots.clear();
        return;
    }
    
    template <class T>
    void Cut<T>::addPlot (const CutPosition& pos, const IPlotMacro& plot) {
        /* Is it safe/smart to type cast to PlotMacro1D<T>? */
        /* Let IPlotMacro + PlotMacro1D<T> -> PlotMacro<T>? */
        try {
            const PlotMacro1D<T>& p  = dynamic_cast< const PlotMacro1D<T>& > (plot);
            //PlotMacro1D<T>* storePlot = new PlotMacro1D<T>( p );
            //this->m_plots.at(pos).emplace_back(storePlot);
	    this->m_plots.at(pos).emplace_back( makeUniqueMove( new PlotMacro1D<T>(p) ) );
        }
        catch (const std::bad_cast& e) {
            try {
                const PlotMacro1D<float>& p  = dynamic_cast< const PlotMacro1D<float>& >(plot);
		//PlotMacro1D<float>* storePlot = new PlotMacro1D<float>( p );
                //this->m_plots.at(pos).emplace_back(storePlot);
		this->m_plots.at(pos).emplace_back( makeUniqueMove( new PlotMacro1D<float>(p) ) );
	    } catch (const std::bad_cast& ee) {
                cout << "<Cut<T>::addPlot> Doesn't recognise template argument of plot '" << plot.name() << "'." << endl;
            }
        }
        return;
    }
  
    
    // Get method(s).
    template <class T>
    std::vector< IPlotMacro* > Cut<T>::plots (const CutPosition& pos) const {
        std::vector< IPlotMacro* > outputList;
	for (auto& plot : this->m_plots.at(pos)) {
	    outputList.emplace_back( plot.get() );
	}
	return outputList;
	// return this->m_plots.at(pos);
    }

    template <class T>
    std::vector< IPlotMacro* > Cut<T>::plots () const {
        std::vector< IPlotMacro* > plotsPre  = plots(CutPosition::Pre);
        std::vector< IPlotMacro* > plotsPost = plots(CutPosition::Post);
	//plotsOut.insert( plotsOut.end(), this->m_plots.at(CutPosition::Post).begin(), this->m_plots.at(CutPosition::Post).end() );
       	plotsPre.insert( plotsPre.end(), plotsPost.begin(), plotsPost.end() );
        return plotsPre;
    }

    template <class T>
    void Cut<T>::print () const {
      INFO("      Configuration for cut '%s':", name().c_str());
      return;
    }
    
    // High-level management method(s).
    template <class T>
    bool Cut<T>::apply (const T& obj, const float& w) {
        DEBUG("Entering.");
        assert(m_function);
        if (!this->m_initialised) { init(); }
        
        // * Pre-cut distributions.
	DEBUG("  Pre-cut distributions.");
        for (IPlotMacro* plot : plots(CutPosition::Pre)) {
	  DEBUG("    -- %s", plot->name().c_str());
            if (plot->name() == "weight") {
	      ((PlotMacro1D<float>*) plot)->fill(w);
            } else {
	      ((PlotMacro1D<T>*) plot)->fill(obj);
            }
        }
        
        // * Selection.
	DEBUG("  Selection.");
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
	    DEBUG("  Post-cut distributions.");
            for (IPlotMacro* plot : plots(CutPosition::Post)) {
                if (plot->name() == "weight") {
		  ((PlotMacro1D<float>*) plot)->fill(w);
                } else {
		  ((PlotMacro1D<T>*) plot)->fill(obj);
                }
            }
        }
        
	DEBUG("  Filling trees.");
        this->m_trees[CutPosition::Pre]->Fill();
        if (passes) {
            this->m_trees[CutPosition::Post]->Fill();
        }

	DEBUG("Exiting.");
        return passes;
    }
    
    
    // Low-level management method(s).
    template <class T>
    void Cut<T>::init () {
        DEBUG("Entering.");
        assert ( this->dir() );
        
        this->dir()->cd();
        
        this->m_trees[CutPosition::Pre]  = makeUniqueMove( new TTree("Precut",  "TTree with (pre-)cut value distribution") );
        this->m_trees[CutPosition::Post] = makeUniqueMove(  new TTree("Postcut", "TTree with (post-)cut value distribution") );
        
        if (m_variable == "") {
            m_variable = "CutVariable";
        }
        
        PlotMacro1D<T>     precut (m_variable);
        PlotMacro1D<T>     postcut(m_variable);
        PlotMacro1D<float> weight ("weight");
        
        precut .setFunction(m_function);
        postcut.setFunction(m_function);
        weight .setFunction([](const float& w) { return w; });
        
        addPlot(CutPosition::Pre,  weight);
        addPlot(CutPosition::Pre,  precut);
        addPlot(CutPosition::Post, weight);
        addPlot(CutPosition::Post, postcut);
        
        
        for (IPlotMacro* plot : plots(CutPosition::Pre))  { plot->setTree(this->m_trees[CutPosition::Pre] .get());  }
        for (IPlotMacro* plot : plots(CutPosition::Post)) { plot->setTree(this->m_trees[CutPosition::Post].get()); }
        
        this->m_initialised = true;
	DEBUG("Exiting.");
        return;
    }
    
}

template class AnalysisTools::Cut<TLorentzVector>;
template class AnalysisTools::Cut<AnalysisTools::PhysicsObject>;
template class AnalysisTools::Cut<AnalysisTools::Event>;
