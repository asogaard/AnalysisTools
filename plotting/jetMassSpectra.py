# Dear emacs, this is -*- python -*-
import sys

from snippets.style     import *
from snippets.functions import *

from ROOT  import *
from array import array

import math
import itertools


def displayName (var):
    if   var == "tau21":              return "#tau_{21}"
    elif var == "tau21_ut":           return "#tau_{21,untrimmed}"
    elif var == "D2":                 return "D_{2}"
    elif var == "D2mod":              return "#tilde{D}_{2}"
    elif var == "pt":                 return "p_{T}"
    elif var == "m":                  return "M"
    elif var == "rho":                return "#rho"
    elif var == "rho_ut":             return "#rho_{untrimmed}"
    elif var == "rhoPrime":           return "#rho'"
    elif var == "rhoPrime_ut":        return "#rho'_{untrimmed}"
    elif var == "rhoDDT":             return "#rho^{DDT}"
    elif var == "rhoDDT_ut":          return "#rho^{DDT}_{untrimmed}"
    elif var == "tau21_mod_rhoPrime": return "#tilde{#tau}_{21}" # '
    elif var == "tau21_mod_rhoDDT":   return "#tau_{21}^{DDT}"
    return var

def displayUnit (var):
    if   var == 'pt': return 'GeV'
    elif var == 'm':  return 'GeV'
    return ''

def rho (m, pt):
    if m  <= 0: return -1E+10
    if pt <= 0: return +1E+10
    return math.log(pow(m, 2.0) / pow(pt, 2.0))

def rhoPrime (m, pt):
    if m  <= 0: return -1E+10
    if pt <= 0: return +1E+10
    return math.log(pow(m, 2.0) / pow(pt, 1.4))

def rhoDDT (m, pt):
    if m  <= 0: return -1E+10
    if pt <= 0: return +1E+10
    return math.log(pow(m, 2.0) / pow(pt, 1.0))


