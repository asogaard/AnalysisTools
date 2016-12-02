#include "AnalysisTools/PlottingHelper.h"

namespace AnalysisTools {

  /// Utility function for creating and moving unique pointer.
  template<typename T>
  std::unique_ptr<T> makeUniqueMove (T* p) {
    return std::move(std::unique_ptr<T>(p));
  }


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
      m_H = 600;
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
      cout << "<PlottingHelper<HistType>::computeImprovement> The provided number (" << improvementDirection << ") is larger than 2." << endl;
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
  void PlottingHelper<HistType>::setPadding (const double& padding) {
    m_padding = padding;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::setNormalised (const bool& normalised) {
    m_normalised = normalised;
    return;
  }
  
  
  /// Get method(s). 
  // ---------------------------------------------------------------------------

  // ...
  
  
  /// High-level management method(s).
  // ---------------------------------------------------------------------------

  template<class HistType>
  void PlottingHelper<HistType>::draw () {

    cout << "<PlottingHelper<HistType>::draw> Entering." << endl;

    // General style settings/variables.      
    gStyle->SetOptStat(0);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLegendFillColor(0);
    
    const unsigned kMyBlue  = 1001;
    const unsigned kMyRed   = 1002;
    const unsigned kMyGreen = kGreen + 2;
    const unsigned kMyLightGreen = kGreen - 10;
    
    TColor myBlue(kMyBlue,   0./255.,  30./255.,  59./255.);
    TColor myRed (kMyRed,  205./255.,   0./255.,  55./255.);
    
    const unsigned kImprovementColor = kOrange + 7; // kViolet + 2

    // Make sure that an output file exists, such that we can write to it.
    assert( m_outfile );
    
    // Load sample information from file.
    loadSampleInfo_();

    // Load histograms.
    loadHistograms_();

    // Setup canvas and pads as appropriate.
    setupCanvas_();
        
    // Normalising.
    if (m_normalised) {
      cout << "<PlottingHelper::draw> Normalising." << endl;

      // Initialise integral variable.
      float integral;
      
      // Data.
      integral = m_data->Integral();
      if (integral > 0) { m_data->Scale(1/integral); }
      
      // Background(s).
      integral = m_sum->Integral();
      if (integral > 0) {
	m_sum->Scale(1/integral);
	for (const auto& p : m_backgrounds) { p.second->Scale(1/integral); }
      }
      
      // Signal.
      for (const auto& p : m_signals) {
	integral = p.second->Integral();
	if (integral > 0) { p.second->Scale(1/integral); }
      }
    }
    
    // Sort backgrounds by integral.
    m_backgroundsSorted.clear();
    for (const auto& p : m_backgrounds) {
      //m_backgroundsSorted.push_back( pair<string, upHistType>(p.first, std::move(p.second)) );
      m_backgroundsSorted.push_back(std::pair<std::string, HistType*>(p.first, p.second.get()));
    }
    std::sort(m_backgroundsSorted.begin(), m_backgroundsSorted.end(),
	      [](const pair<string, HistType*> & p1, const pair<string, HistType*> & p2) -> bool
	      {
		return p1.second->Integral() < p2.second->Integral();
	      });
    
    std::unique_ptr<THStack> background (new THStack("StackedBackgrounds", ""));
    for (const auto& p : m_backgroundsSorted) {
      background->Add(p.second); // .get());
    }
    
    // Drawing (main pad).
    cout << "<PlottingHelper::draw> Going to first pad." << endl;
    m_pads.first->cd();
    
    background->Draw("HIST");
    gPad->Update();
    
    /* Get plot maximum. */
    
    double maxData = 0., maxMC = 0.;
    for (unsigned ibin = 0; ibin < m_data->GetXaxis()->GetNbins(); ibin++) {
      maxData = max(m_data->GetBinContent(ibin + 1), maxData);
      maxMC   = max(m_sum ->GetBinContent(ibin + 1), maxMC);
    }
    
    const double plotmax = max(maxData, maxMC); //max(m_data->GetMaximum(), m_sum->GetMaximum());
    const double plotmin = (m_normalised ? 1e-05 : 0.5); // log-plots only
    //const double m_padding = 2.3; /* @TODO: Make dependent on the number of text lines. */
    
    if (m_log) {
      //m_sum->SetMaximum(exp(m_padding * (log(plotmax) - log(plotmin)) + log(plotmin)));
      //m_sum->SetMinimum(plotmin);
      m_sum->GetYaxis()->SetRangeUser(plotmin, exp(m_padding * (log(plotmax) - log(plotmin)) + log(plotmin)));
    } else {
      m_sum->SetMaximum(m_padding * plotmax);
      m_sum->GetYaxis()->SetRangeUser(0, m_padding * plotmax);
    }
    
    
    // -- Background
    cout << "<PlottingHelper::draw> Drawing background." << endl;
    m_sum->DrawCopy("AXIS");
    background->Draw("HIST same");
    m_sum->DrawCopy("E2 same");
    
    // -- Signal
    cout << "<PlottingHelper::draw> Drawing signal." << endl;
    for (const auto& p : m_signals) {
      cout << "<PlottingHelper::draw>  - " << p.first << endl;
      p.second->DrawCopy("HIST same");
    }
    
    // -- Data
    cout << "<PlottingHelper::draw> Drawing data." << endl;
    if (m_data) {
      m_data->DrawCopy("PE same");
    }
    m_sum->DrawCopy("AXIS same");
    
    
    // Ratio distributions.
    if (m_data) {
      m_ratiohists["Ratio"] = makeUniqueMove((HistType*) m_data->Clone("DataMC_Ratio"));
      m_ratiohists["Ratio"]->SetDirectory(0);
      m_ratiohists["Ratio"]->Divide(m_sum.get());
      
      m_ratiohists["StatsError"] = makeUniqueMove((HistType*) m_ratiohists["Ratio"]->Clone("DataMC_StatsError"));
      m_ratiohists["StatsError"]->SetDirectory(0);
      for (unsigned i = 0; i < m_ratiohists["StatsError"]->GetXaxis()->GetNbins(); i++) {
	m_ratiohists["StatsError"]->SetBinContent(i + 1, 1);
	double c = m_sum->GetBinContent(i + 1);
	double e = m_sum->GetBinError(i + 1);
	m_ratiohists["StatsError"]->SetBinError  (i + 1, (c > 0 ? e/c : 0));
      }
      m_ratiohists["StatsError"]->GetYaxis()->SetTitle("Data / MC");
      
      // Styling.
      styleHist_(m_ratiohists["Ratio"].get(),      false, "Ratio");
      styleHist_(m_ratiohists["StatsError"].get(), true,  "StatsError");
      
      m_ratiohists["StatsError"]->GetXaxis()->SetTitleSize(s_fontSizeM * 0.75/0.25);
      m_ratiohists["StatsError"]->GetYaxis()->SetTitleSize(s_fontSizeM * 0.75/0.25);
      m_ratiohists["StatsError"]->GetXaxis()->SetLabelSize(s_fontSizeM * 0.75/0.25);
      m_ratiohists["StatsError"]->GetYaxis()->SetLabelSize(s_fontSizeM * 0.75/0.25);
      m_ratiohists["StatsError"]->GetXaxis()->SetNdivisions(505);
      m_ratiohists["StatsError"]->GetYaxis()->SetNdivisions(503);
      
      m_ratiohists["StatsError"]->GetXaxis()->SetTitleOffset(1.3);
      m_ratiohists["StatsError"]->GetYaxis()->SetTitleOffset(1.5 / (0.75/0.25));
      
      m_ratiohists["StatsError"]->GetXaxis()->SetTickLength(0.03 * 0.75/0.25);
      
      //m_ratiohists["StatsError"]->GetYaxis()->SetRangeUser(0.75, 1.25);
      m_ratiohists["StatsError"]->GetYaxis()->SetRangeUser(0.0, 2.0);
      
      m_ratiohists["StatsError"]->GetXaxis()->SetTitle(m_axistitles.at(0).c_str());
      m_ratiohists["StatsError"]->GetXaxis()->SetLabelOffset(0.02);
      
    }
    
    if (m_ratio) {
      cout << "<PlottingHelper::draw> Drawing ratio pads." << endl;
      m_pads.second->cd();
      
      m_ratiohists["StatsError"]->Draw("E2");
      m_ratiohists["Ratio"]->Draw("PE0L same");
    }
    
    
    // Borders and axis ticks.
    cout << "<PlottingHelper::draw> Setting borders and axis ticks." << endl;
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
    cout << "<PlottingHelper::draw> Drawing text." << endl;
    m_pads.first->cd();
    
    TLatex latex;
    latex.SetTextSize(s_fontSizeL);
    latex.SetTextFont(72);
    
    string text;
    
    double xDraw = 0.21;
    double yDraw = 0.855;
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
    
    
    // Legend.
    cout << "<PlottingHelper::draw> Drawing legends." << endl;
    unsigned N = (m_data ? 1 : 0) + m_backgrounds.size() + m_signals.size() + 1;
    
    xDraw = 0.63; // 0.65;
    yDraw = 0.95; // 0.9;
    
    double xmin = xDraw - yStep * 0.25 - (m_improvementDirection != -1 ? 0.05 : 0);
    double ymax = yDraw - yStep * 1;
    
    double width  = (m_improvementDirection != -1 ? 0.85 : 0.90) - xmin;
    double height = N * s_fontSizeS * 1.2;
    
    TLegend* legend = new TLegend(xmin, ymax - height, xmin + width, ymax);
    legend->SetTextSize(s_fontSizeS);
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
      legend->AddEntry(it->second, it->first.c_str(), "F");
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
    
    legend->Draw("same");
    
    
    // Lines.
    cout << "<PlottingHelper::draw> Drawing lines." << endl;
    TLine line;
    if (m_ratio) {
      m_pads.second->cd();
      double axismin = m_ratiohists["StatsError"]->GetXaxis()->GetXmin();
      double axismax = m_ratiohists["StatsError"]->GetXaxis()->GetXmax();
      
      line.SetLineColor(kGray + 3);
      line.DrawLine(axismin, 1, axismax, 1);
      
      line.SetLineStyle(2);
      line.DrawLine(axismin, 1.1, axismax, 1.1);
      line.DrawLine(axismin, 0.9, axismax, 0.9);
    }
    
    // Cut improvements.
    if (m_improvementDirection != -1 && m_signals.size() > 0) {
      cout << "<PlottingHelper::draw> Drawing cut improvements." << endl;
      
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
	  cout << "<PlottingHelper::draw> -- Baseline sensitivity: " << y[i] << endl;
	}
      }
      
      cout << "<PlottingHelper::draw> -- Maximal sensitivity: " << yMaxImpr << "(" << xMaxImpr << ")" << endl;
      
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
    cout << "<PlottingHelper::draw> Writing canvas." << endl;
    m_canvas->Write();
    string savename = replaceAll(m_input + "/" + m_branch + (m_improvementDirection != -1 ? "_improvement" : ""), "/", "__");
    m_canvas->SaveAs((m_outdir + savename + ".pdf").c_str());
    /*
      if (m_branch != "") {
      m_canvas->SaveAs((m_outdir + m_branch + ".pdf").c_str());
      } else {
      m_canvas->SaveAs((m_outdir + "TESTplot.pdf").c_str());
      }
    */
    cout << "<PlottingHelper::draw> Exiting." << endl;
    return;
  }
  
