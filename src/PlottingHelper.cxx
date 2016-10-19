#include "AnalysisTools/PlottingHelper.h"

namespace AnalysisTools {

    // Set method(s).
    void PlottingHelper::setInputName (const string& input) {
        size_t pos = input.find(":");
        if (pos != string::npos) {
            m_input  = input.substr(0, pos);
            m_branch = input.substr(pos + 1);
        } else {
            m_input = input;
        }
        return;
    }
    
    void PlottingHelper::setFilenames (const vector<string>& filenames) {
        m_filenames = filenames;
        return;
    }

    void PlottingHelper::setOutfile  (TFile* outfile) {
        m_outfile = outfile;
	string outfilename (m_outfile->GetName());
	unsigned pos = outfilename.rfind("/");
	if (pos != string::npos) {
	  m_outdir = outfilename.substr(0, pos + 1);
	}
        return;
    }

    void PlottingHelper::setXsecFile (const string& xsecfile) {
        m_xsecfile = xsecfile;
        return;
    }
    
    void PlottingHelper::setSettingsFile (const string& settingsfile) {
        m_settingsfile = settingsfile;
        return;
    }

    void PlottingHelper::setLuminosity (const double& lumi) {
        m_lumi = lumi;
        return;
    }

    void PlottingHelper::setPrintLines (const vector<string>& printlines) {
        m_printlines = printlines;
        return;
    }
    
    void PlottingHelper::setXaxisTitle (const string& title) {
        m_axistitles.at(0) = title;
        return;
    }

    void PlottingHelper::setYaxisTitle (const string& title) {
        m_axistitles.at(1) = title;
        return;
    }

    void PlottingHelper::setZaxisTitle (const string& title) {
        m_axistitles.at(2) = title;
        return;
    }

    void PlottingHelper::setAxisTitles (const string& xtitle, const string& ytitle) {
        setXaxisTitle(xtitle);
        setYaxisTitle(ytitle);
        return;
    }
    
    void PlottingHelper::setAxisTitles (const string& xtitle, const string& ytitle, const string& ztitle) {
        setXaxisTitle(xtitle);
        setYaxisTitle(ytitle);
        setZaxisTitle(ztitle);
        return;
    }
    
    void PlottingHelper::setAxis (const unsigned& nbins, const double& xmin, const double& xmax) {
        setXaxis(nbins, xmin, xmax);
        return;
    }

    void PlottingHelper::setAxis (const vector<double>& bins) {
        setXaxis(bins);
        return;
    }
    
    void PlottingHelper::setXaxis (const unsigned& nbinsx, const double& xmin, const double& xmax) {
        m_nbinsx = nbinsx;
        m_xmin   = xmin;
        m_xmax   = xmax;
        return;
    }

    void PlottingHelper::setYaxis (const unsigned& nbinsy, const double& ymin, const double& ymax) {
        m_nbinsy = nbinsy;
        m_ymin   = ymin;
        m_ymax   = ymax;
        return;
    }
    
    void PlottingHelper::setXaxis (const vector<double>& bins) {
        /* ... */
        return;
    }

    void PlottingHelper::setYaxis (const vector<double>& bins) {
        /* ... */
        return;
    }

    void PlottingHelper::drawRatioPad (const bool& ratio) {
        m_ratio = ratio;
	if (!m_ratio) {
	    m_H = 600;
	}
	return;
    }
  
    void PlottingHelper::rebin (const unsigned& rebin) {
        m_rebin = rebin;
	return;
    }
  
    void PlottingHelper::computeImprovement (const int& improvementDirection) {
        if (improvementDirection > 2) {
	    cout << "<PlottingHelper::computeImprovement> The provided number (" << improvementDirection << ") is larger than 2." << endl;
	    return;
	}
	m_improvementDirection = improvementDirection;
	return;
    }
  
    void PlottingHelper::printBinContents (const bool& print) {
        m_print = print;
	return;
    }
  
