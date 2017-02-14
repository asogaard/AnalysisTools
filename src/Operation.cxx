#include "AnalysisTools/Operation.h"

namespace AnalysisTools {
  
  // Set method(s).
  template <class T>
  void Operation<T>::setFunction (const std::function< float(T&) >& f) {
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
    /* Is it safe/smart to type cast to PlotMacro1D<T>? */
    /* Let IPlotMacro + PlotMacro1D<T> -> PlotMacro<T>? */
    try {
      const PlotMacro1D<T>& p  = dynamic_cast< const PlotMacro1D<T>& > (plot);
      //PlotMacro1D<T>* storePlot = new PlotMacro1D<T>( p );
      //this->m_plots.at(pos).push_back(storePlot);
      this->m_plots.at(pos).push_back( makeUniqueMove( new PlotMacro1D<T>(p) ) );
    }
    catch (const std::bad_cast& e) {
      try {
	const PlotMacro1D<float>& p  = dynamic_cast< const PlotMacro1D<float>& >(plot);
	//PlotMacro1D<float>* storePlot = new PlotMacro1D<float>( p );
	//this->m_plots.at(pos).push_back(storePlot);
	this->m_plots.at(pos).push_back( makeUniqueMove( new PlotMacro1D<float>(p) ) );
      } catch (const std::bad_cast& ee) {
	cout << "<Operation<T>::addPlot> Doesn't recognise template argument of plot '" << plot.name() << "'." << endl;
      }
    }
    return;
  }
  
  
  // Get method(s).
  template <class T>
  std::vector< IPlotMacro* > Operation<T>::plots (const CutPosition& pos) const {
    std::vector< IPlotMacro* > outputList;
    for (auto& plot : this->m_plots.at(pos)) {
      outputList.push_back( plot.get() );
    }
    return outputList;
    // return this->m_plots.at(pos);
  }
  
  template <class T>
  std::vector< IPlotMacro* > Operation<T>::plots () const {
    std::vector< IPlotMacro* > plotsPre  = plots(CutPosition::Pre);
    std::vector< IPlotMacro* > plotsPost = plots(CutPosition::Post);
    //plotsOut.insert( plotsOut.end(), m_plots.at(CutPosition::Post).begin(), m_plots.at(CutPosition::Post).end() );
    plotsPre.insert( plotsPre.end(), plotsPost.begin(), plotsPost.end() );
    return plotsPre;
  }
  
  template <class T>
  void Operation<T>::print () const{
    INFO("      Configuration for operation '%s':", name().c_str());
    return;
  }
  
  // High-level management method(s).
  template <class T>
  bool Operation<T>::apply (T& obj, const float& w) {
    
    assert(m_function);
    if (!m_initialised) { init(); }
    
    // * Pre-cut distributions.
    for (IPlotMacro* plot : plots(CutPosition::Pre)) {
      if (plot->name() == "weight") {
	((PlotMacro1D<float>*) plot)->fill(w);
      } else {
	((PlotMacro1D<T>*) plot)->fill(obj);
      }
    }
    
    // * Selection.
    bool passes = false;
    float val = m_function(obj);
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
      for (IPlotMacro* plot : plots(CutPosition::Post)) {
	if (plot->name() == "weight") {
	  ((PlotMacro1D<float>*) plot)->fill(w);
	} else {
	  ((PlotMacro1D<T>*) plot)->fill(obj);
	}
      }
    }

    // Only fill trees if there are trees to fill.
    if (m_trees.size() > 0) {
      m_trees[CutPosition::Pre]->Fill();
      if (passes) {
	m_trees[CutPosition::Post]->Fill();
      }
    }

    return passes;
  }
  
  
  // Low-level management method(s).
  template <class T>
  void Operation<T>::init () {
    assert ( this->dir() );
    
    this->dir()->cd();
    
    // Only add trees if there are actually non-trivial plotting macros registered.
    if (plots().size() > 0) {
      
      this->m_trees[CutPosition::Pre]  = makeUniqueMove( new TTree("Precut",  "TTree with (pre-)cut value distribution") );
      this->m_trees[CutPosition::Post] = makeUniqueMove( new TTree("Postcut", "TTree with (post-)cut value distribution") );
      
      PlotMacro1D<float> weight ("weight");
      
      weight .setFunction([](const float& w) { return w; });
      
      addPlot(CutPosition::Pre,  weight);
      addPlot(CutPosition::Post, weight);
      
      for (auto plot : plots(CutPosition::Pre))  { plot->setTree(m_trees[CutPosition::Pre] .get()); }
      for (auto plot : plots(CutPosition::Post)) { plot->setTree(m_trees[CutPosition::Post].get()); }
    }
    
    m_initialised = true;
    
    return;
  }
  
}

template class AnalysisTools::Operation<TLorentzVector>;
template class AnalysisTools::Operation<AnalysisTools::PhysicsObject>;
template class AnalysisTools::Operation<AnalysisTools::Event>;