# Main function.
# ----------------------------------------------------------------------------------------------------
def main ():
    
    # Initial setup.
    xsec = loadXsec('../share/weightsMC.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get.
    treename = 'BoostedJetISR/EventSelection/{0}/{1}/Precut'
    prefix   = ''
    getvars  = ['pt', 'm', 'CutVariable', '_rho_']

    # Load data.
    values = { 'Nominal'  : {},
               'rhoPrime' : {},
               'rhoDDT'   : {},
               }
    Nevents = dict()
    for var in ['rhoPrime', 'rhoDDT', 'Nominal']:
        getvars[3] = var
        print treename.format(var, 'tau21' + ('_mod_%s' % var if 'rho' in var else ''))
        values[var] = loadData(paths, treename.format(var, 'tau21' + ('_mod_%s' % var if 'rho' in var else '')), getvars, prefix, xsec, ignore = (lambda DSID: 305367 <= DSID <= 305374 ))

        if 'rho' in var:
            values[var]['tau21_mod_%s' % var] = values[var]['CutVariable']
        else:
            values[var]['tau21'] = values[var]['CutVariable']
            pass

        if not values[var]:
            print "WARNING: No value were loaded."
            return

        Nevents[var] = len(values[var][getvars[0]])
        pass


    # ==============================================================
    # Plot jet mass spectra 
    # --------------------------------------------------------------

    if True:
        
        # Define cut binning.
        binsdict = {
            'tau21': [
                #(0.1, 0.2),
                #(0.2, 0.3),
                #(0.3, 0.4),
                #(0.4, 0.5),
                #(0.5, 0.6),
                #(0.6, 0.7),
                #(0.7, 0.8),
                #(0.8, 0.9),
                (0.0, 0.4),
                (0.4, 1.5),
                ],
            
            'tau21_mod_rhoPrime': [
                #(0.1, 0.2),
                #(0.2, 0.3),
                #(0.3, 0.4),
                #(0.4, 0.5),
                #(0.5, 0.6),
                #(0.6, 0.7),
                #(0.7, 0.8),
                #(0.8, 0.9),
                (0.0, 0.4),
                (0.4, 1.5),
                ],

            'tau21_mod_rhoDDT': [
                #(0.1, 0.2),
                #(0.2, 0.3),
                #(0.3, 0.4),
                #(0.4, 0.5),
                #(0.5, 0.6),
                #(0.6, 0.7),
                #(0.7, 0.8),
                #(0.8, 0.9),
                (0.0, 0.4),
                (0.4, 1.5),
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
                ( 150.,  200.),
                ( 200.,  250.),
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
            'tau21': {
                },
            'tau21_mod_rhoPrime': {
                'rhoPrime' : (-1.5, 9999.),
                },
            'tau21_mod_rhoDDT': {
                'rhoDDT' : (1.0, 9999.),
                },
            'tau21mod': {
                'rhoPrime' : (-1.0, 2.0),
                },
            'D2'   : {},
            'D2mod': {
                'rho' : (DDTmin, DDTmax),
                },
            'pt'   : {
                'tau21_mod_rhoPrime': (0.0, 0.6),
                'rhoPrime':          (-1.5, 8.0),
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
        #variables =  ['pt'] # ['tau21', 'tau21mod', 'pt']
        variables =  ['tau21', 'tau21_mod_rhoPrime', 'tau21_mod_rhoDDT']

        # Loop correction (rho) variable
        for correctVar in ['Nominal', 'rhoPrime', 'rhoDDT']:
            histograms = { var : [TH1F('h_%s_%d' % (var, ibin), "", 50, 0, 200) for ibin in range(len(binsdict[var]))] for var in ['tau21_mod_%s' % correctVar if 'rho' in correctVar else 'tau21']}# variables }
            
            print "Keys:"
            for key, _ in values[correctVar].iteritems():
                print " -- '%s'" % key
                pass

            # Filling histograms.
            for ievent in xrange(Nevents[correctVar]):
                for histvar, hists in histograms.iteritems():
                    bins = binsdict[histvar]
                    for ibin, bin in enumerate(bins):
                        
                        # -- Has to be within bin.
                        if not(bin[0] <= values[correctVar][histvar][ievent] < bin[1]): continue
                        
                        # -- Has to pass all cuts.
                        if False in [p[0] <= values[correctVar][k][ievent] < p[1] for k, p in cuts[histvar].iteritems()]: continue
                        
                        # -- Fill.
                        hists[ibin].Fill( values[correctVar]['m'][ievent], values[correctVar]['weight'][ievent] )
                        
                        pass
                    pass
                pass
            
            
            # Drawing histograms.
            print " == Drawing histograms."
            log = True
            padding = 1.5
            
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
                    #h.SetLineColor((kRed if ibin < 5 else kBlue) + (ibin % 5))
                    h.SetLineColor((kRed if ibin < 3 else kBlue) + (ibin % 3) * 2)
                    h.GetXaxis().SetTitle('Jet mass [GeV]')
                    h.GetYaxis().SetTitle('Jets (normalised)')
                    
                    h.Draw('HIST ' + ('' if ibin == 0 else 'SAME'))
                    pass
                
                
                
                def cutInterval(a,b,digits = 1):
                    s = '---'
                    if   a <= -9999.:
                        s = "< %.*f" % (digits, b)
                    elif b >= 9999.:
                        s = "> %.*f" % (digits, a)
                    else:
                        s = "#in  [%.*f, %.*f]" % (digits, a, digits, b)
                        pass
                    return s

                drawText([ "#sqrt{s} = 13 TeV",
                           "Inclusive #gamma events",
                           "Trimmed anti-k_{t}^{R=1.0}",
                           "Req. 1 #gamma with p_{T} > 155 GeV",                       
                           ]
                         + ([] if histvar == 'pt' else ["Jet p_{T} > 150 GeV"])
                         + ["Jet M < p_{T}  / 2"]
                         + ["%s %s %s" % (displayName(cutvar), cutInterval(p[0], p[1]), displayUnit(cutvar)) for cutvar, p in cuts[histvar].iteritems()]
                         )
                
                # -- Legend.
                legend = drawLegend(hists,
                                    ["[%.1f, %.1f] %s" % (p[0], p[1], displayUnit(histvar)) for p in binsdict[histvar]],
                                    'L',
                                    header = 'Jet %s slices:' % displayName(histvar),
                                    ymax = 0.835
                                    )
                
                legend.SetTextSize(0.033)
                
                # -- Show.
                gPad.Update()
                
                savename = "jetMassSpectrum__%s.pdf" % histvar
                print "savename: '%s'" % savename

                wait()
                c.SaveAs(savename)

                pass
            
            pass #end: loop correctVar
        pass
    pass
    
# Main function call.
if __name__ == '__main__':
    main()
    pass