    void PlottingHelper::setNormalised (const bool& normalised) {
        m_normalised = normalised;
	return;
    }

    
    // Get method(s).
    // ...
    
    
    // High-level management method(s).
    void PlottingHelper::draw () {
        cout << "<PlottingHelper::draw> Entering." << endl;
        closePads();
        assert( m_outfile );
        
        // General style settings/variables.      
        gStyle->SetOptStat(0);
        gStyle->SetLegendBorderSize(0);
        gStyle->SetLegendFillColor(0);
        
        const unsigned int kMyBlue  = 1001;
        const unsigned int kMyRed   = 1002;
        const unsigned int kMyGreen = kGreen + 2;
        const unsigned int kMyLightGreen = kGreen - 10;

        TColor* myBlue = new TColor(kMyBlue,   0./255.,  30./255.,  59./255.);
        TColor* myRed  = new TColor(kMyRed,  205./255.,   0./255.,  55./255.);

	const unsigned int kImprovementColor = kOrange + 7; // kViolet + 2
        
        // Canvases.
        m_outfile->cd();
        TCanvas* m_canvas = new TCanvas((m_input + (m_branch != "" ? ":" : "") + m_branch).c_str(), "", m_W, m_H);

        m_canvas->cd();
        if (m_ratio) {
            m_pads.first  = new TPad("pad1", "", 0.0, 0.25, 1.0, 1.0);
            m_pads.second = new TPad("pad2", "", 0.0, 0.0,  1.0, 0.25);
            m_pads.first ->Draw();
            m_pads.second->Draw();
        } else {
            m_pads.first  = new TPad("pad1", "", 0.0, 0.0, 1.0, 1.0);
            m_pads.first->Draw();
        }

        loadXsec();
        loadHistograms();

        // Adding background distributions.
        cout << "<PlottingHelper::draw> Adding background distributions." << endl;
        
	// Normalising.
	if (m_normalised) {
	    float integral;

	    // -- Data
	    integral = m_data->Integral();
	    if (integral > 0) { m_data->Scale(1/integral); }

	    // -- Background
	    integral = m_sum->Integral();
	    if (integral > 0) {
	        m_sum->Scale(1/integral);
		for (const auto& p : m_backgrounds) { p.second->Scale(1/integral); }
	    }

	    // -- Signal
	    for (const auto& p : m_signals) {
	        integral = p.second->Integral();
		if (integral > 0) { p.second->Scale(1/integral); }
	    }
	}
        
        // -- Sorting by integral.
        m_backgroundsSorted.clear();
        for (const auto& p : m_backgrounds) {
            m_backgroundsSorted.push_back( pair<string, TH1F*>(p.first, p.second) );
        }
        std::sort(m_backgroundsSorted.begin(), m_backgroundsSorted.end(),
                  [](const pair<string, TH1F*> & p1, const pair<string, TH1F*> & p2) -> bool
                  {
                      return p1.second->Integral() < p2.second->Integral();
                  });
        
        THStack* background = new THStack("StackedBackgrounds", "");
        for (const auto& p : m_backgroundsSorted) {
            background->Add(p.second);
        }

        // Drawing (main pad).
        cout << "<PlottingHelper::draw> Going to first pad." << endl;
        m_pads.first->cd();
 
        background->Draw("HIST");
        gPad->Update();
        
        /* Get plot maximum. */
        const double plotmax   = max(m_data->GetMaximum(), m_sum->GetMaximum());
        const double plotmin   = (m_normalised ? 1e-05 : 0.5); // log-plots only
        const double maxoffset = 2.3; /* @TODO: Make dependent on the number of text lines. */
        
        if (m_log) {
            m_sum->SetMaximum(exp(maxoffset * (log(plotmax) - log(plotmin)) + log(plotmin)));
            m_sum->SetMinimum(plotmin);
        } else {
            m_sum->SetMaximum(maxoffset * plotmax);
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
            m_ratiohists["Ratio"] = (TH1F*) m_data->Clone("DataMC_Ratio");
	    m_ratiohists["Ratio"]->SetDirectory(0);
            m_ratiohists["Ratio"]->Divide(m_sum);
            
            m_ratiohists["StatsError"] = (TH1F*) m_ratiohists["Ratio"]->Clone("DataMC_StatsError");
	    m_ratiohists["StatsError"]->SetDirectory(0);
            for (unsigned i = 0; i < m_ratiohists["StatsError"]->GetXaxis()->GetNbins(); i++) {
                m_ratiohists["StatsError"]->SetBinContent(i + 1, 1);
                double c = m_sum->GetBinContent(i + 1);
                double e = m_sum->GetBinError(i + 1);
                m_ratiohists["StatsError"]->SetBinError  (i + 1, (c > 0 ? e/c : 0));
            }
            m_ratiohists["StatsError"]->GetYaxis()->SetTitle("Data / MC");
            
            // Styling.
            styleHist(m_ratiohists["Ratio"],      false, "Ratio");
            styleHist(m_ratiohists["StatsError"], true,  "StatsError");
            
            m_ratiohists["StatsError"]->GetXaxis()->SetTitleSize(m_fontSizeM * 0.75/0.25);
            m_ratiohists["StatsError"]->GetYaxis()->SetTitleSize(m_fontSizeM * 0.75/0.25);
            m_ratiohists["StatsError"]->GetXaxis()->SetLabelSize(m_fontSizeM * 0.75/0.25);
            m_ratiohists["StatsError"]->GetYaxis()->SetLabelSize(m_fontSizeM * 0.75/0.25);
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
        latex.SetTextSize(m_fontSizeL);
        latex.SetTextFont(72);
        
        string text;
        
        double xDraw = 0.21;
        double yDraw = 0.855;
        double yStep = m_fontSizeM * 1.25;
        
        text = "ATLAS #font[42]{Internal}";
        latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
        yDraw -= yStep * 1.3;
        
        latex.SetTextSize(m_fontSizeM);
        latex.SetTextFont(42);
        
        text = "#sqrt{s} = 13 TeV";
        latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
        yDraw -= yStep;
        
        std::stringstream lumi_stream;
        lumi_stream << std::setprecision(2) << m_lumi;
        text = "#lower[-0.2]{#scale[0.5]{#int}}L dt = " + lumi_stream.str() + " fb^{-1}";
        latex.DrawLatexNDC(xDraw, yDraw, text.c_str());
        yDraw -= yStep;
        
        for (const string& line : m_printlines) {
            latex.DrawLatexNDC(xDraw, yDraw, line.c_str());
            yDraw -= yStep;
        }

        
        // Legend.
        cout << "<PlottingHelper::draw> Drawing legends." << endl;
        unsigned N = (m_data ? 1 : 0) + m_backgrounds.size() + m_signals.size() + 1;

        xDraw = 0.65;
        yDraw = 0.95; // 0.9;
        
        double xmin = xDraw - yStep * 0.25;
        double ymax = yDraw - yStep * 1;
        
        double width  = 0.90 - xmin;
        double height = N * m_fontSizeS * 1.2;
        
        TLegend* legend = new TLegend(xmin, ymax - height, xmin + width, ymax);
        legend->SetTextSize(m_fontSizeS);
        legend->SetMargin(0.25);
        legend->SetFillStyle(0);
        
        // -- Data
        if (m_data) {
            legend->AddEntry(m_data, "Data", "PEL");
        }

        // -- Background(s).
        vector< pair<string, TH1F*> >::iterator it = m_backgroundsSorted.end();
        for (; it-- != m_backgroundsSorted.begin();) {
            legend->AddEntry(it->second, it->first.c_str(), "F");
        }

        // -- Signal(s).
        for (const auto& p : m_signals) {
	    stringstream stream;
	    stream << fixed << setprecision(1) << m_scaleSignal;
	    string s = stream.str();
	    legend->AddEntry(p.second, (p.first + (m_scaleSignal != 1 ? " #times " + s : "")).c_str(), "L");
        }
        
        // -- Stats. uncert.
        if (m_sum) {
            legend->AddEntry(m_sum, "Stat. uncert.", "F");
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
	  double x[100], y[100]; // @TOFIX
	  double xMaxImpr = 0., yMaxImpr = 0.;
	  for (unsigned i = 0; i < m_nbinsx; i++) {
	    double S, B;
	    switch (m_improvementDirection) {
	    case 0: // Integrate upwards.
	      x[i] = m_sum->GetXaxis()->GetBinLowEdge(i + 1);
	      S = m_signals.begin()->second->Integral(i + 1, m_nbinsx + 1);
	      B = m_sum                    ->Integral(i + 1, m_nbinsx + 1);
	      break;             
	    case 1: // Integrate downwards
	      x[i] = m_sum->GetXaxis()->GetBinUpEdge(i + 1);
	      S = m_signals.begin()->second->Integral(0, i + 1);
	      B = m_sum                    ->Integral(0, i + 1);
	      break;             
	    case 2: // Select bin.
	      x[i] = m_sum->GetXaxis()->GetBinCenter(i + 1);
	      S = m_signals.begin()->second->GetBinContent(i + 1);
	      B = m_sum                    ->GetBinContent(i + 1);
	      break;             
	    default:
	      break;
	    }
	    if (m_normalised) {
	      /* Right thing to do? */
	      y[i] = S / sqrt(S + B + eps);
	    } else {
	      y[i] = S / m_scaleSignal / sqrt(S / m_scaleSignal + B + eps);
	    }
	    if (y[i] > yMaxImpr) { 
	      yMaxImpr = y[i]; 
	      xMaxImpr = x[i];
	    }
	  }
	  
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
	  double ymax = yMaxImpr * maxoffset * 1.1;
	  double dy   = (ymax - ymin)/(1. - m_pads.first->GetTopMargin() - m_pads.first->GetBottomMargin());
	  double dx   = (m_xmax - m_xmin)/(1. - m_pads.first->GetLeftMargin() - m_pads.first->GetRightMargin());
	  
	  transPad->Range(m_xmin - m_pads.first->GetLeftMargin()   * dx, \
			  ymin   - m_pads.first->GetBottomMargin() * dy, \
			  m_xmax + m_pads.first->GetRightMargin()  * dx, \
			  ymax   + m_pads.first->GetTopMargin()    * dy);
	  
	  transPad->Draw();
	  transPad->cd();
	  impr->Draw("LP same");
	  transPad->Update();
	  
	  TArrow arrow;
	  arrow.SetLineWidth(3);
	  arrow.SetLineColor(kImprovementColor);
	  arrow.SetFillColor(kImprovementColor);
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
    
    void PlottingHelper::save (const string& path) {
        /* ... */
        return;
    }
    
    
    // Low-level management method(s).
    void PlottingHelper::closePads () {
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
    
    void PlottingHelper::loadHistograms () {
        
        cout << "<PlottingHelper::loadHistograms> Entering." << endl;
        
        if (m_filenames.size() == 0) {
            cout << "<PlottingHelper::loadHistograms> Warning: No input files were provided." << endl;
            return;
        }
        
        m_hists.clear();
        
         // Looping input files.
        // -------------------------------------------------------------------
        
        cout << "<PlottingHelper::loadHistograms>   Looping " << m_filenames.size() << " input files." << endl;
        unsigned it = 0;
        for (const string& filename : m_filenames) {
            
            if (!fileExists(filename)) {
                cout << "<PlottingHelper::loadHistograms> Warning: Input file '" << filename << "' doesn't exist." << endl;
                continue;
            }
            
            TFile* file = new TFile(filename.c_str(), "READ");
            
            TObject* obj = (TObject*) file->Get(m_input.c_str());
            
            if (obj == nullptr) {
                cout << "<PlottingHelper::loadHistograms> Warning: Requested input '" << m_input << "' could not be retrieved from file '" << filename << "'." << endl;
                continue;
            }
            
            double sumOfWeights = 1.;
            //sumOfWeights = ((TH1F*) file->Get("h_rawWeight"))->Integral();
            
            
             // Get histogram.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            TH1F* hist;
            
            if ((hist = dynamic_cast<TH1F*>(obj))) {
                
                /* Reading histogram. */
                hist->SetDirectory(0);     // < Keep in memory after file is closed!
                TH1::AddDirectory(kFALSE); // < Keep in memory after file is closed!

                hist->Rebin(m_rebin);
                
            } else if (TTree* tree = dynamic_cast<TTree*>(obj)) {
                
                /* Reading tree. */
                /* @TODO: sDistinguish between the number of ':' in the input name, to allow for TH2 as well. */
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
                
                cout << "<PlottingHelper::loadHistograms> Warning: Requested input '" << m_input << "' could not be cast as either TH1F* or TTree*." << endl;
                continue;
                
            }
            
            
            
             // Get 'isMC' and 'DSID'.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            /* @TODO: Try from file name, assuming standard naming convention, eg. using regex. If fail, look in tree. */
            
            bool     isMC = 0, isSignal = 0;
            unsigned DSID = 0;
            
            /* From tree
             TTree* inputTree = (TTree*) file->Get("outputTree");
             
             TBranch *isMCBranch, *DSIDBranch;
             
             inputTree->SetBranchAddress( "isMC", &isMC, &isMCBranch );
             inputTree->SetBranchAddress( "DSID", &DSID, &DSIDBranch );
             
             const unsigned int nEvents = inputTree->GetEntries();
             if (nEvents == 0) {
             cout << " -- (File '" << filename << "' is empty.)" << endl;
             continue;
             }
             
             inputTree->GetEvent(0);
             */
            
            /* From filename. */
            vector<string> path_fields = split(filename, '/');
            vector<string> filename_fields = split(path_fields.at(path_fields.size() - 1), '.');
            vector<string> file_fields = split(filename_fields.at(filename_fields.size() - 2), '_');
            string DSID_string = file_fields.at(file_fields.size() - 1);
            DSID = stoi(DSID_string);
            
            if (m_info.count(DSID) == 0) {
                cout << "<PlottingHelper::loadHistograms> Warning: DSID '" << DSID << "' was not found in m_info." << endl;
                continue;
            }
            SampleInfo info = m_info.at(DSID);
            
            isMC     = (info.type != PlotType::Data);    //(filename.find("MC") != string::npos);
            isSignal = (info.type == PlotType::Signal);

            
            
             // Perform styling.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            if (m_axistitles.at(0) != "" && !m_ratio) {
                hist->GetXaxis()->SetTitle(m_axistitles.at(0).c_str());
            } else {
                hist->GetXaxis()->SetLabelOffset(9999.9);
            }
            if (m_axistitles.at(1) != "") {
  	        hist->GetYaxis()->SetTitle((m_axistitles.at(1) + (m_normalised ? " (Normalised)" : "")).c_str());
            }
            /* @TODO: z-axis */
            
            styleHist(hist, isMC, info.name); // DSID);

            
            
             // Perform scaling.
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            if (isMC) {
	        //hist->Scale(info.xsec / sumOfWeights);
	        hist->Scale(info.xsec / float(info.evts));
                if (m_lumi > 0) {
                    hist->Scale(m_lumi); // @TODO: Correct event weights.
                }
                if (isSignal) {
		    hist->Scale(m_scaleSignal);
                    assert( m_signals.count(info.name) == 0 );
                    m_signals[info.name] = (TH1F*) hist->Clone(("autohist_Signal_" + info.name).c_str()); //.insert( pair<unsigned, TH1F*>(DSID, hist) );
		    m_signals[info.name]->SetDirectory(0);
                } else {
                    if (m_backgrounds.count(info.name) == 0) {
                        m_backgrounds[info.name] = (TH1F*) hist->Clone(("autohist_Background_" + info.name).c_str()); //.insert( pair<unsigned, TH1F*>(DSID, hist) );
			m_backgrounds[info.name]->SetDirectory(0);
                    } else {
                        m_backgrounds[info.name]->Add(hist);
                    }
                    if (m_sum) {
                        m_sum->Add(hist);
                    } else {
                        m_sum = (TH1F*) hist->Clone("autohist_Sum");
			m_sum->SetDirectory(0);
                        m_sum->Sumw2();
                    }
                }
            } else {
                if (m_data) {
                    m_data->Add(hist);
                } else {
                    m_data = (TH1F*) hist->Clone("autohist_Data");
		    m_data->SetDirectory(0);
                    m_data->Sumw2();
                }
            }
            
            //m_hists[info.type][info.name].push_back(hist);

            file->Close();
           
        }

	if (!m_data) { 
	  m_data = (TH1F*) m_sum->Clone("autohist_Data");
	  m_data->SetDirectory(0);
	  m_data->Scale(0.);
	}
        
        styleHist(m_sum,  true,  "StatsError");
        styleHist(m_data, false, "Data");
        
        m_outfile->cd();
        
        cout << "<PlottingHelper::loadHistograms> Exiting." << endl;

        return;
    }
    
    
    void PlottingHelper::loadXsec () {
        
        cout << "<PlottingHelper::loadXsec> Entering." << endl;
        
        /***************************************\
        |* Load stuff from cross section file. *|
        \***************************************/
        
        m_info.clear();
        
        assert( m_xsecfile != "" );
        if (!fileExists(m_xsecfile)) {
            cout << "<PlottingHelper::loadXsec> ERROR: Cross section file '"<< m_xsecfile <<"' was not found. Exiting." << endl;
            return;
        }
        
        ifstream file ( m_xsecfile.c_str() );
        if (!file.is_open()) {
            cout << "<PlottingHelper::loadXsec> ERROR: Cross section file could not be opened. Exiting." << endl;
            return;
        }
        string line = "";
        while ( file.good() ) {
            
            std::getline ( file, line );
	    line = trim(line);
            if (line             == "")  { continue; }
	    if (line.substr(0,1) == "#") { continue; }
            vector<string> fields = split(line, ',');
            
	    unsigned DSID;
	    double   xsec;
	    unsigned evts;
	    double   eff;
	    string   name;
	    
	    // -- Dataset ID.
            DSID = (unsigned) stoi(fields.at(0));
            assert (m_info.count(DSID) == 0);
            
	    // -- Cross section.
	    xsec = stod(fields.at(1));

	    // -- Number of events.
	    evts = (unsigned) stoi(fields.at(2));

	    if (evts == 0) {
	        std::cout << "<PlottingHelper::loadXsec> Warning: DSID '" << DSID << "' has 0 events. Skipping." << std::endl;
		continue; 
	    }

	    // -- Generator filter efficiency.
	    eff = stod(fields.at(3));

	    // -- Physics name.
	    name = (string) fields.at(4);
	    name = trim(name);

	    bool signal = false;
	    std::regex re_singlePhoton1("Sherpa_CT10_SinglePhotonPt([0-9]+).*");
	    std::regex re_singlePhoton2("Sherpa_CT10_SinglePhotonPt([0-9]+)_([0-9]+).*");
            std::regex re_signal(".*dmA_jja_Ph([0-9]+)_mRp0([0-9]+).*");
	    std::smatch re_match_name;
	    if        (std::regex_match(name, re_match_name, re_singlePhoton2)) {
	      name = "Incl. #gamma p_{T} #in [" + string(re_match_name[1]) + ", " + string(re_match_name[2]) + "] GeV";
	    } else if (std::regex_match(name, re_match_name, re_singlePhoton1)) {
	      name = "Incl. #gamma p_{T} > " + string(re_match_name[1]) + " GeV";
	    } else if (std::regex_match(name, re_match_name, re_signal)) {
	      name = "Z' (" + string(re_match_name[2]) + " GeV)";
	      signal = true;
	    }

	    // -- Store in SampleInfo.
            SampleInfo info;
            info.DSID = DSID;
            info.name = name;
            info.evts = evts;

	    /* Ex.: MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp055_mD10_gSp3_gD1 */
	    //std::regex re_signal(".*dmA_jja_.*");
            //std::smatch re_match_signal;
            //bool signal = (bool) std::regex_match(name, re_match_signal, re_signal);
	    
	    info.type = (signal ? PlotType::Signal : PlotType::Background);
	    info.xsec = xsec * eff;

	    /*
            if (data){
                info.type = PlotType::Data;
                info.lumi = stod(fields.at(1));
	    }
	    */
            m_info[DSID] = info;

        }
        
        file.close();
        
        cout << "<PlottingHelper::loadXsec> Exiting." << endl;
        
        return;
    }
    
    
    void PlottingHelper::styleHist (TH1F* hist, const bool& isMC, const string& name) { // const unsigned& DSID) {
        
        cout << "<PlottingHelper::styleHist> Calling with: '" << name << "' (" << (isMC ? "MC" : "data") << ")." << endl;
        
	if (!hist) { return; }

        // Base styling.
        hist->SetTitle("");
        
        hist->SetLineWidth(2);
        hist->SetLineColor(kBlack);
        
        hist->GetXaxis()->SetTitleSize(m_fontSizeM);
        hist->GetYaxis()->SetTitleSize(m_fontSizeM);
        
        hist->GetXaxis()->SetTitleOffset(1.3);
        hist->GetYaxis()->SetTitleOffset(1.7);
        
        hist->GetXaxis()->SetLabelSize(m_fontSizeM);
        hist->GetYaxis()->SetLabelSize(m_fontSizeM);
        
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
                hist->SetLineColor(kRed + m_signals.size() * 2);
                
            } else {
                
                hist->SetFillStyle(1001);
                
            }
            
            hist->SetFillColor(kRed); // TEMP

	    /* Incl. gamma */
            std::regex re_inclGamma("Incl. #gamma.*");
            std::smatch re_match_inclGamma;
	    if      (std::regex_match(name, re_match_inclGamma, re_inclGamma)) { 
	      hist->SetFillColor (kAzure + m_backgrounds.size() + 1); 
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
            hist->SetMarkerSize (0.5);
            hist->SetMarkerColor(kBlack);
            
            hist->SetLineColor(kBlack);
            
        }
        
        return;
        
    }
    
}
