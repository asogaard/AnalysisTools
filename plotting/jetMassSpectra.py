# Dear emacs, this is -*- python -*-
import sys

from AStyle     import *
from AFunctions import *

from ROOT  import *
from array import array

import math
import itertools


def displayName (var):
    if  var == 'tau21':
        return'#tau_{21}'
    if  var == 'tau21mod':
        return'#tilde{#tau}_{21}'
    elif var == 'D2':
        return 'D_{2}'
    elif var == 'D2mod':
        return '#tilde{D}_{2}'
    elif var == 'pt':
        return 'p_{T}'
    elif var == 'm':
        return 'M'
    elif var == 'rho':
        return '#rho'
    elif var == 'rhoPrime':
        return "#rho'"
    elif var == 'rhoDDT':
        return '#rho^{DDT}'
    return var

def displayUnit (var):
    if   var == 'pt':
        return 'GeV'
    elif var == 'm':
        return 'GeV'
    return ''

def rho (m, pt):
    return math.log(pow(m + eps, 2.) / pow(pt, 2.))

def rhoPrime (m, pt):
    if max(m, 0) == 0:
        return -1E+10
    return math.log(pow(m, 2.) / pow(pt, 1.4))

def rhoDDT (m, pt):
    return math.log(pow(m + eps, 2.) / float(pt * 1.))


