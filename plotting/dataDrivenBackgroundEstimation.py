# Dear emacs, this is -*- python -*-
import sys

from AStyle     import *
from AFunctions import *

from ROOT  import *
from array import array

import math
import itertools

def displayName (var):
    if   var == "tau21":              return "#tau_{21}"
    if   var == "tau21_ut":           return "#tau_{21,untrimmed}"
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
    treename = 'BoostedJetISR/EventSelection/{0}/tau21_mod_{0}/{1}'
    prefix   = ''
    getvars  = ['pt', 'rhoPrime']

    # Load data.
    values = { 'rhoPrime' : { 'Precut': {} , 'Postcut' : {} }, 
               'rhoDDT'   : { 'Precut': {} , 'Postcut' : {} },
               }
    Nevents = dict()
    for var, pos in itertools.product(['rhoPrime', 'rhoDDT'], ['Precut', 'Postcut']):
        getvars[1] = var
        values[var][pos] = loadData(paths, treename.format(var, pos), getvars, prefix, xsec, ignore = (lambda DSID: 305367 <= DSID <= 305374 ))

        if not values[var][pos]: 
            print "WARNING: No value were loaded."
            return

        if pos == 'Precut':
            Nevents[var] = len(values[var][pos])
            pass
        pass
    
    
    # ==============================================================
    # Plot efficiencies.
    # --------------------------------------------------------------

    if True:

        # Definitions
        axislimits = {
            'pt':       ( 0,   2000.),
            'rhoPrime': (-1.5,    4.0),
            'rhoDDT':   ( 1.0,    7.0),
            }

        nBins = 15

        axes = { 
            'pt':       array('d', [80, 150, 200, 250, 300, 400, 500, 700, 1000, 2000]),
            'rhoPrime': array('d', [-2.5] + [ axislimits['rhoPrime'][0] + (axislimits['rhoPrime'][1] - axislimits['rhoPrime'][0]) * i / float(nBins - 1) for i in range(nBins) ]),
            'rhoDDT':   array('d', [0.] + [ axislimits['rhoDDT'][0]   + (axislimits['rhoDDT'][1]   - axislimits['rhoDDT'][0])   * i / float(nBins - 1) for i in range(nBins) ]),
            }
        
        # Variables to to map
        xvars   = ['rhoPrime',  'rhoDDT']
        yvars   = ['pt']

        for xvar in xvars + yvars:
            print "===> %s" % xvar
            print axes[xvar]
            pass

        # Efficiency maps
        efficiencies = {
            yvar : {
                xvar : TH2F('Efficiency vs. (%s, %s)'     % (xvar, yvar), "",
                            len(axes[xvar]) - 1, axes[xvar], len(axes[yvar]) - 1, axes[yvar]) for xvar in xvars
                } for yvar in yvars  }
        
        passed = {
            yvar : {
                xvar : TH2F('Num. passed vs. (%s, %s)'     % (xvar, yvar), "",
                            len(axes[xvar]) - 1, axes[xvar], len(axes[yvar]) - 1, axes[yvar]) for xvar in xvars
                } for yvar in yvars  }

        total = {
            yvar : {
                xvar : TH2F('Num. total vs. (%s, %s)'     % (xvar, yvar), "",
                            len(axes[xvar]) - 1, axes[xvar], len(axes[yvar]) - 1, axes[yvar]) for xvar in xvars
                } for yvar in yvars  }

        # Loop events, fill standard profiles
        for yvar, xvar in itertools.product(yvars, xvars):
            # -- Pre-cut (total)
            for (x,y,w) in zip(values[xvar]['Precut'][xvar], 
                               values[xvar]['Precut'][yvar],
                               values[xvar]['Precut']['weight']):
                total[yvar][xvar].Fill(x, y, w)
                pass

            # -- Post-cut (passed)
            for (x,y, w) in zip(values[xvar]['Postcut'][xvar], 
                                values[xvar]['Postcut'][yvar],
                                values[xvar]['Postcut']['weight']):
                passed[yvar][xvar].Fill(x, y, w)
                pass

            # -- Efficiency
            efficiencies[yvar][xvar] = TEfficiency(passed[yvar][xvar], total[yvar][xvar])
            
            xtitle = displayName(xvar) + (' [%s]' % displayUnit(xvar) if displayUnit(xvar) else '')
            ytitle = displayName(yvar) + (' [%s]' % displayUnit(yvar) if displayUnit(yvar) else '')
            print "xtitle: '%s'" % xtitle
            print "ytitle: '%s'" % ytitle

            c = makePlot( [efficiencies[yvar][xvar]],
                          drawOpt = 'COL Z',
                          logy = True,
                          xtitle = xtitle,
                          ytitle = ytitle,
                          ztitle = 'Background efficiency',
                          xlines = [-1.5 if xvar == 'rhoPrime' else 1.0],
                          ylines = [150.],
                          )

            savename = "efficiencyMap_%s_vs_%s.pdf" % (yvar, xvar)
            print "savename: '%s'" % savename
            wait()
            c.SaveAs(savename)

            pass

           

        pass # end section: efficiency maps

    # ==============================================================
            
    return 0



if __name__ == '__main__':
    main()
    pass
