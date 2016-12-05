# Dear emacs, this is -*- python -*-
import sys

from AStyle     import *
from AFunctions import *

from ROOT  import *
from array import array

import math
import itertools
import time

def displayName (var):
    if   var == "pt": return "p_{T}"
    elif var == "m":  return "M"
    elif var == "rhoDDT":  return "#rho^{DDT}"
    return var

def displayUnit (var):
    if   var == 'pt': return 'GeV'
    elif var == 'm':  return 'GeV'
    return ''

# 2D interpolation
from scipy import interpolate
#import matplotlib.pyplot as plt


# Main function.
# ----------------------------------------------------------------------------------------------------
def main ():
 
    gROOT.ProcessLine(".L share/Loader.C+");
   
    # Initial setup.
    xsec = loadXsec('share/sampleInfo.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get. 
    treename = 'BoostedJetISR/EventSelection/{0}/tau21_mod_rhoDDT/Postcut'
    prefix   = 'leadingfatjet_'
    getvars  = ['pt', 'rhoDDT']

    # Load data.
    values = dict()
    values['Pass'] = loadDataFast(paths, treename.format('Pass'), getvars, prefix, xsec, ignore = (lambda DSID: 305367 <= DSID <= 305374 ))
    values['Fail'] = loadDataFast(paths, treename.format('Fail'), getvars, prefix, xsec, ignore = (lambda DSID: 305367 <= DSID <= 305374 ))

    if not values['Pass'] or not values['Fail']:
        print "WARNING: No values were loaded."
        return

    # Get number of events.
    Nevents = { cat : len(values[cat][getvars[0]]) for cat,_ in values.items() }

    # Rename variables.
    # ...
    
    # Plotting stuff.
    lines = [ "#sqrt{s} = 13 TeV",
              "Inclusive #gamma MC",
              "Trimmed anti-k_{t} jets",
              "Req. #geq 1 jet with p_{T} > 350 GeV",
              #"Trimmed anti-k_{t}^{R=1.0} jets",
              #"Req. 1 #gamma with p_{T} > 100 GeV",
              ]

    colours = [kViolet + 7, kAzure + 7, kTeal, kSpring - 2, kOrange - 3, kPink]


    # ==============================================================
    # Jet substructure pass/fail ratio map.
    # --------------------------------------------------------------

    if True:

        xvars   = ['rhoDDT']
        #yvars   = ['HLT_j380', 'HLT_j400']
        yvars   = ['pt']
        
        axislimits = {
            #'pt': (60, 200, 800),
            'pt':      (16, 400, 2000), # (16, ...
            'rhoDDT' : (12, 1, 7) # (12, ...
            }

        profiles = { 
            yvar : {
                xvar : TProfile2D('Npass/Nfail vs (%s, %s)' % (yvar, xvar), "",
                                  axislimits[xvar][0], axislimits[xvar][1], axislimits[xvar][2], 
                                  axislimits[yvar][0], axislimits[yvar][1], axislimits[yvar][2]
                                  ) for xvar in xvars
                } for yvar in yvars  }

        for cat in values.keys():
            for ievent in xrange(Nevents[cat]):
                for yvar, xvar in itertools.product(yvars, xvars):
                    profiles[yvar][xvar].Fill( values[cat][xvar]    [ievent],
                                               values[cat][yvar]    [ievent],
                                               1 if cat == 'Pass' else 0,
                                               values[cat]['weight'][ievent] )
                    pass # end: loop variables            
                pass # end: loop events
            pass # end: loop pass/fail

        
        c = TCanvas('c', "", 700, 600)
        c.SetRightMargin(0.18)

        for xvar, yvar in itertools.product(xvars, yvars):

            ps = profiles[yvar][xvar]

            ps.GetXaxis().SetTitle('%s%s' % (displayName(xvar), (' [%s]' % displayUnit(xvar) if displayUnit(xvar) else '')))
            ps.GetYaxis().SetTitle('%s%s' % (displayName(yvar), (' [%s]' % displayUnit(yvar) if displayUnit(yvar) else '')))
            ps.GetYaxis().SetTitleOffset(1.6)
            ps.GetZaxis().SetTitle('N_{pass}/N_{fail}')
            ps.GetZaxis().SetTitleOffset(1.3)
            ps.GetZaxis().SetRangeUser(0., 1.0)

            ps.Draw('COL Z')
            gPad.Update()
            #gPad.SaveAs('plotting/transferFactorMap.pdf')

            # Interpolation grid
            #x, y = np.mgrid[axislimits[xvar][1]:axislimits[xvar][2]:axislimits[xvar][0] * 1j, 
            #                axislimits[yvar][1]:axislimits[yvar][2]:axislimits[yvar][0] * 1j]

            xaxis = ps.GetXaxis()
            yaxis = ps.GetYaxis()

            nx = xaxis.GetNbins()
            ny = yaxis.GetNbins()

            x  = np.zeros((nx,ny))
            y  = np.zeros((nx,ny))
            z  = np.zeros((nx,ny))
            ze = np.ones ((nx,ny))
            for i,j in itertools.product(xrange(nx), xrange(ny)):
                x [i,j] = xaxis.GetBinCenter(i + 1)
                y [i,j] = yaxis.GetBinCenter(j + 1)
                z [i,j] = ps.GetBinContent(i + 1, j + 1)
                err = ps.GetBinError  (i + 1, j + 1)
                if err == 0:
                    err = 0.5
                    pass
                ze[i,j] = pow(err, -1.) / 20.
                pass


            tck = interpolate.bisplrep(x, y, z, w = ze.flatten(), s=float(nx*ny))

            print tck

            nx_new, ny_new = (500,500)

            xnew, ynew = np.mgrid[axislimits[xvar][1]:axislimits[xvar][2]:nx_new*1j,
                                  axislimits[yvar][1]:axislimits[yvar][2]:ny_new*1j]

            znew = interpolate.bisplev(xnew[:,0], ynew[0,:], tck)

            ps_new = TH2F('ps_new', "", 
                          nx_new, axislimits[xvar][1], axislimits[xvar][2],
                          ny_new, axislimits[yvar][1], axislimits[yvar][2])
            ps_new.GetXaxis().SetTitle('%s%s' % (displayName(xvar), (' [%s]' % displayUnit(xvar) if displayUnit(xvar) else '')))
            ps_new.GetYaxis().SetTitle('%s%s' % (displayName(yvar), (' [%s]' % displayUnit(yvar) if displayUnit(yvar) else '')))
            ps_new.GetYaxis().SetTitleOffset(1.6)
            ps_new.GetZaxis().SetTitle('N_{pass}/N_{fail} density')
            ps_new.GetZaxis().SetTitleOffset(1.3)
            
            for i,j in itertools.product(xrange(nx_new), xrange(ny_new)):
                ps_new.SetBinContent(i + 1, j + 1, znew[i,j])
                pass

            c1 = TCanvas('c1', "", 700, 600)
            c1.SetRightMargin(0.18)
            ps_new.GetZaxis().SetRangeUser(0, 1.)
            ps_new.Draw('COL Z')
            gPad.Update()
            #gPad.SaveAs('plotting/transferFactorDensity.pdf')


            dev = TH1F('dev', "", 25, -6., 4.)

            #dev.GetXaxis().SetTitle('Transfer factor pull')
            #dev.GetYaxis().SetTitle('Counts')

            for i,j in itertools.product(xrange(nx), xrange(ny)):
                xcoord = ps.GetXaxis().GetBinCenter(i + 1)
                ycoord = ps.GetYaxis().GetBinCenter(j + 1)
                val1 = ps.GetBinContent(i + 1, j + 1)
                val2 = interpolate.bisplev(xcoord, ycoord, tck)
                err  = ps.GetBinError(i + 1, j + 1)
                err = err if err > 0. else 0.5

                d = (val2 - val1) / err
                #dev.SetBinContent(i + 1, j + 1, d)

                dev.Fill(min(max(d,-4.9),4.9))
                pass


            # Draw.
            lines = [ "#sqrt{s} = 13 TeV",
                      "Inclusive #gamma MC",
                      "HLT_j380", 
                     "Trimmed anti-k_{t} jets",
                      "At least 1 jet with p_{T} > 400 GeV",
                      "Exactly 1 #gamma with p_{T} > 155 GeV",
                      "Jet p_{T} > 2 M",
                      "",
                      "Mean: %.3f #pm %.3f" % (dev.GetMean(), dev.GetMeanError()),
                      "RMS: %.3f" % (dev.GetRMS()),
                      ]
            legendOpts = LegendOptions(histograms = [], names = [], xmin = 0.58, ymax = 0.845)
            textOpts   = TextOptions(lines = lines)

            c2 = makePlot([dev],
                     legendOpts,
                     textOpts,
                     xtitle = 'Transfer factor pull',
                     ytitle = 'Counts',
                     colours = [kViolet + 7, kAzure + 7, kTeal, kSpring - 2, kOrange - 3, kPink],
                     )
            c2.cd()
            dev.Draw('HIST same')

            print "dev.Integral(): %f | Overflow: %f | Underflow: %f" % (dev.Integral(), dev.GetBinContent(0), dev.GetBinContent(20 + 1))
            print "dev.GetRMS(): %f (%f)" % (dev.GetRMS(), 0.845864)
            gPad.Update()
            #gPad.SaveAs('plotting/transferFactorPulls.pdf')

            wait()

            return
            points = np.zeros((nx*ny,2))
            values = np.zeros((nx*ny,))
            errors = np.zeros((nx*ny,))
            for i in xrange(nx):
                for j in xrange(ny):
                    points[i*ny + j,:] = np.array([xaxis.GetBinCenter(i + 1), yaxis.GetBinCenter(j + 1)]) 
                    values[i*ny + j] = ps.GetBinContent(i + 1, j + 1)
                    errors[i*ny + j] = ps.GetBinError  (i + 1, j + 1) 
                    pass
                pass

            points = np.array(points)
            values = np.array(values)
            errors = np.array(errors)

            grid_z = griddata(points, values, (grid_x, grid_y), method='cubic')


            print grid_z

            # -- Show.
            gPad.Update()
            wait()

            savename = "plot_transferFactorMap.pdf"
            print "Savename: '%s'" % savename
            #c.SaveAs(savename)
            
            pass # end: loop variables
        
        pass # end: transfer factor map

    return

if __name__ == '__main__':
    main()
    pass