# Main function.
# ----------------------------------------------------------------------------------------------------
def main ():
    
    # Initial setup.
    xsec = loadXsec('../share/weightsMC.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get. 
    treename = 'BoostedJetISR/Fatjets/Nominal/dPhi/Postcut'
    prefix   = 'plot_fatjet_'
    getvars  = ['pt', 'm', 'tau21', 'D2', 'D2mod', 'rho', 'rhoDDT']

    # Load data.
    values = loadData(paths, treename, getvars, prefix, xsec, ignore = (lambda DSID: 305367 <= DSID <= 305374 ))

    if not values: return
    Nevents = len(values[getvars[0]])

    values['rhoPrime'] = map(lambda (m,pt): rhoPrime(m,pt), zip(values['m'], values['pt']))

    def modFunc (rhoval):
        ''' Linear correction function for rhoPrime = log(m^2/pt^1.4) '''
        p0 =  0.477415
        p1 = -0.103591
        return p0 + p1 * rhoval

    rhomin = -1.0
    values['tau21mod'] = map(lambda (t,rp): t + (modFunc(rhomin) - modFunc(rp)), zip(values['tau21'], values['rhoPrime']))

    # ==============================================================
    # Plot substructure variables.
    # --------------------------------------------------------------
    
    if False:
        
        variables = ['tau21mod']

        histograms = { var: TH1F('%s' % var, "", 100, -1, 1) for var in variables }

        for var, h in histograms.iteritems():
            for ievent in range(Nevents):
                h.Fill( values[var]     [ievent],
                        values['weight'][ievent] )
                pass

            h.GetXaxis().SetTitle( displayName(var) )
            h.GetYaxis().SetTitle("Number of jets (a.u.)")

            c = TCanvas('c', "", 600, 600)

            h.Draw('HIST')

            drawText()
            gPad.Update()
            wait()
            pass

        pass

    # ==============================================================
    # Plot substructure variable profiles.
    # --------------------------------------------------------------

    if True:
        
        # Define cut binning.
        binsdict = {
            'tau21': [
                (0.1, 0.2),
                (0.2, 0.3),
                (0.3, 0.4),
                (0.4, 0.5),
                (0.5, 0.6),
                (0.6, 0.7),
                (0.7, 0.8),
                (0.8, 0.9),
                ],
            
            'tau21mod': [
                (0.0, 0.1),
                (0.1, 0.2),
                (0.2, 0.3),
                (0.3, 0.4),
                (0.4, 0.5),
                (0.5, 0.6),
                (0.6, 0.7),
                (0.7, 0.8),
                (0.8, 0.9),
                ],
            
            'D2': [
                (0.0, 0.5),
                (0.5, 1.0),
                (1.0, 1.5),
                (1.5, 2.0),
                (2.0, 2.5),
                (2.5, 3.0),
                (3.0, 3.5),
                (3.5, 4.0),
                ],
            
            'D2mod': [
                (0.0, 0.5),
                (0.5, 1.0),
                (1.0, 1.5),
                (1.5, 2.0),
                (2.0, 2.5),
                (2.5, 3.0),
                (3.0, 3.5),
                (3.5, 4.0),
                ],
            
            'pt' : [
                ( 250.,  300.),
                ( 300.,  400.),
                ( 400.,  500.),
                ( 500.,  600.),
                ( 600.,  800.),
                ( 800., 1500.),
                ],
            
            'rho': [
                (-8, -7),
                (-7, -6),
                (-6, -5),
                (-5, -4),
                (-4, -3),
                (-3, -2),
                (-2, -1),
                ]
            }

        DDTmin = -6.0
        DDTmax = -1.0
        
        cuts = {
            'tau21': {},
            'tau21mod': {
                'rhoPrime' : (-1.0, 2.0),
                },
            'D2'   : {},
            'D2mod': {
                'rho' : (DDTmin, DDTmax),
                },
            'pt'   : {
                'tau21': (0.0, 0.4),
                
                #'D2':    (0.0, 1.2),

                #'D2mod': ( 0.0, 2.3),
                #'rho':   (-6,  -1),

                #'tau21mod': ( 0.0, 0.6),
                #'rhoPrime': (-1,   2),
                },
            'rho'  : {
                #'tau21' : (0.0, 0.38),
                #'D2' : (0.0, 1.5),
                'D2mod' : (0.0, 2.5),
                },
            }


        # Define histograms.
        variables =  ['pt'] # ['tau21', 'tau21mod', 'pt']

        histograms = { var : [TH1F('h_%s_%d' % (var, ibin), "", 50, 0, 200) for ibin in range(len(binsdict[var]))] for var in variables }


        # Filling histograms.
        for ievent in range(Nevents):
            for histvar, hists in histograms.iteritems():
                bins = binsdict[histvar]
                for ibin, bin in enumerate(bins):

                    # -- Has to be within bin.
                    if not(bin[0] <= values[histvar][ievent] < bin[1]): continue

                    # -- Has to pass all cuts.
                    if False in [p[0] <= values[k][ievent] < p[1] for k, p in cuts[histvar].iteritems()]: continue

                    # -- Fill.
                    hists[ibin].Fill( values['m'][ievent], values['weight'][ievent] )

                    pass
                pass
            pass

        
        # Drawing histograms.
        print " == Drawing histograms."
        log = True
        padding = 1.7

        c = TCanvas('c', "", 600, 600)
        c.SetLogy(log)
        
        for histvar, hists in histograms.iteritems():
            for h in hists:
                if h.Integral() == 0.: continue
                h.Scale(1./h.Integral())
                pass

            (ymin, ymax) = getPlotMinMax(hists, log, padding, ymin = (1e-03 if histvar == 'pt' else None))
            
            for ibin, h in enumerate(hists):
                h.GetYaxis().SetRangeUser(ymin, ymax)
                h.SetLineColor((kRed if ibin < 5 else kBlue) + (ibin % 5))
                h.GetXaxis().SetTitle('Jet mass [GeV]')
                h.GetYaxis().SetTitle('Jets (normalised)')
                
                h.Draw('HIST ' + ('' if ibin == 0 else 'SAME'))
                pass



            drawText([ "#sqrt{s} = 13 TeV",
                       "Inclusive #gamma events",
                       "Trimmed anti-k_{t}^{R=1.0}",
                       "Req. 1 #gamma with p_{T} > 155 GeV",                       
                       ]
                     + ([] if histvar == 'pt' else ["Jet p_{T} > 250 GeV"])
                     + ["%s #in  [%.1f, %.1f] %s" % (displayName(cutvar), p[0], p[1], displayUnit(cutvar)) for cutvar, p in cuts[histvar].iteritems()]
                     )
            
            # -- Legend.
            legend = drawLegend(hists,
                                ["%s #in  [%.0f, %.0f) %s" % (displayName(histvar), p[0], p[1], displayUnit(histvar)) for p in binsdict[histvar]],
                                'L'
                                )
            
            legend.SetTextSize(0.033)
            
            # -- Show.
            gPad.Update()
            wait()

            pass

        pass


# Main function call.
if __name__ == '__main__':
    main()
    pass
