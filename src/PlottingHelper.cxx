#include "AnalysisTools/PlottingHelper.h"

namespace AnalysisTools {

  /// Set method(s).
  // ---------------------------------------------------------------------------

  template<class HistType>
  void PlottingHelper<HistType>::setInputName (const string& input) {
    size_t pos = input.find(":");
    if (pos != string::npos) {
      m_input  = input.substr(0, pos);
      m_branch = input.substr(pos + 1);
    } else {
      m_input = input;
    }
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setFilenames (const vector<string>& filenames) {
    m_filenames = filenames;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setOutfile  (TFile* outfile) {
    m_outfile = outfile;
    string outfilename (m_outfile->GetName());
    unsigned pos = outfilename.rfind("/");
    if (pos != string::npos) {
      m_outdir = outfilename.substr(0, pos + 1);
    }
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setSampleInfoFile (const string& sampleinfofile) {
    m_sampleinfofile = sampleinfofile;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setSettingsFile (const string& settingsfile) {
    m_settingsfile = settingsfile;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setPrintLines (const vector<string>& printlines) {
    m_printlines = printlines;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setXaxisTitle (const string& title) {
    m_axistitles.at(0) = title;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setYaxisTitle (const string& title) {
    m_axistitles.at(1) = title;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setZaxisTitle (const string& title) {
    m_axistitles.at(2) = title;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setAxis (const unsigned& nbins, const double& xmin, const double& xmax) {
    setXaxis(nbins, xmin, xmax);
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setAxis (const vector<double>& bins) {
    setXaxis(bins);
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setXaxis (const unsigned& nbinsx, const double& xmin, const double& xmax) {
    m_nbinsx = nbinsx;
    m_xmin   = xmin;
    m_xmax   = xmax;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setYaxis (const unsigned& nbinsy, const double& ymin, const double& ymax) {
    m_nbinsy = nbinsy;
    m_ymin   = ymin;
    m_ymax   = ymax;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setXaxis (const vector<double>& bins) {
    /* @TODO... */
    m_nbinsx = bins.size() - 1;
    m_xmin = bins.at(0);
    m_xmax = bins.at(m_nbinsx);
    const unsigned numBins (m_nbinsx);
    m_xbins = &bins[0];
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setYaxis (const vector<double>& bins) {
    /* @TODO... */
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::drawRatioPad (const bool& ratio) {
    m_ratio = ratio;
    if (!m_ratio) {
      m_height = 600;
    }
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::rebin (const unsigned& rebin) {
    m_rebin = rebin;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setLog (const bool& doLog) {
    m_log = doLog;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::computeImprovement (const int& improvementDirection) {
    if (improvementDirection > 2) {
      INFO("The provided number (%d) is larger than 2.", improvementDirection);
      return;
    }
    m_improvementDirection = improvementDirection;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::printBinContents (const bool& print) {
    m_print = print;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setScaleSignal (const double& scaleSignal) {
    m_scaleSignal = scaleSignal;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setScaleBackground (const std::string& name, const double& scale) {
    m_scaleBackground[name] = scale;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setPadding (const double& padding) {
    m_padding = padding;
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::setPulls (const bool& pulls) {
    m_pulls = pulls;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setNormalised (const bool& normalised) {
    m_normalised = normalised;
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::setSortBackgrounds (const bool& sortBackgrounds) {
    m_sortBackgrounds = sortBackgrounds;
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::addRegion (const std::vector<float>& limits, const std::string& title, const bool& snap, const bool& light) {
    m_regions.push_back(std::tuple<Range, std::string, bool, bool>(Range(limits), title, snap, light));
    return;
  }

  /*
  template<class HistType>
  void PlottingHelper<HistType>::addRegion (const std::vector<double>& limits, const std::string& title) {
    addRegion(std::vector<float>(limits.begin(), limits.end()), title);
    return;
  }
  */

  template<class HistType>
  void PlottingHelper<HistType>::addSystematic (const std::function< void(TH1F* syst, const TH1F* data, const TH1F* background) >& f) {
    // Store function for later evaluation
    m_systematicCalls.push_back(f);
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::addSystematic (const std::string& cat_up, const std::string& cat_down) {
    // Store category pair
    assert(cat_up != cat_down);
    assert(cat_up != "" && cat_down != "");
    assert(cat_up != "Nominal" && cat_down != "Nominal");
    m_systematicsCategories.push_back(std::make_pair(cat_up, cat_down));
    return;
  }

  
  
  /// Get method(s). 
  // ---------------------------------------------------------------------------

  template<class HistType>
  float PlottingHelper<HistType>::getDataEstimateAgreement (const std::vector<unsigned>& bins) {

    // Check(s)
    if (bins.size() == 0) {
      WARNING("No bins were provided");
      return 0;
    }
    if (!m_data) {
      WARNING("No data was loaded");
      return 0;
    }
    if (!m_background) {
      WARNING("No background estimate was loaded");
      return 0;
    }

    // Compute average z-statistic for requested bins.
    float deviation = 0.;
    for (const unsigned& bin : bins) {
      float data_val  = m_data->GetBinContent(bin);
      float data_stat = m_data->GetBinError  (bin);
      float est_val  = m_background->GetBinContent(bin);
      float est_stat = m_background->GetBinError  (bin);
      float syst = (m_systematicsSum ? m_systematicsSum->GetBinError(bin) : 0);
      deviation += std::fabs(data_val - est_val) / std::sqrt( sq(data_stat) + sq(est_stat) + sq(syst) );
    }
    deviation /= float(bins.size());

    INFO("Data estimate agreement (average z-statistic) for %d bins: %.2f", bins.size(), deviation);

    return deviation;
  }
  
  
  
  /// High-level management method(s).
  // ---------------------------------------------------------------------------

  template<class HistType>
  bool PlottingHelper<HistType>::draw () {

    DEBUG("Entering.");

    // General style settings/variables.      
    gStyle->SetOptStat(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLegendFillColor(0);
    
    const unsigned kMyBlue  = 1001u;
    const unsigned kMyRed   = 1002u;
    const unsigned kMyGreen = kGreen + 2;
    const unsigned kMyLightGreen = kGreen - 10;
    
    //TColor myBlue(kMyBlue,   0./255.,  30./255.,  59./255.);
    //TColor myRed (kMyRed,  205./255.,   0./255.,  55./255.);
    
    const unsigned kImprovementColor = kOrange + 7; // kViolet + 2

    // Make sure that an output file exists, such that we can write to it.
    assert( m_outfile );
    
    // Load sample information from file.
    if (!loadSampleInfo_()) {
      return false;
    }

    getSystematicsFromCategories_();

    // Load histograms.
    if (!loadHistograms_(m_input)) {
      return false;
    }

    // Setup canvas and pads as appropriate.
    if (!setupCanvas_()) {
      return false;
    }
        
    // Normalising.
    if (m_normalised) {
      DEBUG("Normalising.");

      // Initialise integral variable.
      float integral;
      
      // Data.
      if (m_data) {
	integral = m_data->Integral();
	if (integral > 0) { m_data->Scale(1/integral); }
      }
      
      // Background(s).
      if (m_sum) {
	integral = m_sum->Integral(); // @TODO: Fix
	if (integral > 0) {
	  m_sum->Scale(1/integral);
	  for (const auto& p : m_backgrounds) { p.second->Scale(1/integral); }
	}
      }
      
      // Signal.
      for (const auto& p : m_signals) {
	integral = p.second->Integral();
	if (integral > 0) { p.second->Scale(1/integral); }
      }
    }
    
    // Sort backgrounds by integral.
    m_backgroundsSorted.clear();

    if (m_sortBackgrounds) {
      // Sort by area, large at bottom.
      for (const auto& p : m_backgrounds) {
	m_backgroundsSorted.push_back(std::pair<std::string, HistType*>(p.first, p.second.get()));
      }
      std::sort(m_backgroundsSorted.begin(), m_backgroundsSorted.end(),
		[](const pair<string, HistType*> & p1, const pair<string, HistType*> & p2) -> bool
		{
		  return p1.second->Integral() < p2.second->Integral();
		});
    } else {
      // Try to order, top-to-bottom, and then order the remainder by alphabetic order.
      std::vector< std::string > tryOrder = {};
      for (const std::string& name : tryOrder) {
	if (m_backgrounds.count(name) > 0) {
	  m_backgroundsSorted.push_back(std::pair<std::string, HistType*>(name, m_backgrounds.at(name).get()));
	}
      }
      for (const auto& p : m_backgrounds) {
	if (std::find(tryOrder.begin(), tryOrder.end(), p.first) == tryOrder.end()) {
	  // Name 'p.first' is not in 'tryOrder' and thus hasn't been added yet.
	  m_backgroundsSorted.push_back(std::pair<std::string, HistType*>(p.first, p.second.get()));
	}
      }
      // Reverse.
      std::reverse(m_backgroundsSorted.begin(), m_backgroundsSorted.end()); 
    }


    // Scale backgrounds
    std::vector<std::string> scaled_backgrounds = {};
    for (const auto& p : m_backgroundsSorted) {
      if (m_scaleBackground.find(p.first) != m_scaleBackground.end()) {
	p.second->Scale(m_scaleBackground[p.first]);
	scaled_backgrounds.push_back(p.first);
      }
    }

    //  -- Check that all requested backgrounds were scaled
    for (const auto& p : m_scaleBackground) {
      if (!contains(scaled_backgrounds, p.first)) {
	WARNING("Background '%s' requested for scaling (%.2f) was not found.", p.first.c_str(), p.second);
      }
    }



    // Add _all_ background
    for (const auto& p : m_backgroundsSorted) {

      // Add to m_background _regardless of subtraction_
      if (m_background) {
        // If so contents of histogram to existing background.
        m_background->Add(p.second);
      } else {
        // Otherwise move first histogram to background.
        m_background = makeUniqueMove( (TH1F*) p.second->Clone("background") );
      }

    }


    // Add systematics
    /* Do this before subtracting background from data. */
    computeSystematics_();
    
    for (const auto& syst : m_systematics) {
      addToSystematicsSum_(syst.get());
    }

    

    // Add non-subtracted backgrounds
    std::unique_ptr<THStack> background (new THStack("StackedBackgrounds", ""));
    std::vector<std::string> subtract (m_subtract);
    m_sum = makeUniqueMove( (TH1F*) m_background->Clone("sum") );
    for (const auto& p : m_backgroundsSorted) {

      //if (m_scaleBackground.find(p.first) != m_scaleBackground.end()) {
      //p.second->Scale(m_scaleBackground[p.first]);
      //scaled_backgrounds.push_back(p.first);
      //}

      INFO(" -- Adding background '%s'.", p.first.c_str());
      std::cout << p.second << std::endl;
      INFO(" ----> Integral: %.4e", p.second->Integral());
      // Check whether to subtract this background.
      std::vector<std::string>::iterator pos = std::find(subtract.begin(), subtract.end(), p.first);
      if (pos != subtract.end()) {
	INFO(" ---- Subtracting");
	//m_data->Add(p.second, -1);
	for (unsigned i = 0; i < m_nbinsx; i++) {
	  double mc = p.second->GetBinContent(i + 1);
	  if (m_data) {
	    double d  = m_data->GetBinContent(i + 1);
	    m_data->SetBinContent(i + 1, d - mc);
	  }
	  m_sum->SetBinContent(i + 1, m_sum->GetBinContent(i + 1) - mc);
	}

	//m_sum ->Add(p.second, -1.);
	subtract.erase(pos);
	continue;
      }

      // If not, add to stack...
      background->Add(p.second);

      /*
      // ... and add to sum
      // Check whether the sum background MC distribution exists.
      if (m_sum) {
	// If so contents of histogram to existing sum.
	m_sum->Add(p.second);
      } else {
	// Otherwise move first histogram to sum.
	m_sum = makeUniqueMove( (TH1F*) p.second->Clone("sum") );
      }
      */

    }

    styleHist_(m_sum.get(),  true,  "StatsError");

    if (subtract.size() > 0) {
      WARNING("Following MC backgrounds were not subtracted:");
      for (const auto& name : subtract) {
	WARNING("-- '%s'", name.c_str());
      }
    }

    INFO(" ----> Data integral: %.4e", m_data->Integral());

    // Drawing (main pad).
    DEBUG("Going to first pad.");
    m_pads.first->cd();

    DEBUG("Drawing stacked background histogram...");
    background->Draw("HIST");
    gPad->Update();
    DEBUG("... Done!");
    
    // Add errors to zero-content data bins
    if (m_data) {
      /*
      for (unsigned i = 0; i < m_nbinsx; i++) {	
	if (m_data->GetBinContent(i + 1) < 0) {
	  m_data->SetBinContent(i + 1, 1.E-01);
	  m_data->SetBinError  (i + 1, 1.);
	}
      }
      */
    }

    // Get plot maximum.
    DEBUG("Getting plot maximum");
    double maxData = 0., maxMC = 0.;
    unsigned nBins = (m_data ? m_data->GetXaxis()->GetNbins() : (m_sum ? m_sum->GetXaxis()->GetNbins() : 0) );
    for (unsigned ibin = 0; ibin < nBins; ibin++) {
      if (m_data) { maxData = max(m_data->GetBinContent(ibin + 1), maxData); }
      if (m_sum)  { maxMC   = max(m_sum ->GetBinContent(ibin + 1), maxMC); }
    }
    
    const double plotmax = (m_plotmax != -9999 ? m_plotmax : max(maxData, maxMC));
    const double plotmin = (m_plotmin != -9999 ? m_plotmin : (m_normalised ? 1e-05 : 0.5)); // log-plots only
    
    // Setting y-axis range.
    DEBUG("Setting y-axis range.");
    if (m_log) {
      m_sum->GetYaxis()->SetRangeUser(plotmin, m_plotmax != -9999 ? plotmax : exp(m_padding * (log(plotmax) - log(plotmin)) + log(plotmin)));
    } else {
      m_sum->GetYaxis()->SetRangeUser(m_plotmin != -9999 ? plotmin : 0, m_plotmax != -9999 ? plotmax : m_padding * plotmax);
    }
    
    // Check x-axis labels.
    if (m_ratio) {
      m_sum->GetXaxis()->SetTitleOffset(9999.);
      m_sum->GetXaxis()->SetLabelOffset(9999.);
    }

    // -- Background
    DEBUG("Drawing background.");
    m_sum->DrawCopy("AXIS");
    background->Draw("HIST same");
    m_sum->DrawCopy("E2 same"); // E2
    
    // -- Signal
    DEBUG("Drawing signal.");
    for (const auto& p : m_signals) {
      DEBUG(" - %s", p.first.c_str());
      p.second->DrawCopy("HIST same");
    }
    
    // -- Data
    DEBUG("Drawing data");
    if (m_data) {
      //m_data->DrawCopy("PE0L same");
      m_data->DrawCopy("PE same");
    }
    m_sum->DrawCopy("AXIS same");

    // Draw cosmetic stuff
    drawArrows_ (plotmin, plotmax);
    drawRegions_(plotmin, plotmax);
    
    // Ratio distributions.
    if (m_data) {
      m_ratiohists["Ratio"] = makeUniqueMove((HistType*) m_data->Clone("DataMC_Ratio"));
      m_ratiohists["Pulls"] = makeUniqueMove((HistType*) m_data->Clone("DataMC_Pulls"));


      if (m_pulls) {
	// Data - MC pulls
	for (unsigned i = 0; i < m_sum->GetXaxis()->GetNbins(); i++) {
	  double c_data = m_ratiohists["Pulls"]->GetBinContent(i + 1);
	  double e_data = m_ratiohists["Pulls"]->GetBinError  (i + 1);
	  double c_MC = m_sum->GetBinContent(i + 1);
	  double e_MC = m_sum->GetBinError  (i + 1);
	  m_ratiohists["Pulls"]->SetBinContent(i + 1, e_data + e_MC > 0 ? (c_data - c_MC) / std::sqrt( std::pow(e_data, 2) + std::pow(e_MC, 2) ) : 0);
	}
      } else {
	// Data/MC ratio
	for (unsigned i = 0; i < m_sum->GetXaxis()->GetNbins(); i++) {
	  double c = m_ratiohists["Ratio"]->GetBinContent(i + 1);
	  double e = m_ratiohists["Ratio"]->GetBinError  (i + 1);
	  double cMC = m_sum->GetBinContent(i + 1);
	  m_ratiohists["Ratio"]->SetBinContent(i + 1, cMC > 0 ? c/cMC : 999.);
	  m_ratiohists["Ratio"]->SetBinError  (i + 1, cMC > 0 ? e/cMC : 0.);	
	}
      }
      
      m_ratiohists["StatsError"] = makeUniqueMove((HistType*) m_ratiohists["Ratio"]->Clone("DataMC_StatsError"));
      m_ratiohists["StatsError"]->SetDirectory(0);
      for (unsigned i = 0; i < m_ratiohists["StatsError"]->GetXaxis()->GetNbins(); i++) {
	m_ratiohists["StatsError"]->SetBinContent(i + 1, 1);
	double c = m_sum       ->GetBinContent(i + 1);
	double e = m_background->GetBinError  (i + 1);
	m_ratiohists["StatsError"]->SetBinError  (i + 1, (c > 0 ? e/c : 999.));
      }

      // Compute sum in quadrature of statistical and systematic uncertainty
      INFO("Computing sum in quadrature of statistical and systematic uncertainty");
      m_ratiohists["StatsSystError"] = makeUniqueMove((HistType*) m_ratiohists.at("Ratio")->Clone("DataMC_StatsSystError"));
      if (m_systematicsSum) {
	for (unsigned bin = 1; bin <= m_ratiohists.at("StatsSystError")->GetXaxis()->GetNbins(); bin++) {
	  float e1 = m_systematicsSum->GetBinError(bin);
	  float e2 = m_background    ->GetBinError(bin);
	  float c  = m_sum           ->GetBinContent(bin);
	  m_ratiohists.at("StatsSystError")->SetBinContent(bin, 1);
	  m_ratiohists.at("StatsSystError")->SetBinError(bin, c > 0 ? std::sqrt( std::pow(e1, 2) + std::pow(e2, 2) ) / c : 999. );
	}
      }
      
      // Styling.
      INFO("Styling");
      styleHist_(m_ratiohists.at("Ratio").get(),          false, "Ratio");
      styleHist_(m_ratiohists.at("StatsError").get(),     true,  "StatsError");
      styleHist_(m_ratiohists.at("StatsSystError").get(), true,  "StatsSystError");
      styleHist_(m_ratiohists.at("Pulls").get(),          false, "Pulls");

      std::vector< std::string > hist_names = { "Ratio", "StatsError", "StatsSystError", "Pulls" };

      double yminratio = 0.8, ymaxratio = 1.2;
      if (m_subtract.size() > 0) {
	yminratio = 0.;
	ymaxratio = 2.;
      }
      if (m_pulls) {
	yminratio = -5;
	ymaxratio =  5;
      }

      for (const std::string& hist_name : hist_names) {
	m_ratiohists.at(hist_name)->GetXaxis()->SetTitleSize(s_fontSizeM * 0.75/0.25);
	m_ratiohists.at(hist_name)->GetYaxis()->SetTitleSize(s_fontSizeM * 0.75/0.25);
	m_ratiohists.at(hist_name)->GetXaxis()->SetLabelSize(s_fontSizeM * 0.75/0.25);
	m_ratiohists.at(hist_name)->GetYaxis()->SetLabelSize(s_fontSizeM * 0.75/0.25);
	m_ratiohists.at(hist_name)->GetXaxis()->SetNdivisions(505);
	m_ratiohists.at(hist_name)->GetYaxis()->SetNdivisions(503);
	
	m_ratiohists.at(hist_name)->GetXaxis()->SetTitleOffset(1.3);
	m_ratiohists.at(hist_name)->GetYaxis()->SetTitleOffset(1.5 / (0.75/0.25));
	m_ratiohists.at(hist_name)->GetXaxis()->SetLabelOffset(0.02);
	
	m_ratiohists.at(hist_name)->GetXaxis()->SetTickLength(0.03 * 0.75/0.25);
	m_ratiohists.at(hist_name)->GetYaxis()->SetRangeUser(yminratio, ymaxratio);
      }

            
      if (m_ratio) {
	DEBUG("Drawing ratio pads.");
	m_pads.second->cd();
	
	if (m_pulls) {
	  m_ratiohists.at("Pulls")->GetYaxis()->SetTitle("Pulls");
	  m_ratiohists.at("Pulls")->Draw("HIST");
	} else {
	  //m_ratiohists.at("StatsError")->GetYaxis()->SetTitle("Data / Est."); // ... / MC
	  m_ratiohists.at("StatsError")->GetYaxis()->SetTitle("Data / MC");
	  m_ratiohists.at("StatsError")->Draw("E2"); 

	  if (m_systematicsSum) {

	    // Move systematics up.
	    for (unsigned bin = 1; bin <= m_ratiohists.at("StatsSystError")->GetXaxis()->GetNbins(); bin++) {
	      m_ratiohists.at("StatsSystError")->SetBinContent(bin, 1 + m_ratiohists.at("StatsSystError")->GetBinError(bin));
	    }
	    m_ratiohists.at("StatsSystError")->DrawCopy("HIST SAME");
	    
	    // Move systematics down.
	    for (unsigned bin = 1; bin <= m_ratiohists.at("StatsSystError")->GetXaxis()->GetNbins(); bin++) {
	      m_ratiohists.at("StatsSystError")->SetBinContent(bin, 1 - m_ratiohists.at("StatsSystError")->GetBinError(bin));
	    }
	    m_ratiohists.at("StatsSystError")->DrawCopy("HIST SAME");
	  } 
	  
	  //m_ratiohists.at("StatsSystErrorDown")->Draw("HIST SAME"); 
	  m_ratiohists.at("Ratio")->Draw("PE0L SAME");
	}
      }
      
      // Out-of-bounds indicators (@TODO: make work)
      auto h_data_up   = makeUniqueMove((HistType*) m_ratiohists.at("Ratio")->Clone("h_data_up"));
      auto h_data_down = makeUniqueMove((HistType*) m_ratiohists.at("Ratio")->Clone("h_data_down"));
      h_data_up  ->SetMarkerColor(kRed);
      h_data_down->SetMarkerColor(kBlue);
      for (unsigned i = 0; i < m_data->GetXaxis()->GetNbins(); i++) {
	double cu = h_data_up  ->GetBinContent(i + 1);
	double cd = h_data_down->GetBinContent(i + 1);
	double dy = (ymaxratio - yminratio) * 0.05;
	h_data_up  ->SetBinContent(i + 1, 0.9); //cu > ymaxratio ? ymaxratio - dy : cu);
	h_data_down->SetBinContent(i + 1, 0.9); //cd < yminratio ? yminratio + dy : cd);
	//h_data_up  ->SetBinError(i + 1, 0);
	//h_data_down->SetBinError(i + 1, 0);
      }
      h_data_up  ->Draw("P SAME");
      h_data_down->Draw("P SAME");
    }
   

    // Borders and axis ticks.
    DEBUG("Setting borders and axis ticks.");
    m_pads.first->SetLogy(m_log);
    m_pads.first->SetTickx();
    m_pads.first->SetTicky();
    m_pads.first->SetRightMargin (m_improvementDirection != -1 ? 0.10 : 0.05); // Default: 0.05
    m_pads.first->SetTopMargin   (0.05);
    m_pads.first->SetLeftMargin  (0.15);
    m_pads.first->SetBottomMargin(0.15);
    
    if (m_ratio) {
      m_pads.first->SetBottomMargin (0.025);
      
      m_pads.second->SetTickx();
      m_pads.second->SetTicky();
      m_pads.second->SetRightMargin (0.05);
      m_pads.second->SetTopMargin   (0.05);
      m_pads.second->SetLeftMargin  (0.15);
      m_pads.second->SetBottomMargin(0.40);
    }
    
    
    // Text.
    DEBUG("Drawing text.");
    m_pads.first->cd();
    
    TLatex latex;
    latex.SetTextSize(s_fontSizeL);
    latex.SetTextFont(72);
    
    string text;
    
    double xDraw = 0.21;
    double yDraw0 = 0.855;
    double yDraw  = yDraw0;
    double yStep = s_fontSizeM * 1.25;
    
    text = "ATLAS #font[42]{Internal}";
    latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
    yDraw -= yStep * 1.3;
    
    latex.SetTextSize(s_fontSizeM);
    latex.SetTextFont(42);
    /*
      text = "#sqrt{s} = 13 TeV";
      latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
      yDraw -= yStep;
      
      std::stringstream lumi_stream;
      lumi_stream << std::setprecision(2) << m_lumi;
      text = "#lower[-0.2]{#scale[0.5]{#int}}L dt = " + lumi_stream.str() + " fb^{-1}";
      latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
      yDraw -= yStep;
    */
    
    std::stringstream lumi_stream;
    lumi_stream << std::setprecision(3) << m_lumi; 
    text = "#sqrt{s} = 13 TeV,  L = " + lumi_stream.str() + " fb^{-1}";
    latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
    yDraw -= yStep;
    
    for (const string& line : m_printlines) {
      latex.DrawLatexNDC(xDraw, yDraw, line.c_str());
      yDraw -= yStep;
    }

    if (m_subtract.size() > 0) {
      std::string line = "Subtracting ";
      unsigned iline = 0;
      for (const std::string& name : m_subtract) {
	if (iline++ > 0) { line += ", "; }
	line += name;
	if (contains(scaled_backgrounds, name)) {
	  std::stringstream scale_stream;
	  scale_stream << std::setprecision(2) << m_scaleBackground[name];
	  line += " (#times " + scale_stream.str() + ")";
	}
      }
      latex.DrawLatexNDC(xDraw, yDraw, line.c_str());
      yDraw -= yStep;
    }
    
    
    // Legend.
    DEBUG("Drawing legend.");
    unsigned N = (m_data ? 1 : 0) + m_backgrounds.size() + m_signals.size() + 1 - m_subtract.size() + (m_systematicsSum ? 1 : 0);
    
    xDraw = 0.61; // 0.63
    yDraw = yDraw0 + 0.35 * yStep; // 0.95;
    
    double xmin = xDraw - yStep * 0.25 - (m_improvementDirection != -1 ? 0.05 : 0);
    double ymax = yDraw - yStep * 1;
    
    double width  = (m_improvementDirection != -1 ? 0.85 : 0.90) - xmin;
    double height = N * s_fontSizeM * 1.2;
    
    TLegend* legend = new TLegend(xmin, ymax - height, xmin + width, ymax);
    legend->SetTextSize(s_fontSizeM);
    legend->SetMargin(0.25);
    legend->SetFillStyle(0);
    
    // -- Data
    if (m_data) {
      legend->AddEntry(m_data.get(), "Data", "PEL");
    }
    
    // -- Background(s).
    //vector< pair<string, upHistType> >::iterator it = m_backgroundsSorted.end();
    auto it = m_backgroundsSorted.end();
    for (; it-- != m_backgroundsSorted.begin();) {
      if (std::find(m_subtract.begin(), m_subtract.end(), it->first) != m_subtract.end()) { continue; }

      // Scale factor for background
      double scale = contains(scaled_backgrounds, it->first) ? m_scaleBackground[it->first] : 1;

      stringstream stream;
      stream << fixed << setprecision(2) << scale;
      string s = stream.str();

      legend->AddEntry(it->second, (it->first + (scale != 1 ? " (#times " + s + ")": "")).c_str(), "F");
    }
    
    // -- Signal(s).
    for (const auto& p : m_signals) {
      stringstream stream;
      stream << fixed << setprecision(0) << m_scaleSignal; // setprecision(1)
      string s = stream.str();
      legend->AddEntry(p.second.get(), (p.first + (m_scaleSignal != 1 ? " #times " + s : "")).c_str(), "L");
    }
    
    // -- Stats. uncert.
    if (m_sum) {
      legend->AddEntry(m_sum.get(), "Stat. uncert.", "F");
    }

    // -- Stats. and syst. uncert.
    if (m_systematicsSum) {
      legend->AddEntry(m_ratiohists.at("StatsSystError").get(), "Stat. #oplus syst.", "F");
    }
    
    legend->Draw("same");
    
    
    // Lines.
    DEBUG("Drawing lines.");
    TLine line;
    if (m_ratio) {
      m_pads.second->cd();
      double axismin = m_ratiohists.at("StatsError")->GetXaxis()->GetXmin();
      double axismax = m_ratiohists.at("StatsError")->GetXaxis()->GetXmax();
      
      line.SetLineColor(kGray + 3);

      if (m_pulls) {
	line.DrawLine(axismin,  0, axismax,  0);
	line.SetLineStyle(2);
	line.DrawLine(axismin,  2, axismax,  2);
	line.DrawLine(axismin, -2, axismax, -2);
      } else {
	line.DrawLine(axismin, 1, axismax, 1);
      }
      
      //line.SetLineStyle(2);
      //line.DrawLine(axismin, 1.1, axismax, 1.1);
      //line.DrawLine(axismin, 0.9, axismax, 0.9);
    }
    
    // Cut improvements.
    if (m_improvementDirection != -1 && m_signals.size() > 0) {
      INFO("Computing cut improvements.");
      
      m_pads.first->SetTicky(false);
      m_pads.first->Update();
      
      // -- Fill.
      double x[m_nbinsx], y[m_nbinsx]; // @TOFIX
      double xMaxImpr = 0., yMaxImpr = 0.;
      for (unsigned i = 0; i < m_nbinsx; i++) {
	double S, B;
	switch (m_improvementDirection) {
	case 0: // Integrate upwards.
	  x[i] = m_sum->GetXaxis()->GetBinLowEdge(i + 1);
	  S = m_signals.begin()->second->Integral(i + 1, m_nbinsx + 1) / m_scaleSignal;
	  B = m_sum                    ->Integral(i + 1, m_nbinsx + 1);
	  break;             
	case 1: // Integrate downwards
	  x[i] = m_sum->GetXaxis()->GetBinUpEdge(i + 1);
	  S = m_signals.begin()->second->Integral(0, i + 1) / m_scaleSignal;
	  B = m_sum                    ->Integral(0, i + 1);
	  break;             
	case 2: // Select bin.
	  x[i] = m_sum->GetXaxis()->GetBinCenter(i + 1);
	  S = m_signals.begin()->second->GetBinContent(i + 1) / m_scaleSignal;
	  B = m_sum                    ->GetBinContent(i + 1);
	  break;             
	default:
	  break;
	}
	
	y[i] = S / sqrt(S + B + eps);
	if (y[i] > yMaxImpr) { 
	  yMaxImpr = y[i]; 
	  xMaxImpr = x[i];
	}
	if (i == m_nbinsx - 1) {
	  INFO(" -- Baseline sensitivity: %.4f", y[i]);
	}
      }
      INFO(" -- Maximal sensitivity: %.4f (at %.3f)", yMaxImpr, xMaxImpr);
      
      // -- Create and style.
      TGraph* impr = new TGraph(m_nbinsx, x, y);
      impr->SetMarkerColor(kImprovementColor);
      impr->SetMarkerStyle(20);
      impr->SetMarkerSize (0.8);
      impr->SetLineColor  (kImprovementColor);
      impr->SetLineWidth  (2);
      
      // -- Draw.
      m_pads.first->cd();
      TPad* transPad = new TPad("transPad", "", 0, 0, 1, 1);
      transPad->SetFillStyle(4000);
      
      // -- Get proper ranges for transparent pad.
      double ymin = 0;
      double ymax = yMaxImpr * m_padding * 1.1;
      double dy   = (ymax - ymin)/(1. - m_pads.first->GetTopMargin() - m_pads.first->GetBottomMargin());
      double dx   = (m_xmax - m_xmin)/(1. - m_pads.first->GetLeftMargin() - m_pads.first->GetRightMargin());
      
      transPad->Range(m_xmin - m_pads.first->GetLeftMargin()   * dx,	\
		      ymin   - m_pads.first->GetBottomMargin() * dy,	\
		      m_xmax + m_pads.first->GetRightMargin()  * dx,	\
		      ymax   + m_pads.first->GetTopMargin()    * dy);
      
      transPad->Draw();
      transPad->cd();
      impr->Draw("LP same");
      transPad->Update();
      
      TArrow arrow;
      arrow.SetLineWidth(3);
      arrow.SetLineColor(kImprovementColor + 2);
      arrow.SetFillColor(kImprovementColor + 2);
      switch (m_improvementDirection) {
      case 0: // Integrate upwards.
	arrow.DrawArrow(xMaxImpr - 0.06 * dx, yMaxImpr, xMaxImpr - 0.01 * dx, yMaxImpr, 0.015, "|>");       
	break;
      case 1: // Integrate downwards.
	arrow.DrawArrow(xMaxImpr + 0.06 * dx, yMaxImpr, xMaxImpr + 0.01 * dx, yMaxImpr, 0.015, "|>");
	break;
      case 2: // Select bin.
	arrow.DrawArrow(xMaxImpr, yMaxImpr + 0.06 * dy, xMaxImpr, yMaxImpr + 0.01 * dy, 0.015, "|>");
	break;
      default: 
	break;
      }
      
      // -- Draw axis on the right side of the pad.
      TGaxis* axis = new TGaxis(m_xmax, 0, m_xmax, ymax, 0, ymax, 50510, "+L");
      axis->SetNdivisions(505);
      axis->SetLineColor (kImprovementColor);
      axis->SetLabelColor(kImprovementColor);
      axis->SetTitleColor(kImprovementColor);
      axis->SetLabelFont(42);
      axis->SetTitleFont(42);
      axis->SetTitle("S / #sqrt{S + B}");
      axis->SetTitleOffset(1.15);
      axis->SetLabelOffset(0.01);
      axis->Draw();
      
    }
    
    
    // Print bin contents.
    if (m_print) {
      cout << "------------------------------------------------------------------------" << endl;
      
      const unsigned Nbins = m_sum->GetXaxis()->GetNbins();
      
      // -- Headers.
      cout << "Bin ";
      
      for (const auto& p : m_backgrounds) {
	cout << " & " << p.first.c_str();
      }
      
      cout << " & Total expected";
      
      for (const auto& p : m_signals) {
	cout << " & " << p.first;
      }
      
      if (m_data) {
	cout << " & Data";
      }
      
      cout << " \\\\ " <<  endl;
      
      // -- Bin contents.
      for (unsigned ibin = 0; ibin < Nbins; ibin++) {
	
	string label = string(m_sum->GetXaxis()->GetBinLabel(ibin + 1)); 
	if (label == "") {
	  cout << formatNumber( m_sum->GetBinCenter(ibin + 1) );
	} else {
	  printf("%-15s", label.c_str() );
	  //cout << label;
	}
	
	for (const auto& p : m_backgrounds) {
	  cout << " & " << formatNumber( p.second->GetBinContent(ibin + 1) );
	}
	
	cout << " & " << formatNumber( m_sum->GetBinContent(ibin + 1) );
	
	for (const auto& p : m_signals) {
	  cout << " & " << (m_scaleSignal == 0 ? "--" : formatNumber( p.second->GetBinContent(ibin + 1) / m_scaleSignal ));
	}
	
	if (m_data) {
	  cout << " & " << formatNumber( m_data->GetBinContent(ibin + 1) );
	}
	
	cout << " \\\\ " <<  endl;
	
      }
      
      cout << "------------------------------------------------------------------------" << endl;
    }
    
    
    // Writing.
    DEBUG("Writing canvas.");
    m_canvas->Write();
    string savename = replaceAll(m_input + "/" + m_branch + (m_improvementDirection != -1 ? "_improvement" : ""), "/", "__");
    INFO("Saving '%s'.", savename.c_str());
    m_canvas->SaveAs((m_outdir + savename + ".pdf").c_str());
    /*
      if (m_branch != "") {
      m_canvas->SaveAs((m_outdir + m_branch + ".pdf").c_str());
      } else {
      m_canvas->SaveAs((m_outdir + "TESTplot.pdf").c_str());
      }
    */

    INFO("Exiting.");
    INFO("");
    return true;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::save (const string& path) {
    /* ... */
    return;
  }
    


  /// Low-level management method(s).
  // ---------------------------------------------------------------------------

  template<class HistType>
  bool PlottingHelper<HistType>::setupCanvas_ () {

    // Make sure the the output file exists.
    assert(m_outfile);

    // Move to the output file.
    m_outfile->cd();

    // Check whether to draw ratio pad.
    drawRatioPad(m_ratio and m_data);

    // Create the canvas.
    std::string canvasName = (m_input + (m_branch != "" ? ":" : "") + m_branch);
    m_canvas = makeUniqueMove(new TCanvas(canvasName.c_str(), "", m_width, m_height));

    // Create necessary pads.
    m_canvas->cd();
    if (m_ratio) {
      m_pads.first  = makeUniqueMove(new TPad("pad1", "", 0.0, 0.25, 1.0, 1.0));
      m_pads.second = makeUniqueMove(new TPad("pad2", "", 0.0, 0.0, 1.0, 0.25));
      m_pads.first ->Draw();
      m_pads.second->Draw();
    } else {
      m_pads.first  = makeUniqueMove(new TPad("pad1", "", 0.0, 0.0, 1.0, 1.0));
      m_pads.first->Draw();
    }

    return true;
  }

  template<class HistType>
  std::unique_ptr<HistType> PlottingHelper<HistType>::getHistogram_ (TFile* file, 
								     const std::string& path) {

    if (!file) {
      ERROR("File doesn't exist, or is not provided.");
    }

    // Try to the object specified by the input name.
    TObject* obj = (TObject*) file->Get(path.c_str());
      
    if (obj == nullptr) {
      DEBUG("Requested input '%s' could not be retrieved from file '%s'.", path.c_str(), file->GetName());
      return nullptr;
    }
    
    // Initialise pointer to output histogram.
    std::unique_ptr<HistType> hist = makeUniqueMove(dynamic_cast<HistType*>(obj));

    if (hist) {
      
      // Reading histgram of specified type.
      hist->SetDirectory(0);     // < Keep in memory after file is closed!
      TH1::AddDirectory(kFALSE); // < --
      
      // Rebin.
      if (m_rebin > 1) {
	hist->Rebin(m_rebin);
      }
      
    } else if (TTree* tree = dynamic_cast<TTree*>(obj)) {
      
      /**
       * Assuming that the DSID information is store in the TTree: "<analysisname>/outputTree"
       */

      // Get analysis name.
      std::string analysisname = split(path, '/')[0] + "/" + split(path, '/')[1];
      std::string treename     = analysisname + "/outputTree";
      // Reading tree.
      /* @TODO: Distinguish between the number of ':' in the input name, to allow for TH2/TProfile as well. */
      unsigned DSID = 0;
      TTree* t = (TTree*) file->Get(treename.c_str());
      if (!tree) {
	DEBUG("Unable to retrieve TTree (%s) from which to read DSID information.", treename.c_str());
	return nullptr;
      }
      t->SetBranchAddress("DSID", &DSID);
      t->GetEntry(0);
      delete t;
      string histname = path + ":" + m_branch + "_autohist_" + to_string(DSID);
      
      // Move to output file.
      m_outfile->cd();

      // Create the histogram using the member axis variables.
      if (m_xbins == nullptr) {
	hist = makeUniqueMove(new TH1F(histname.c_str(), "", m_nbinsx, m_xmin, m_xmax));
      } else {
	hist = makeUniqueMove(new TH1F(histname.c_str(), "", m_nbinsx, m_xbins));
      }
      hist->Sumw2();
      /**
       * @TODO: Depending on type, set other axes.
       * hist->GetYaxis()->Set([], [], []);
       */

      // Initialise value and weight variables.
      double value, weight = 1.;
      tree->SetBranchAddress(m_branch.c_str(), &value);
      tree->SetBranchAddress("weight",         &weight);
      /**
       * @TODO: Depending on _number_ of branches, set up several (e.g. for HistType == TProfile).
       */

      // Fill histogram from tree.
      unsigned nEntries = tree->GetEntries();
      for (unsigned iEntry = 0; iEntry < nEntries; iEntry++) {
	tree->GetEntry(iEntry);
	hist->Fill(m_includeOverflow ? min(max(value, m_xmin + 1E-06), m_xmax - 1E-06) : value, weight); //(weight > 1000. ? 1. : weight));
	/**
	 * @TODO: Depending on HistType, call 'Fill()' with more argument (e.g. for HistType == TProfile).
	 */
      }
      
    } else {
      
      cout << "<PlottingHelper::getHistogram_> Warning: Requested input '" << m_input << "' could not be cast as either <class HistType> or TTree*." << endl;
      return nullptr;
      
    }

    return hist;
  }
  
  
  template<class HistType>
  bool PlottingHelper<HistType>::loadHistograms_ (const std::string& input) {
    
    DEBUG("Entering.");
    INFO("Getting histogram/tree: '%s'", input.c_str());

    // Check whether any file names were provided.
    if (m_filenames.size() == 0) {
      ERROR("No input files were provided.");
      return false;
    }
    
    // Initialise luminosity counter.
    //m_lumi = 0.;
    
    // Resetting
    m_data = nullptr;
    m_backgrounds.clear();

    // Looping input files.
    // -------------------------------------------------------------------------
   
    INFO("Looping %d input files.", m_filenames.size());
    unsigned it = 0;
    bool success = false;
    for (const string& filename : m_filenames) {
   
      // Make sure that the current file exists.
      if (!fileExists(filename)) {
	WARNING("Input file '%s' doesn't exist.", filename.c_str());
	continue;
      }
      
      // Open the current file.
      TFile file (filename.c_str(), "READ");

      std::unique_ptr<HistType> hist (getHistogram_ (&file, input));
      if (!hist) { continue; }
      else { success = true; }

      // Get DSID from filename.
      /**
       * @TODO: - Use regex?
       *        - Fallback to looking in tree?
       */
      unsigned DSID = 0;
      
      vector<string> path_fields = split(filename, '/');
      vector<string> filename_fields = split(path_fields.at(path_fields.size() - 1), '.');
      vector<string> file_fields = split(filename_fields.at(filename_fields.size() - 2), '_');
      string DSID_string = file_fields.at(file_fields.size() - 1);
      DSID = stoi(DSID_string);
      
      if (m_info.count(DSID) == 0) {
	WARNING("DSID %d was not found in m_info.", DSID);
	continue;
      }

      // Get sample info.
      SampleInfo info = m_info.at(DSID);
      
      bool isMC     = (info.type != SampleType::Data);
      bool isSignal = (info.type == SampleType::Signal);
      
      // Keep track of the actual amount of luminosity added.
      /*
      if (!isMC) {
	INFO("Adding %f to %f -> %5", info.lumi / 1000., m_lumi, m_lumi + info.lumi / 1000.);
	m_lumi += info.lumi / 1000.; // Scaling pb-1 -> fb-1.
      }
      */
            
      // Perform styling.
      styleHist_(hist.get(), isMC, info.name);
            
      // Add histogram to appropriate collection.
      if (isMC) {
	// Scaling MC samples by cross section.
	hist->Scale(info.xsec); // / float(info.evts)); // @TEMP: Assuming MC weights are scaled by one over sum of weights

	if (isSignal) {

	  // Scaling signal MC samples by optional scaling factor.
	  hist->Scale(m_scaleSignal);

	  // Make sure that each signal is unique.
	  if (m_signals.count(info.name) != 0) {
	    WARNING("Signal with name '%s' already encountered! Breaking.", info.name.c_str());
	  }
	  assert( m_signals.count(info.name) == 0 );

	  // Move histogram to signal collection.
	  m_signals[info.name] = std::move(hist);

	} else {
	  
	  /*
	  // Check whether the sum background MC distribution exists.
	  if (m_sum) {

	    // If so contents of histogram to existing sum.
	    m_sum->Add(hist.get());

	  } else {

	    // Otherwise move first histogram to sum.
	    m_sum = makeUniqueMove( (TH1F*) hist->Clone("sum") );

	  }
	  */

	  // Check whether this is the first sample of a given type.
	  if (m_backgrounds.count(info.name) == 0) {

	    // If so, move first histogram to background collection.
	    m_backgrounds[info.name] = std::move(hist);

	  } else {

	    // Otherwise add contents of histogram to existing collection.
	    m_backgrounds[info.name]->Add(hist.get());

	  }

	}

      } else {

	// Check whether the sum data distribution exists.
	if (m_data) {

	  // If so, add contents of histogram to existing data sum.
	  m_data->Add(hist.get());

	} else {

	  // Otherwise move first histogram to data sum.
	  m_data = std::move(hist);

	}
      }
            
    } // end: loop file names.

    // If no histograms were loaded, exit.
    if (!success) { 
      WARNING("No histograms were loaded. Exiting.");
      return false; 
    }
    
    // If no data is present, scale MC to 1 fb-1.
    if (m_lumi == 0.) { m_lumi = 1.; }
    
    // Scale MC distributions to amount of luminosity added.
    for (const auto& pair : m_signals) {
      pair.second->Scale(m_lumi);
    }
    for (const auto& pair : m_backgrounds) {
      if (pair.first.find("(d.d.)") == std::string::npos) {
	// Only scale MC backgrounds by luminosity (i.e. not data-driven ones)
	pair.second->Scale(m_lumi);
      }
    }
    //if (m_sum) {
    //  m_sum->Scale(m_lumi);
    //}
    
    // Perform styling.
    //styleHist_(m_sum.get(),  true,  "StatsError");

    styleHist_(m_data.get(), false, "Data");
    m_outfile->cd();
    
    DEBUG("Exiting.");
    
    return true;
  }
  
  
  template<class HistType>
  bool PlottingHelper<HistType>::loadSampleInfo_ () {

    INFO("Entering.");

    // Clear sample info container.
    m_info.clear();
    
    // Make sure that the sample info file is specified and exists.
    if (m_sampleinfofile == "") {
      ERROR("Please specify a sample info file.");
      return false;
    }
    if (!fileExists(m_sampleinfofile)) {
      ERROR("Sample info file '%s' was not found.", m_sampleinfofile.c_str());
      return false;
    }
    
    // Open sample info file.
    ifstream file ( m_sampleinfofile.c_str() );
    if (!file.is_open()) {
      ERROR("Sample info file could not be opened.");
      return false;
    }

    // Read each line in file.
    // Assumed format for MC is
    //   DSID, cross section [pb], num. events, gen. eff., physicsdatasetname
    // Assumed format for data is
    //   Run number, luminosity [pb-1], "Data" (without quotes)
    string line = "";
    while ( file.good() ) {
      
      // Get next line.
      std::getline ( file, line );
      line = trim(line);

      // If line is empty, or is a comment, skip it.
      if (line             == "")  { continue; }
      if (line.substr(0,1) == "#") { continue; }

      // Split line into fields, separated by commas.
      vector<string> fields = split(line, ',');

      // Initialised dataset ID/run number and SampleInfo variables.
      unsigned DSID;
      SampleInfo info;
      
      // Get dataset ID/run number.
      DSID = (unsigned) stoi(fields.at(0));
      //assert (m_info.count(DSID) == 0);
      if (m_info.count(DSID) > 0) {
	WARNING("Sample information for DSID %d has already been stored.", DSID);
	continue;
      }
      info.DSID = DSID;
      
      // Get cross section/luminosity.
      double xsec = stod(fields.at(1));
      
      // Check sample type.
      std::string firstChar = trim(fields.at(2)).substr(0,1);
      if (firstChar == "D" || firstChar == "d") { 
	/* If the third field starts with a "D"/"d", it's data. */

	// Set the sample info fields appropriate for data, and we're done.
	info.type = SampleType::Data;
	info.name = "Data";
	info.lumi = xsec;
	
      } else {
	/* Otherwise it's MC. */

	// Get number of events.
	unsigned evts = (unsigned) stoi(fields.at(2));
	
	if (evts == 0) {
	  WARNING("DSID %d has 0 events. Skipping.", DSID);
	  continue; 
	}
	info.evts = evts;
	
	// Get generator filter efficiency.
	double eff = stod(fields.at(3));
	
	// Get physics name.
	string name = "NA";
	string ldn = (string) fields.at(4);
	ldn = trim(ldn);
	
	// Determine MC type from physics ldn.
	bool signal = false;

	std::regex re_sherpaInclGamma1("Sherpa_CT10_SinglePhotonPt([0-9]+).*");
	std::regex re_sherpaInclGamma2("Sherpa_CT10_SinglePhotonPt([0-9]+)_([0-9]+).*");

	std::regex re_sherpaMultijets("Sherpa_CT10_jets_JZ([0-9]+)");
	std::regex re_pythiaMultijets("Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ([0-9]+)W");

	std::regex re_sherpaInclW("Sherpa_CT10_Wqq_Pt.*");
	std::regex re_sherpaInclZ("Sherpa_CT10_Zqq_Pt.*");
	std::regex re_pythiaInclW("Pythia8EvtGen_A14NNPDF23LO_WHad_.*");
	std::regex re_pythiaInclZ("Pythia8EvtGen_A14NNPDF23LO_ZHad_.*");

	std::regex re_sherpaGammaW("Sherpa_CT10_WqqGammaPt.*");
	std::regex re_sherpaGammaZ("Sherpa_CT10_ZqqGammaPt.*");

	std::regex re_powhegTtbar("PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad");

	std::regex re_ISRgammaSignal1(".*dmA_jja_Ph([0-9]+)_mRp0([0-9]+).*");
	std::regex re_ISRgammaSignal2(".*dmA_jja_Ph([0-9]+)_mRp([0-9]+).*");
	std::regex re_ISRjetSignal(".*dmA_jjj_Jet([0-9]+).*_mRp([0-9]+).*");

	std::smatch re_match_name;
	if (info.DSID < 200000) {
	  name = "Incl. #gamma (d.d.)";
	} else if (std::regex_match(ldn, re_match_name, re_sherpaInclGamma2)) {
	  name = "Incl. #gamma"; // @TEMP
	  // p_{T} #in [" + string(re_match_name[1]) + ", " + string(re_match_name[2]) + "] GeV";
	} else if (std::regex_match(ldn, re_match_name, re_sherpaInclGamma1)) {
	  name = "Incl. #gamma"; // @TEMP
	  // p_{T} > " + string(re_match_name[1]) + " GeV";
	} 
	else if (std::regex_match(ldn, re_match_name, re_sherpaMultijets)) { name = "QCD multijets"; }
	else if (std::regex_match(ldn, re_match_name, re_pythiaMultijets)) { name = "QCD multijets (pythia)"; }
	else if (std::regex_match(ldn, re_match_name, re_sherpaInclW))     { name = "Incl. W"; }
	else if (std::regex_match(ldn, re_match_name, re_sherpaInclZ))     { name = "Incl. Z"; }
	else if (std::regex_match(ldn, re_match_name, re_pythiaInclW))     { name = "Incl. W (pythia)"; }
	else if (std::regex_match(ldn, re_match_name, re_pythiaInclZ))     { name = "Incl. Z (pythia)";	}
	else if (std::regex_match(ldn, re_match_name, re_sherpaGammaW))    { name = "#gamma + W"; } 
	else if (std::regex_match(ldn, re_match_name, re_sherpaGammaZ))    { name = "#gamma + Z"; }
	else if (std::regex_match(ldn, re_match_name, re_powhegTtbar))     { name = "t#bar{t}"; }
	else if (std::regex_match(ldn, re_match_name, re_ISRgammaSignal1)) {
	  name = "Z' (" + string(re_match_name[2]) + " GeV)";
	  signal = true;
	}
	else if (std::regex_match(ldn, re_match_name, re_ISRgammaSignal2)) {
	  name = "Z' (" + string(re_match_name[2]) + "00 GeV)";
	  signal = true;
	}
	else if (std::regex_match(ldn, re_match_name, re_ISRjetSignal)) {
	  name = "Z' (" + string(re_match_name[2]) + " GeV)";
	  signal = true;
	}

	if (name == "") {
	  DEBUG("Dataset name '%s' was not recognised.", ldn.c_str());
	}
	
	// Set remaining sample info fields, and we're done.
	info.name = name;
	info.type = (signal ? SampleType::Signal : SampleType::Background);
	info.xsec = xsec * eff;
      }
      
      // Store sample info in map.
      m_info[DSID] = info;
      
    } // end: loop lines in file.
     
    // Close sample info file.
    file.close();
    
    DEBUG("Exiting");
    
    return true;
  }
  
  
  template<class HistType>
  void PlottingHelper<HistType>::styleHist_ (HistType* hist, const bool& isMC, const string& name) { 
    
    DEBUG("Calling with: %s (%s)", name.c_str(), (isMC ? "MC" : "data"));

    if (!hist) { return; }
    
    // Base styling.
    hist->SetTitle("");
    
    hist->SetLineWidth(2);
    hist->SetLineColor(kBlack);
    
    hist->GetXaxis()->SetTitleSize(s_fontSizeM);
    hist->GetYaxis()->SetTitleSize(s_fontSizeM);
    
    hist->GetXaxis()->SetTitleOffset(1.3);
    hist->GetYaxis()->SetTitleOffset(1.7);
    
    hist->GetXaxis()->SetLabelSize(s_fontSizeM);
    hist->GetYaxis()->SetLabelSize(s_fontSizeM);
    
    hist->GetXaxis()->SetNdivisions(505);
    hist->GetYaxis()->SetNdivisions(505);
    
    hist->SetFillColor(kRed); // Default.
    
    if (name == "StatsError") {
      hist->SetFillColor(kGray + 3);
      hist->SetFillStyle(3254);
      
      hist->SetMarkerColor(0);
      hist->SetMarkerStyle(0);
      
      hist->SetLineStyle(1);
      hist->SetLineColor(kGray + 3);

      return;
    }

    if (name == "StatsSystError") {
      hist->SetFillColor(0);
      hist->SetFillStyle(0);
      
      hist->SetMarkerColor(0);
      hist->SetMarkerStyle(0);
      
      hist->SetLineStyle(1);
      hist->SetLineColor(kGray + 3);

      return;
    }

    if (name == "Pulls") {
      hist->SetFillColorAlpha(kBlack, 0.3);
      
      hist->SetMarkerColor(0);
      hist->SetMarkerStyle(0);
      
      hist->SetLineStyle(1);
      hist->SetLineWidth(2);
      hist->SetLineColor(kBlack);
      return;
    }
    
    // Set axis titles.
    /**
     * @TODO: z-axis title?
     */
    if (m_axistitles.at(0) != "") {
	hist->GetXaxis()->SetTitle(m_axistitles.at(0).c_str());
    }
    if (m_axistitles.at(1) != "") {
      hist->GetYaxis()->SetTitle((m_axistitles.at(1) + (m_normalised ? " (Normalised)" : "")).c_str());
    }

    if (isMC) {
      
      // Determine whether is signal.
      std::regex re("Z' \\(([0-9])+ GeV\\)");
      std::smatch re_match;
      bool isSignal = (bool) std::regex_match(name, re_match, re);
      
      if (isSignal) {
	
	hist->SetFillStyle(0);
	hist->SetFillColor(0);
	
	hist->SetLineStyle(1);
	hist->SetLineColor((m_signals.size() > 2 ? (m_signals.size() > 5 ? kGreen : kBlue) : kRed) + (m_signals.size() % 3) * 2);
	
      } else {
	
	hist->SetFillStyle(1001);
	
      }
     
      /* Incl. gamma */
      std::regex re_inclGamma("Incl. #gamma.*");
      std::smatch re_match_inclGamma;
      if      (std::regex_match(name, re_match_inclGamma, re_inclGamma)) { 
	//hist->SetFillColor (kAzure + m_backgrounds.size() + 1);
	hist->SetFillColor (kAzure + 7);
      }

      if      (name == "#gamma + W") { hist->SetFillColor(kAzure + 2); }
      else if (name == "#gamma + Z") { hist->SetFillColor(kAzure + 3); }


      if (name == "QCD multijets" || name == "QCD multijets (pythia)") {
	hist->SetFillColor(kOrange - 3);
      }
      if      (name == "Incl. W" || name == "Incl. W (pythia)") { hist->SetFillColor(kOrange + 8); }
      else if (name == "Incl. Z" || name == "Incl. Z (pythia)") { hist->SetFillColor(kOrange + 9); }

      if (name == "t#bar{t}") {
	hist->SetFillColor(kSpring - 2);
      }
      
      /* V (-> ll/lv) + X */
      if      (name == "Wenu")  { hist->SetFillColor(kAzure);      }
      else if (name == "Wmunu") { hist->SetFillColor(kAzure + 1);  }
      else if (name == "Zee")   { hist->SetFillColor(kAzure + 2);  }
      else if (name == "Zmumu") { hist->SetFillColor(kAzure + 3);  }
      /* VV semilept. */
      else if (name == "WqqZll") { hist->SetFillColor(kGreen + 1); }
      else if (name == "ZqqZll") { hist->SetFillColor(kGreen + 2); }
      /* 4 lep. + X */
      else if (name == "llll")          { hist->SetFillColor(kViolet + 1); }
      else if (name == "lllljj_EW6")    { hist->SetFillColor(kViolet + 2); }
      else if (name == "lllv")          { hist->SetFillColor(kViolet + 3); }
      else if (name == "lllvjj_EW6")    { hist->SetFillColor(kViolet + 4); }
      else if (name == "llvv_OS")       { hist->SetFillColor(kViolet + 5); }
      else if (name == "llvvjj_SS_EW4") { hist->SetFillColor(kViolet + 6); }
      else if (name == "llvvjj_SS_EW6") { hist->SetFillColor(kViolet + 7); }
      /* top */
      else if (name == "ttbar_dil") { hist->SetFillColor(kOrange); }
      
    } else {
      
      // Data.
      hist->SetMarkerStyle(8);
      hist->SetMarkerSize (0.8); // 0.5
      hist->SetMarkerColor(kBlack);
      hist->SetLineColor(kBlack);
      
      // Remove horisonal error bars.
      //gStyle->SetErrorX(0.001);
      
    }
    
    return;    
  }

  template<class HistType>
  void PlottingHelper<HistType>::computeSystematics_ () {
    // Loop stored systematics calls
    INFO("Entering.");
    for (auto f : m_systematicCalls) {
      computeSystematic_(f);
    }
    INFO("Exiting");
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::computeSystematic_ (const std::function< void(TH1F* syst, const TH1F* data, const TH1F* background) >& f) {

    INFO("Entering");
    // Initialise (unique) systematic name
    std::string systname = "syst_" + std::to_string(m_systematics.size());

    // Initialise systematic histogram
    std::unique_ptr<HistType> syst;
    if (m_xbins == nullptr) {
      syst = makeUniqueMove(new TH1F(systname.c_str(), "", m_nbinsx, m_xmin, m_xmax));
    } else {
      syst = makeUniqueMove(new TH1F(systname.c_str(), "", m_nbinsx, m_xbins));
    }
    syst.get()->Sumw2();

    // Call function
    f(syst.get(), m_data.get(), m_background.get());

    // Append systematic to list
    m_systematics.push_back(std::move(syst));

    INFO("Exiting");
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::addToSystematicsSum_ (HistType* syst) {
    if (m_systematicsSum) {
      for (unsigned bin = 1; bin <= m_systematicsSum->GetXaxis()->GetNbins(); bin++) {
	float c1 = m_systematicsSum->GetBinError(bin);
	float c2 = syst            ->GetBinError(bin);
	m_systematicsSum->SetBinError(bin, std::sqrt( std::pow(c1, 2) + std::pow(c2, 2) ));
      }
    } else {
      m_systematicsSum = makeUniqueMove( (TH1F*) syst->Clone("syst_sum") );
    }
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::getSystematicsFromCategories_ () {
    
    // Get systematics from pairs of categories.
    DEBUG("Number of systematics category pairs to go through: %d", m_systematicsCategories.size());

    if (m_systematicsCategories.size() == 0) { return; }

    loadHistograms_(m_input);

    // -- Nominal "variation"
    std::map< std::string, std::unique_ptr<HistType> > variations;
    variations["Nominal"] = std::move(getLeadingBackground_()); // < THIS IS SPECIALISED: GENERALLY SHOULDN'T JUST USE LEADING BACKGROUND

    // -- Loop category pairs
    for (const auto& pair : m_systematicsCategories) {
      const std::string up   = pair.first;
      const std::string down = pair.second;
      DEBUG("  Going to retrieve data from categories '%s' and '%s'.", up.c_str(), down.c_str());
      
      for (const std::string& cat : {up, down}) {
	auto fields = split(m_input, '/');
	std::string new_input = replaceAll(m_input, fields[1], cat);
	DEBUG("    Substituting '%s' -> '%s'", m_input.c_str(), new_input.c_str());

	loadHistograms_(new_input);
	variations[cat] = std::move(getLeadingBackground_());
      }
      
      // Get systematics band
      DEBUG("  Creating systematics histogram");
      HistType* syst = (HistType*) variations["Nominal"]->Clone(("syst_" + up + "_" + down).c_str());
      for (unsigned bin = 1; bin <= variations["Nominal"]->GetXaxis()->GetNbins(); bin++) {
	float s_up   = std::fabs(variations[up]  ->GetBinContent(bin) - variations["Nominal"]->GetBinContent(bin));
	float s_down = std::fabs(variations[down]->GetBinContent(bin) - variations["Nominal"]->GetBinContent(bin));
	float stat   = 0.; //variations["Nominal"]->GetBinError(bin);
	DEBUG("  %d : %4.1f / %4.1f / %4.1f", bin, s_up, s_down, stat);
	syst->SetBinContent(bin, 1);
	syst->SetBinError  (bin, std::max(std::max(s_up, s_down) - stat, float(0))); // < THIS IS SPECIALISED: GENERALLY SHOULDN'T SUBTRACT STATS. UNCERTAINTY
      }
      DEBUG("    Adding to systematics sum.");
      addToSystematicsSum_(syst);
      DEBUG("    Done");
      delete syst; syst = nullptr;
    }
    
    // Restoring stuff
    DEBUG("Restoring stuff");
    loadHistograms_(m_input);
    
    DEBUG("Done");
    return;
  }

  template<class HistType>
  std::unique_ptr<HistType> PlottingHelper<HistType>::getSumOfBackgrounds_ () {

    // Check(s)
    if (m_backgrounds.size() == 0) {
      WARNING("No background were found.");
      return nullptr;
    }

    // Loop backgrounds
    std::unique_ptr<HistType> output(nullptr);
    for (const auto& p : m_backgrounds) {

      // Check whether sum-histogram already exists.
      if (output) {
        // If so, add contents of histogram to existing background.
        output->Add(p.second.get());
      } else {
        // Otherwise move first histogram to background.
        output = makeUniqueMove( (HistType*) p.second->Clone("sumOfBackgrounds") );
      }

    }
    return std::move(output);
  }
  

  template<class HistType>
  std::unique_ptr<HistType> PlottingHelper<HistType>::getLeadingBackground_ () {

    // Check(s)
    if (m_backgrounds.size() == 0) {
      WARNING("No background were found.");
      return nullptr;
    }

    // Loop backgrounds
    std::unique_ptr<HistType> output(nullptr);
    for (const auto& p : m_backgrounds) {

      // Check whether current background is largest
      if (output == nullptr or p.second->Integral() > output->Integral()) {
        output = makeUniqueMove( (HistType*) p.second->Clone("leadingBackgrounds") );
      }

    }

    return std::move(output);
  }


  template<class HistType>
  void PlottingHelper<HistType>::drawArrows_ (const float& plotmin, const float& plotmax) {

    // Check(s)
    if (m_arrowsDown.size() + m_arrowsRight.size() + m_arrowsLeft.size() == 0) { return; }
    
    // Definitions
    float ymin = (m_log ? plotmin : 0 );
    float ymax = (m_log ? exp(m_padding * (log(plotmax) - log(plotmin)) + log(plotmin)) : m_padding * plotmax );
    float xmin = gPad->GetUxmin();
    float xmax = gPad->GetUxmax();
    
    // Initialise and style TLine objects
    TLine whiteLine, blackLine;
    whiteLine.SetLineWidth(2);
    blackLine.SetLineWidth(4);
    whiteLine.SetLineColor(kWhite);
    blackLine.SetLineColor(kBlack);
    
    // Compute absolute arrow coordinates
    float offset_bottom = (m_ratio ? 0.04 : 0.14); // relative
    float arrow_head        = 0.01; // relative
    float arrow_base_length = 0.10; // relative
    float arrow_head_lengthx = arrow_head * (xmax - xmin);
    float arrow_head_lengthy, arrow_head_lengthydown;
    float arrow_ymin, arrow_ymax;
    if (m_log) {
      float logdy   = std::log(ymax) - std::log(ymin);
      float logymin = std::log(ymin);
      arrow_head_lengthy = std::exp((offset_bottom + arrow_head) * logdy + logymin) - std::exp(offset_bottom * logdy + logymin);
      arrow_head_lengthydown = std::exp((offset_bottom) * logdy + logymin) - std::exp((offset_bottom - arrow_head) * logdy + logymin);
      arrow_ymin = std::exp( offset_bottom * logdy + logymin);
      arrow_ymax = std::exp((offset_bottom + arrow_base_length) * logdy + logymin);
    } else {
      arrow_head_lengthy = arrow_head * (ymax - ymin);
      arrow_ymin = offset_bottom * (ymax - ymin) + ymin;
      arrow_ymax = arrow_ymin + arrow_base_length * (ymax - ymin);
    }
    
    // Actually draw the arrows.
    for (TLine* line : { &blackLine, &whiteLine }) {
      float x, y1, y2, dx, dy;
      
      dx = arrow_head_lengthx;
      y1 = arrow_ymin;
      y2 = arrow_ymax;
      dy = arrow_head_lengthy;
      
      // -- Down
      for (const float& x : m_arrowsDown) {
	line->DrawLine(x, y1, x, y2);
	line->DrawLine(x, y1, x + dx, y1 + dy);
	line->DrawLine(x, y1, x - dx, y1 + dy);
      }
      
      // -- Right
      float dydown = arrow_head_lengthydown;
      for (const float& x : m_arrowsRight) {
	line->DrawLine(x, y1, x, y2);
	line->DrawLine(x, y1, x + 2 * dx, y1);
	line->DrawLine(x + 2 * dx, y1, x + 1 * dx, y1 + dy);
	line->DrawLine(x + 2 * dx, y1, x + 1 * dx, y1 - dydown);
      }
      
      // -- Left
      for (const float& x : m_arrowsLeft) {
	line->DrawLine(x, y1, x, y2);
	line->DrawLine(x, y1, x - 2 * dx, y1);
	line->DrawLine(x - 2 * dx, y1, x - 1 * dx, y1 + dy);
	line->DrawLine(x - 2 * dx, y1, x - 1 * dx, y1 - dydown);
      }
      
    }
    
    return;
  }

  template<class HistType>
  void PlottingHelper<HistType>::drawRegions_ (const float& plotmin, const float& plotmax) {

    // Check(s)
    if (m_regions.size() == 0) { return; }
    
    // Definitions
    float ymin = (m_log ? plotmin : 0 );
    float ymax = plotmax;
    float xmin = gPad->GetUxmin();
    float xmax = gPad->GetUxmax();
    float dy = 0.05; // Relative offset below region title
    dy = (m_log ? std::exp(dy * (std::log(ymax) - std::log(ymin)) + std::log(ymin)) - ymin : dy * (ymax - ymin));
    
    // Initialise and style TLine object
    TLine line;
    line.SetLineWidth(2);
    line.SetLineStyle(2);
    line.SetLineColor(kGray + 3);

    // Initialise and style TLatex object
    TLatex text;
    text.SetTextAlign(21);
    text.SetTextSize(s_fontSizeM);
    text.SetTextFont(42);
    
    // Get pointer to x-axis.
    TAxis* axis = m_background->GetXaxis();

    // Draw the regions
    std::vector<float> coords;
    Range range;
    std::string title;
    bool snap, light;
    for (const auto& tuple : m_regions) {

      // Unpack region range, title, and "snap"
      std::tie(range, title, snap, light) = tuple;

      if (light) { text.SetTextColor(kWhite); }
      else       { text.SetTextColor(kBlack); }

      float x_down = std::max(range.down(), xmin);
      float x_up   = std::min(range.up(),   xmax);
      if (snap) {
	x_down = snapToAxis(x_down, axis, SnapDirection::Nearest);
	x_up   = snapToAxis(x_up,   axis, SnapDirection::Nearest);
      }

      // Draw lines delimiting regions
      for (float x : {range.down(), range.up()}) {
	if (snap) { x = snapToAxis(x, axis, SnapDirection::Nearest); }
	if (contains(coords, x))    { continue; }
	if (x <= xmin || x >= xmax) { continue; }
	line.DrawLine(x, ymin, x, ymax);
	coords.push_back(x);
      }
            
      // Draw region title
      text.DrawLatex(0.5 * (x_down + x_up), ymin + dy, title.c_str());
      
    }
    
    return;
  }

  /// Explicitly instatiate templates.
  template class PlottingHelper<TH1F>;
  
}