  template<class HistType>
  void PlottingHelper<HistType>::save (const string& path) {
    /* ... */
    return;
  }
  
  
  /// Low-level management method(s).
  // ---------------------------------------------------------------------------

  template<class HistType>
  void PlottingHelper<HistType>::closePads_ () {
    /*
      if (m_canvas) {
      delete m_canvas;
      m_canvas = nullptr;
      }
      if (m_pads.first) {
      delete m_pads.first;
      m_pads.first = nullptr;
      }
      if (m_pads.second) {
      delete m_pads.second;
      m_pads.second = nullptr;
      }
    */
    return;
  }


  template<class HistType>
  bool PlottingHelper<HistType>::setupCanvas_ () {

    // Make sure the the output file exists.
    assert(m_outfile);

    // Move to the output file.
    m_outfile->cd();

    // Create the canvas.
    std::string canvasName = (m_input + (m_branch != "" ? ":" : "") + m_branch);
    m_canvas = makeUniqueMove(new TCanvas(canvasName.c_str(), "", m_W, m_H));

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

    // Try to the object specified by the input name.
    cout << "Getting '" << path << "'." << endl;
    TObject* obj = (TObject*) file->Get(path.c_str());
    cout << "  pointer: " << obj << endl;
      
    if (obj == nullptr) {
      cout << "<PlottingHelper::getHistogram_> Warning: Requested input '" << m_input << "' could not be retrieved from file '" << file->GetName() << "'." << endl;
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
      
      // Reading tree.
      /* @TODO: Distinguish between the number of ':' in the input name, to allow for TH2/TProfile as well. */
      unsigned it = 0; // @TODO: Fix counter.
      string histname = m_input + ":" + m_branch + "_autohist_" + to_string(it++);
      
      // Move to output file.
      m_outfile->cd();

      // Create the histogram using the member axis variables.
      hist = makeUniqueMove(new TH1F(histname.c_str(), "", m_nbinsx, m_xmin, m_xmax));
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
	hist->Fill(value, weight);
	/**
	 * @TODO: Depending on HistType, call 'Fill()' with more argument (e.g. for HistType == TProfile).
	 */
      }
      
    } else {
      
      cout << "<PlottingHelper::getHistogram_> Warning: Requested input '" << m_input << "' could not be cast as either <class HistType> or TTree*." << endl;
      return nullptr;
      
    }
    
