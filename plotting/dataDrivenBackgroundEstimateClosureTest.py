# Dear emacs, this is -*- python -*-
import sys

from snippets.style     import *
from snippets.functions import *

from scripts.transferFactor import *

from ROOT  import *
from array import array

import math
import itertools
import time

def displayName (var):
    if   var == "pt": return "p_{T}"
    elif var == "m":  return "M"
    return var

def displayUnit (var):
    if   var == 'pt': return 'GeV'
    elif var == 'm':  return 'GeV'
    return ''


# Main function.
# ----------------------------------------------------------------------------------------------------
def main ():
 
    # Whether to save plots to file.
    save = True
    
    # Load ROOT dictionary for reading TLorentzVectors from file (no necessary).
    gROOT.ProcessLine(".L ../share/Loader.C+");
   
    # Initial setup.
    xsec = loadXsec('share/sampleInfo.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get. 
    treename = 'BoostedJetISR/EventSelection/{0}/tau21_mod_rhoDDT/Postcut'
    prefix   = 'leadingfatjet_'
    getvars  = ['m', 'rhoDDT', 'pt']

    categories = [ 'Fail', # 'Background (transferred failing data)'
                   'Pass', # 'Data (passing incl. gamma MC)',
                  ]

    # Load data.
    values = dict()
    values['Pass'] = loadDataFast(paths, treename.format('Pass'), getvars, prefix, xsec, 
                                  keepOnly = (lambda DSID: 361039 <= DSID <= 361062)
                                  )
    values['Fail'] = loadDataFast(paths, treename.format('Fail'), getvars, prefix, xsec, 
                                  keepOnly = (lambda DSID: 361039 <= DSID <= 361062)#keepOnly = (lambda DSID: 276262 <= DSID <= 284484 )
                                  )
    
    if not values['Pass'] and not values['Fail']:
        print "WARNING: No value were loaded."
        return

    # Get number of events.
    Nevents = { pf : len(values[pf]['m']) for pf in categories }
    
    # Plotting stuff.
    lines = [ "#sqrt{s} = 13 TeV,  L = 1 fb^{-1}",
              "Inclusive #gamma MC",
              "HLT_j380",
              "Trimmed anti-k_{t}^{R=1.0} jets",
              "At least 1 jet with p_{T} > 400 GeV",
              "Exactly 1 #gamma with p_{T} > 155 GeV",
              "|#Delta#phi(J,#gamma)| > #pi / 2",
              "Jet p_{T} > 2 M",
              #"Trimmed anti-k_{t}^{R=1.0} jets",
              #"Req. 1 #gamma with p_{T} > 100 GeV",
              ]

    colours = [kViolet + 7, kAzure + 7, kTeal, kSpring - 2, kOrange - 3, kPink]


    # ==============================================================
    # Data driven background estimate closure test.
    #
    # The inclusive gamma background in the pass region is treated 
    # as the data (since we're blinded), and the data in the fail 
    # region is propagated into the signal region, using the 
    # transfer factor method, and is treated as the background 
    # estimate (which it also will be in the end).
    # --------------------------------------------------------------

    if True:

        variables  = ['m']
        
        axislimits = {
            'm': (100, 0, 300),
            }

        distributions = { 
            var : {
                cat : TH1F('%s distribution, %s' % (var, cat), "", *axislimits[var]) for cat in categories
                } for var in variables }

        for pf in categories:
            for ievent in xrange(Nevents[pf]):
                for var in variables:

                    # Axis variables.
                    xmin, xmax = axislimits[var][1:]
                    binw = (xmax - xmin)/float(axislimits[var][0])

                    # Transfer factor. One for "signal data/pass".
                    tf = 1.

                    # Data driven method.
                    if pf == 'Fail':
                        tf = transferFactorCalculator(values[pf]['rhoDDT'][ievent], values[pf]['pt'][ievent])
                        pass

                    # Fill distributions.
                    val = values[pf][var]     [ievent]
                    w   = values[pf]['weight'][ievent]
                    distributions[var][pf].Fill( min(max(val, xmin + binw/2.), xmax - binw/2.),
                                                 w * tf)
                    
                    pass # end: loop variables            
                pass # end: loop events
            pass # end: pass/fail

        
        for var in variables:

            
            dists = [distributions[var][cat] for cat in categories]

            mcerror = dists[0].Clone('McError')
            #mcerror.SetFillStyle(3405)
            mcerror.SetMarkerSize(0)
            mcerror.SetFillStyle(1001)
            

            #dists.insert(1, mcerror)

            ratio = dists[-1].Clone('Ratio')
            ratio.Divide(mcerror)

            mcerrratio = mcerror.Clone('mcerrratio')
            mcerrratio.Divide(mcerror)
            for i in xrange(mcerrratio.GetXaxis().GetNbins()):
                if mcerrratio.GetBinError(i + 1) == 0.:
                    mcerrratio.SetBinError(i + 1, 9999.)
                    pass
                pass
                

            # Make plot.
            legendOpts = LegendOptions(histograms = dists,
                                       names = ['Data-driven bkg.',
                                                'Pseudo-data',
                                                ],
                                       #categories = [(mcerrratio, 'Background stat. uncert.', 'FL')],
                                       xmin = 0.58, ymax = 0.845,
                                       types = ['FL', 'PLE'])
            textOpts   = TextOptions(lines = lines)

            c = TCanvas('c', "", 600, 700)

            pads = [ TPad('pad1', "", 0, 0.25, 1., 1.),
                     TPad('pad2', "", 0, 0, 1., 0.25),
                          ]
            for pad in pads:
                c.cd()
                pad.Draw()
                pass

            # Ensure proper ratio-pad styling.
            pads[0].SetBottomMargin(0.02)

            pads[1].SetTopMargin(0.04)
            pads[1].SetBottomMargin(0.35)
 
            mcerrratio.GetXaxis().SetLabelSize  ( dists[0].GetXaxis().GetLabelSize()   * 3.)
            mcerrratio.GetXaxis().SetTitleSize  ( dists[0].GetXaxis().GetTitleSize()   * 3.)
            #mcerrratio.GetXaxis().SetTitleOffset( dists[0].GetXaxis().GetTitleOffset() / 3.)
            mcerrratio.GetXaxis().SetTickLength ( dists[0].GetXaxis().GetTickLength()  * 3.)

            mcerrratio.GetYaxis().SetLabelSize  ( dists[0].GetYaxis().GetLabelSize()   * 3.)
            mcerrratio.GetYaxis().SetTitleSize  ( dists[0].GetYaxis().GetTitleSize()   * 3.)
            mcerrratio.GetYaxis().SetTitleOffset( dists[0].GetYaxis().GetTitleOffset() / 3.)
            mcerrratio.GetYaxis().SetNdivisions(503)

            dists[0].GetXaxis().SetTitleOffset(9999.)
            dists[0].GetXaxis().SetLabelOffset(9999.)


            # Draw main plot
            makePlot( dists,
                      legendOpts,
                      textOpts,
                      canvas = pads[0],
                      drawOpts = ['HIST', 'PLE'],
                      xtitle = "Leading jet %s %s" % (displayName(var), '[%s]' % displayUnit(var) if displayUnit(var) != '' else ''),
                      ytitle = 'Events',
                      colours = [ colours[1], kBlack],
                      padding = 1.2,
                      )

            # Draw ratio plot
            makePlot( [mcerrratio, ratio],
                      canvas = pads[1],
                      xtitle = "Leading jet %s %s" % (displayName(var), '[%s]' % displayUnit(var) if displayUnit(var) != '' else ''),
                      ytitle = 'Data / MC',
                      drawOpts = ['E3', 'PLE'],
                      colours = [kGray + 2, kBlack], 
                      ymin = 0.5,
                      ymax = 1.5,
                      ylines = [1.],
                      )
            c.cd()
                      

            # -- Show.
            gPad.Update()
            wait()

            savedir = 'plotting/'
            savename = "plot_dataDrivenBackgroundEstimateClosureTest.pdf"
            print "Savename: '%s'" % (savedir + savename)
            if save:
                c.SaveAs(savedir + savename)
                pass
            
            
            pass # end: loop variables
        
        pass # end: jet trigger efficiency turn-on

    return

if __name__ == '__main__':
    main()
    pass
