# Dear emacs, this is -*- python -*-
import sys

from snippets.style     import *
from snippets.functions import *

from ROOT  import *
from array import array

import math
import itertools

def rho (m, pt):
    if   m  <= 0: return -1E+10
    elif pt <= 0: return +1E+10
    return math.log(pow(m, 2.) / pow(pt, 2.0))

def rhoPrime (m, pt):
    if   m  <= 0: return -1E+10
    elif pt <= 0: return +1E+10
    return math.log(pow(m, 2.) / pow(pt, 1.4))

def rhoDDT (m, pt):
    if   m  <= 0: return -1E+10
    elif pt <= 0: return +1E+10
    return math.log(pow(m, 2.) / pow(pt, 1.0))


# Main function.
# ----------------------------------------------------------------------------------------------------
def main ():
 
    gROOT.ProcessLine(".L ../share/Loader.C+");
   
    # Initial setup.
    xsec = loadXsec('../share/weightsMC.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get. 
    #treename = 'BoostedJetISR/Fatjets/Nominal/dPhi/Postcut'
    treename = 'BoostedJet+ISRgamma/EventSelection/Pass/NumFatjets/Postcut'
    prefix   = ''
    #getvars  = ['m', 'pt', 'pt_ut', 'tau21', 'tau21_ut']
    getvars  = ['leadingfatjet_m', 'leadingfatjet_pt', 'tau21DDT']

    # Load data.
    values = loadDataFast(paths, treename, getvars, prefix, xsec, 
                          #ignore = (lambda DSID: 305367 <= DSID <= 305374 )
                          keepOnly = (lambda DSID: 361039 <= DSID <= 361062 )
                          )

    if not values: 
        print "WARNING: No value were loaded."
        return

    Nevents = len(values[getvars[0]])


    values['m']  = values.pop('leadingfatjet_m')
    values['pt'] = values.pop('leadingfatjet_pt')

    # -- Compute trimmed rho variables
    '''
    print "Computing trimmed rho variables"
    values['rho']         = map(lambda (m,pt): rho     (m,pt), zip(values['m'], values['pt']))
    values['rhoPrime']    = map(lambda (m,pt): rhoPrime(m,pt), zip(values['m'], values['pt']))
    values['rhoDDT']      = map(lambda (m,pt): rhoDDT  (m,pt), zip(values['m'], values['pt']))

    # -- Compute untrimmed rho variables
    print "Computing untrimmed rho variables"
    values['rho_ut']      = map(lambda (m,pt): rho     (m,pt), zip(values['m'], values['pt_ut']))
    values['rhoPrime_ut'] = map(lambda (m,pt): rhoPrime(m,pt), zip(values['m'], values['pt_ut']))
    values['rhoDDT_ut']   = map(lambda (m,pt): rhoDDT  (m,pt), zip(values['m'], values['pt_ut']))

    print "Done computing new variables"
    '''    
    
    # ==============================================================
    # Performing linear corrections
    # --------------------------------------------------------------

    if False: #True:

        # Definitions
        pTcut = 150.

        xaxislimits = {
            'rho':         (-5.5, -1.0),
            'rhoPrime':    (-2.5,  4.0),
            'rhoDDT':      (-0.5,  8.0),
            }
        
        fit_limits = {
            'rho':         (-4.5, -1.0),
            'rhoPrime':    (-1.5,  4.0),
            'rhoDDT':      ( 1.0,  8.0),
            }
                
        # Variables to correct
        #xvars   = ['rho',    'rhoPrime',    'rhoDDT']
        xvars   = ['m']
        yvars   = ['tau21DDT']

        # Standard profiles
        profiles = {
            yvar : {
                xvar : TProfile('<%s> vs. %s'     % (yvar, xvar), "", 50, xaxislimits[xvar][0], xaxislimits[xvar][1], -1, 3) for xvar in xvars
                } for yvar in yvars  }

        # Linearly corrected profiles
        profiles_mod = {
            yvar : {
                xvar : TProfile('<%s> vs. %s mod' % (yvar, xvar), "", 50, xaxislimits[xvar][0], xaxislimits[xvar][1], -1, 3) for xvar in xvars
                } for yvar in yvars  }

        # Loop events, fill standard profiles
        for ievent in xrange(Nevents):
            for yvar, xvar in itertools.product(yvars, xvars):
                #if values['pt'][ievent] < pTcut: continue
                #if values['m'][ievent] > values['pt'][ievent] / 2.: continue # Boosted regime
                profiles[yvar][xvar].Fill( values[xvar][ievent],
                                           values[yvar][ievent],
                                           values['weight'][ievent]
                                           )

                pass
            pass

        # Perform fits, and fill linearly corrected profiles
        for yvar, xvar in itertools.product(yvars, xvars):
            print " == <%s> vs. %s" % (yvar, xvar)
            fit    = TF1('fit_%s_%s'    % (yvar, xvar), 'pol1', fit_limits[xvar][0], fit_limits[xvar]\
                             [1])
            fitmod = TF1('fitmod_%s_%s' % (yvar, xvar), 'pol1', fit_limits[xvar][0], fit_limits[xvar]\
                             [1])
            fit   .SetLineColor(kGray + 3)
            fitmod.SetLineColor(kGray + 3)
            fitmod.SetLineStyle(2)
            
            profiles[yvar][xvar].Fit(fit, 'R')
            
            y  = values[yvar]
            x  = values[xvar]
            x1 = fit_limits[xvar][0]

            values['%s_mod_%s' % (yvar, xvar)] = array('d', [ y + (fit.Eval(x1) - fit.Eval(x)) for x,\
y in zip(values[xvar], values[yvar])])
            
            for ievent in xrange(Nevents):
                if values['pt'][ievent] < pTcut: continue
                if values['m'][ievent] > values['pt'][ievent] / 2.: continue
                profiles_mod[yvar][xvar].Fill( values[xvar][ievent],
                                               values['%s_mod_%s' % (yvar, xvar)][ievent],
                                               values['weight'][ievent]
                                               )
                pass
            
            profiles_mod[yvar][xvar].Fit(fitmod, 'R')
            

            # Draw profiles
            lines = [ "#sqrt{s} = 13 TeV",
                      "Inclusive #gamma events",
                      "Trimmed anti-k_{t}^{R=1.0}",
                      "Req. 1 #gamma with p_{T} > 155 GeV",
                      "Jet p_{T} > %d GeV" % int(pTcut),
                      "Jet M < p_{T} / 2 (boosted)",
                      ]
            
            names = ["Uncorrected", "Corrected"]
            
            #colours = [kRed + 1, kBlue + 1]
            #colours = [ kAzure + 1, kOrange + 1]
            colours = [kRed + 0, kRed + 2]
            
            
            ps    = profiles    [yvar][xvar]
            psmod = profiles_mod[yvar][xvar]
            
            histograms = [ ps, psmod, fit, fitmod ]
            
            legendOpts = LegendOptions(histograms = histograms,
                                       names = names,
                                       xmin = 0.59,
                                       ymax = 0.835)
            
            textOpts   = TextOptions(lines = lines)
            
            c = makePlot( histograms,
                          legendOpts,
                          textOpts,
                          #padding = 1.2,
                          ymax = 1.5,
                          xtitle = "%s %s" % (displayName(xvar), displayUnit(xvar)),
                          ytitle = "#LT%s#GT %s" % (displayName(yvars[0]), displayUnit(yvars[0])),
                          colours = colours)

            text = TLatex()
            offset = 0.05
            scale  = 1.
            xx =  c.GetLeftMargin() + offset * scale
            yy =  c.GetBottomMargin() + offset

            text.DrawLatexNDC(xx, yy, 
                              "#chi^{2}/N_{dof} = %.1f/%d (p = %4.2f)" % (fit.GetChisquare(), 
                                                                          fit.GetNDF(), 
                                                                          fit.GetProb()))
            text.DrawLatexNDC(xx, yy + 1.3 * text.GetTextSize(),
                              "Fit: %.3f + %s #times (%.3f)" % (fit.GetParameter(0),
                                                                displayName(xvar),
                                                                fit.GetParameter(1)))

            savename = "correction_%s_vs_%s__pt_%dGeV.pdf" % (yvar, xvar, pTcut)
            print savename

            #wait()
            c.SaveAs(savename)
            del c

            pass
        
        pass # end section: linear correction 


    # ==============================================================
    # Comparing (un-)trimmed tau21 profiles.
    # --------------------------------------------------------------

    if True:

        # Define variables
        boosted = True

        xaxislimits = {
            'pt':          ( 0, 2000),
            'm':           ( 0,  300),
            'rho':         (-9,    0),
            'rho_ut':      (-9,    0),
            'rhoPrime':    (-4,    4),
            'rhoPrime_ut': (-4,    4),
            'rhoDDT':      (-2,    7),
            'rhoDDT_ut':   (-2,    7),
            }
        
        cutsdict = {
            # -- pT slices
            'pt': [
                ( 150,  200),
                ( 400,  500),
                ( 1000, 1500),
                ]
            }

        #xvars   = ['rho',    'rhoPrime',    'rhoDDT',
        #           'rho_ut', 'rhoPrime_ut', 'rhoDDT_ut']
        xvars = ['m']
        #yvars = ['tau21', 'tau21_ut']
        #yvars = ['tau21_mod_rhoDDT', 'tau21_mod_rhoPrime']
        yvars = ['tau21DDT']

        cutvars = [p[0] for p in cutsdict.items()]

        colours = [kRed + 2 * i for i in range(3)]

        lines = [ "#sqrt{s} = 13 TeV",
                  "Inclusive #gamma events",
                  "Trimmed anti-k_{t}^{R=1.0}",
                  "Req. 1 #gamma with p_{T} > 155 GeV",
                  #"Restricted #rho'/#rho^{DDT} range",
                  ]

        # Define histograms to be plotted.
        profiles = { yvar : { xvar : { cutvar : [] for cutvar in cutvars } for xvar in xvars } for yvar in yvars  }

        density = dict()

        for yvar, xvar, cutvar in itertools.product(yvars, xvars, cutvars):
            if not xvar in density:
                density[xvar] = TH1F("Density_%s" % xvar, "", 50, xaxislimits[xvar][0], xaxislimits[xvar][1])
                pass

            for icut, _ in enumerate(cutsdict[cutvar]):
                ps = profiles[yvar][xvar][cutvar]
                ps.append( TProfile("<%s> vs. %s, %s cut %d" % (yvar, xvar, cutvar, icut), "",
                                    30, xaxislimits[xvar][0], xaxislimits[xvar][1],
                                    -1000, 2000, ('' if 'mod' in yvar else '')) )
                pass
            pass
        
        # Loop events
        for ievent in xrange(Nevents):

            # Require boosted regime
            #if boosted and values['m'][ievent] > values['pt'][ievent] / 2.: continue

            # Loop variables
            for yvar, xvar, cutvar in itertools.product(yvars, xvars, cutvars):

                # Impose rho-constraints
                if yvar == 'tau21_mod_rhoPrime':
                    if values['rhoPrime'][ievent] < -1.5: continue
                elif yvar == 'tau21_mod_rhoDDT':
                    if values['rhoDDT'][ievent]   <  1.0: continue
                    pass
                
                # Fill density
                if yvar == yvars[0]:
                    density[xvar].Fill( values[xvar]    [ievent],
                                        values['weight'][ievent] )
                    pass

                # Loop cuts
                for icut, cut in enumerate(cutsdict[cutvar]):                    
                    if not(cut[0] <= values[cutvar][ievent] < cut[1]): continue
                 
                    # Fill profiles
                    ps = profiles[yvar][xvar][cutvar]
                    ps[icut].Fill( values[xvar]    [ievent],
                                   values[yvar]    [ievent],
                                   values['weight'][ievent] )
                    pass
                pass 
            pass 

        for xvar, cutvar in itertools.product(xvars, cutvars):
            cuts = cutsdict[cutvar]

            print [str(yvar) + '_' + str(icut) for yvar in yvars for icut,_ in enumerate(cuts)]
            histograms = [profiles[yvar][xvar][cutvar][icut] for yvar in yvars for icut,_ in enumerate(cutsdict[cutvar])]
            
            names = ['[%d, %d] %s' % (cut[0], cut[1], displayUnit(cutvar)) for cut in cuts]


            categoryHists = [TH1F('hcat0', "", 1, 0, 1),
                             TH1F('hcat1', "", 1, 0, 1)]

            for h in categoryHists:
                h.SetLineColor  (kGray + 3)
                h.SetMarkerColor(kGray + 3)
                pass
            categoryHists[0].SetMarkerStyle(20)
            categoryHists[1].SetMarkerStyle(24)

            legendOpts = LegendOptions(histograms = [], 
                                       names = names, 
                                       header = '%s slices:' % displayName(cutvar), 
                                       #categories = [("#rho^{DDT}", categoryHists[0], 'P'),
                                       #              ("#rho'",      categoryHists[1], 'P')], 
                                       xmin = 0.59, 
                                       ymax = 0.835)

            textOpts   = TextOptions(lines = lines + (["Jet M < p_{T} / 2 (boosted)"] if boosted else []))

            c = makePlot( histograms, 
                          legendOpts,
                          textOpts,
                          xtitle = "%s %s" % (displayName(xvar), '[%s]' % displayUnit(xvar) if displayUnit(xvar) != '' else displayUnit(xvar)),
                          ytitle = "#LT%s#GT %s" % (displayName(yvars[0]), displayUnit(yvars[0])),
                          colours = colours + colours,
                          markers = 3*[20]  + 3*[24])

            savename = 'trim-untrim_substructureProfile_%s_vs_%s.pdf' % (yvars[0], xvar)

            print "savename: '%s'" % savename

            wait()
            c.SaveAs(savename)

            pass
        
        pass
    
    # ...

    # ==============================================================
            
    return 0



if __name__ == '__main__':
    main()
    pass