    return hist; // std::move(hist);
  }
  
  
  template<class HistType>
  void PlottingHelper<HistType>::loadHistograms_ () {
    
    cout << "<PlottingHelper::loadHistograms_> Entering." << endl;
    
    // Check whether any file names were provided.
    if (m_filenames.size() == 0) {
      cout << "<PlottingHelper::loadHistograms_> WARNING: No input files were provided." << endl;
      return;
    }
    
    // Initialise luminosity counter.
    m_lumi = 0.;
    
    // Looping input files.
    // -------------------------------------------------------------------------
   
    cout << "<PlottingHelper::loadHistograms_>   Looping " << m_filenames.size() << " input files." << endl;
    unsigned it = 0;
    for (const string& filename : m_filenames) {
   
      // Make sure that the current file exists.
      if (!fileExists(filename)) {
	cout << "<PlottingHelper::loadHistograms_> Warning: Input file '" << filename << "' doesn't exist." << endl;
	continue;
      }
      
      // Open the current file.
      TFile file (filename.c_str(), "READ");

      std::unique_ptr<HistType> hist (getHistogram_ (&file, m_input));

      /*
      // Try to the object specified by the input name.
      TObject* obj = (TObject*) file.Get(m_input.c_str());
      
      if (obj == nullptr) {
	cout << "<PlottingHelper::loadHistograms_> Warning: Requested input '" << m_input << "' could not be retrieved from file '" << filename << "'." << endl;
	continue;
      }
      
      double sumOfWeights = 1.;
      //sumOfWeights = ((TH1F*) file->Get("h_rawWeight"))->Integral();
      
      
      // Get histogram.
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      
      TH1F* hist;
      
      if ((hist = dynamic_cast<TH1F*>(obj))) {
	
	/ * Reading histogram. * /
	hist->SetDirectory(0);     // < Keep in memory after file is closed!
	TH1::AddDirectory(kFALSE); // < Keep in memory after file is closed!
	
	if (m_rebin > 1) {
	  hist->Rebin(m_rebin);
	}
	
      } else if (TTree* tree = dynamic_cast<TTree*>(obj)) {
	
	/ * Reading tree. * /
	/ * @TODO: Distinguish between the number of ':' in the input name, to allow for TH2/TProfile as well. * /
	string histname = m_input + ":" + m_branch + "_autohist_" + to_string(it++);
	
	m_outfile->cd();
	hist = new TH1F(histname.c_str(), "", m_nbinsx, m_xmin, m_xmax);
	hist->GetXaxis()->Set(m_nbinsx, m_xmin, m_xmax);
	hist->Sumw2(); // ?
	
	double var, weight = 1.;
	tree->SetBranchAddress(m_branch.c_str(), &var);
	tree->SetBranchAddress("weight",         &weight);
	
	unsigned nEntries = tree->GetEntries();
	for (unsigned iEntry = 0; iEntry < nEntries; iEntry++) {
	  tree->GetEntry(iEntry);
	  hist->Fill(var, weight);
	}
	
      } else {
	
	cout << "<PlottingHelper::loadHistograms_> Warning: Requested input '" << m_input << "' could not be cast as either TH1F* or TTree*." << endl;
	continue;
	
      }
      

    */
      
      
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
	cout << "<PlottingHelper::loadHistograms_> Warning: DSID '" << DSID << "' was not found in m_info." << endl;
	continue;
      }

      // Get sample info.
      SampleInfo info = m_info.at(DSID);
      
      bool isMC     = (info.type != SampleType::Data);
      bool isSignal = (info.type == SampleType::Signal);
      
      // Keep track of the actual amount of luminosity added.
      if (!isMC) {
	m_lumi += info.lumi / 1000.; // Scaling pb-1 -> fb-1.
      }
            
      // Set axis titles.
      if (m_axistitles.at(0) != "" && !m_ratio) {
	hist->GetXaxis()->SetTitle(m_axistitles.at(0).c_str());
      } else {
	hist->GetXaxis()->SetLabelOffset(9999.9);
      }
      if (m_axistitles.at(1) != "") {
	hist->GetYaxis()->SetTitle((m_axistitles.at(1) + (m_normalised ? " (Normalised)" : "")).c_str());
      }
      /* @TODO: z-axis? */
      
      // Perform styling.
      styleHist_(hist.get(), isMC, info.name);
      
      
      // Add histogram to appropriate collection.
      if (isMC) {
	// Scaling MC samples by cross section.
	hist->Scale(info.xsec / float(info.evts));

	if (isSignal) {
	  // Scaling signal MC samples by optional scaling factor.
	  hist->Scale(m_scaleSignal);

	  // Make sure that each signal is unique.
	  assert( m_signals.count(info.name) == 0 );

	  // Move histogram to signal collection.
	  m_signals[info.name] = std::move(hist); //(hist->Clone(("autohist_Signal_" + info.name).c_str()));
	  //m_signals[info.name]->SetDirectory(0);

	} else {

	  // Check whether the sum background MC distribution exists.
	  if (m_sum) {
	    // If so contents of histogram to existing sum.
	    m_sum->Add(hist.get());

	  } else {
	    // Otherwise move first histogram to sum.
	    m_sum = makeUniqueMove( (TH1F*) hist->Clone("sum") );
	    m_sum->SetDirectory(0);
	    m_sum->Sumw2();
	  }

	  // Check whether this is the first sample of a given type.
	  if (m_backgrounds.count(info.name) == 0) {

	    // If so, move first histogram to background collection.
	    m_backgrounds[info.name] = std::move(hist); //(TH1F*) hist->Clone(("autohist_Background_" + info.name).c_str()); //.insert( pair<unsigned, TH1F*>(DSID, hist) );
	    //m_backgrounds[info.name]->SetDirectory(0);
	    // Explicitly copy bin labels
	    //for (unsigned ibin = 0; ibin < hist->GetXaxis()->GetNbins(); ibin++) {
	    //m_backgrounds[info.name]->GetXaxis()->SetBinLabel(ibin + 1, hist->GetXaxis()->GetBinLabel(ibin + 1));
	    //}
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
	  m_data = std::move(hist); // (TH1F*) hist->Clone("autohist_Data");
	  //m_data->SetDirectory(0);
	  //m_data->Sumw2();
	}
      }
            
    } // end: loop file names.
    
    // If no data is present, scale MC to 1 fb-1.
    if (m_lumi == 0.) {
      m_lumi = 1.;
    }
    
    // Scale MC distributions to amount of luminosity added.
    for (const auto& pair : m_signals) {
      pair.second->Scale(m_lumi);
    }
    for (const auto& pair : m_backgrounds) {
      pair.second->Scale(m_lumi);
    }
    if (m_sum) {
      m_sum->Scale(m_lumi);
    }
    
    // If no data is present, make a dummy histogram. (Why?)
    if (!m_data) { 
      m_data = makeUniqueMove((HistType*) m_sum->Clone("dummy_data"));
      m_data->SetDirectory(0);
      m_data->Scale(0.);
    }

    // Perform styling.
    styleHist_(m_sum.get(),  true,  "StatsError");
    styleHist_(m_data.get(), false, "Data");
    
    m_outfile->cd();
    
    cout << "<PlottingHelper::loadHistograms_> Exiting." << endl;
    
    return;
  }
  
  
  template<class HistType>
  void PlottingHelper<HistType>::loadSampleInfo_ () {

    cout << "<PlottingHelper::loadSampleInfo_> Entering." << endl;

    // Clear sample info container.
    m_info.clear();
    
    // Make sure that the sample info file is specified and exists.
    if (m_sampleinfofile == "") {
      cout << "<PlottingHelper::loadSampleInfo_> WARNING: Please specify a sample info file." << endl;
      return;
    }
    if (!fileExists(m_sampleinfofile)) {
      cout << "<PlottingHelper::loadSampleInfo_> ERROR: Sample info file '"<< m_sampleinfofile <<"' was not found. Exiting." << endl;
      return;
    }
    
    // Open sample info file.
    ifstream file ( m_sampleinfofile.c_str() );
    if (!file.is_open()) {
      cout << "<PlottingHelper::loadSampleInfo_> ERROR: Cross section file could not be opened. Exiting." << endl;
      return;
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
      assert (m_info.count(DSID) == 0);
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
	  std::cout << "<PlottingHelper::loadSampleInfo_> Warning: DSID '" << DSID << "' has 0 events. Skipping." << std::endl;
	  continue; 
	}
	info.evts = evts;
	
	// Get generator filter efficiency.
	double eff = stod(fields.at(3));
	
	// Get physics name.
	string name = (string) fields.at(4);
	name = trim(name);
	
	// Determine MC type from physics name.
	bool signal = false;
	std::regex re_singlePhoton1("Sherpa_CT10_SinglePhotonPt([0-9]+).*");
	std::regex re_singlePhoton2("Sherpa_CT10_SinglePhotonPt([0-9]+)_([0-9]+).*");
	std::regex re_signal(".*dmA_jja_Ph([0-9]+)_mRp0([0-9]+).*");
	std::smatch re_match_name;
	if        (std::regex_match(name, re_match_name, re_singlePhoton2)) {
	  name = "Incl. #gamma";
	  // p_{T} #in [" + string(re_match_name[1]) + ", " + string(re_match_name[2]) + "] GeV";
	} else if (std::regex_match(name, re_match_name, re_singlePhoton1)) {
	  name = "Incl. #gamma";
	  // p_{T} > " + string(re_match_name[1]) + " GeV";
	} else if (std::regex_match(name, re_match_name, re_signal)) {
	  name = "Z' (" + string(re_match_name[2]) + " GeV)";
	  signal = true;
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
    
    cout << "<PlottingHelper::loadSampleInfo_> Exiting." << endl;
    
    return;
  }
  
  
  template<class HistType>
  void PlottingHelper<HistType>::styleHist_ (HistType* hist, const bool& isMC, const string& name) { // const unsigned& DSID) {
    
    cout << "<PlottingHelper::styleHist_> Calling with: '" << name << "' (" << (isMC ? "MC" : "data") << ")." << endl;
    
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
    
    
    if (name == "StatsError") {
      hist->SetFillColor(kGray + 1);
      hist->SetFillStyle(3254);
      
      hist->SetMarkerColor(0);
      hist->SetMarkerStyle(0);
      
      hist->SetLineStyle(1);
      hist->SetLineColor(kGray + 2);
      return;
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
      
      hist->SetFillColor(kRed); // TEMP
      
      /* Incl. gamma */
      std::regex re_inclGamma("Incl. #gamma.*");
      std::smatch re_match_inclGamma;
      if      (std::regex_match(name, re_match_inclGamma, re_inclGamma)) { 
	hist->SetFillColor (kAzure + m_backgrounds.size() + 1); // 1); 
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
      
      hist->SetMarkerStyle(8);
      hist->SetMarkerSize (0.8); // 0.5
      hist->SetMarkerColor(kBlack);
      hist->SetLineColor(kBlack);
      
      // Remove horisonal error bars.
      gStyle->SetErrorX(0.001);
      
    }
    
    return;
    
  }

  /// Explicitly instatiate templates.
  template class PlottingHelper<TH1F>;
  
}
