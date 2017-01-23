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
    elif var == "tau21_mod_rhoPrime": return "#tau_{21}'"
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
    #xsec = loadXsec('../share/weightsMC.csv')
    xsec = loadXsec('../share/sampleInfo.csv')
    
    # Get list of file paths to plot from commandline arguments.
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get. 
    #treename = 'BoostedJetISR/EventSelection/Nominal/NumPhotons/Postcut'
    treename = 'BoostedJetISR/Fatjets/Nominal/dPhi/Postcut'
    #prefix   = 'plot_leadingFatjet_'
    prefix   = 'plot_fatjet_'
    getvars  = ['m', 'pt', 'pt_ut', 'tau21', 'tau21_ut']

    # Load data.
    values = loadData(paths, treename, getvars, prefix, xsec, 
                      #ignore = (lambda DSID: 305367 <= DSID <= 305374 )
                      keeponly= (lambda DSID: 361039 <= DSID <= 361062 )
                      )

    if not values: 
        print "WARNING: No value were loaded."
        return

    Nevents = len(values[getvars[0]])


    # -- Compute trimmed rho variables
    values['rho']         = map(lambda (m,pt): rho     (m,pt), zip(values['m'], values['pt']))
    values['rhoPrime']    = map(lambda (m,pt): rhoPrime(m,pt), zip(values['m'], values['pt']))
    values['rhoDDT']      = map(lambda (m,pt): rhoDDT  (m,pt), zip(values['m'], values['pt']))

    # -- Compute untrimmed rho variables
    values['rho_ut']      = map(lambda (m,pt): rho     (m,pt), zip(values['m'], values['pt_ut']))
    values['rhoPrime_ut'] = map(lambda (m,pt): rhoPrime(m,pt), zip(values['m'], values['pt_ut']))
    values['rhoDDT_ut']   = map(lambda (m,pt): rhoDDT  (m,pt), zip(values['m'], values['pt_ut']))



    # ==============================================================
    # Improved bias correction.
    # --------------------------------------------------------------

    if False:

        pTcut = 250.
        
        #xvars   = ['rho', 'rhoPrime', 'rhoDDT']
        xvars   = ['rho',    'rhoPrime',    'rhoDDT', 
                   'rho_ut', 'rhoPrime_ut', 'rhoDDT_ut']
        yvars   = ['tau21',
                   'tau21_ut']
        
        xaxislimits = {
            'rho':         (-8, 0),
            'rho_ut':      (-8, 0),
            'rhoPrime':    (-4, 4),
            'rhoPrime_ut': (-4, 4),
            'rhoDDT':      (-2, 6),
            'rhoDDT_ut':   (-2, 6),
            }

        profiles = { 
            yvar : {
                xvar : TProfile('<%s> vs. %s'     % (yvar, xvar), "", 50, xaxislimits[xvar][0], xaxislimits[xvar][1], -100, 200) for xvar in xvars
                } for yvar in yvars  }

        profiles_mod = { 
            yvar : {
                xvar : TProfile('<%s> vs. %s mod' % (yvar, xvar), "", 50, xaxislimits[xvar][0], xaxislimits[xvar][1], -100, 200) for xvar in xvars
                } for yvar in yvars  }

        fit_limits = {
            'rho':         (-4.0, -2.0),
            'rho_ut':      (-4.0, -2.0),
            'rhoPrime':    (-1.0,  2.0),
            'rhoPrime_ut': (-1.0,  2.0),
            'rhoDDT':      ( 1.5,  3.5),
            'rhoDDT_ut':   ( 1.5,  3.5),
            }


        for ievent in xrange(Nevents):
            for yvar, xvar in itertools.product(yvars, xvars):
                if values['pt'][ievent] < pTcut: continue
                if values['m'][ievent] > values['pt'][ievent] / 2.: continue # Boosted regime
                profiles[yvar][xvar].Fill( values[xvar]    [ievent],
                                           values[yvar]    [ievent],
                                           values['weight'][ievent] )
                pass # end: loop variables            
            pass # end: loop events

        
        c = TCanvas('c', "", 600, 600)

        for yvar, xvar in itertools.product(yvars, xvars):
            print " == <%s> vs. %s" % (yvar, xvar)
            fit    = TF1('fit_%s_%s'    % (yvar, xvar), 'pol1', fit_limits[xvar][0], fit_limits[xvar][1])
            fitmod = TF1('fitmod_%s_%s' % (yvar, xvar), 'pol1', fit_limits[xvar][0], fit_limits[xvar][1])
            fit   .SetLineColor(kRed  + 3)
            fitmod.SetLineColor(kBlue + 3)

            profiles[yvar][xvar].Fit(fit, 'R')

            y  = values[yvar]
            x  = values[xvar]
            x1 = fit_limits[xvar][0]
            #values['%s_mod_%s' % (yvar, xvar)] = array('d', [ y[i] + (fit.Eval(x1) - fit.Eval(x[i])) for i in xrange(Nevents)])
            values['%s_mod_%s' % (yvar, xvar)] = array('d', [ y + (fit.Eval(x1) - fit.Eval(x)) for x,y in zip(values[xvar], values[yvar])])

            for ievent in xrange(Nevents):
                if values['pt'][ievent] < pTcut: continue
                if values['m'][ievent] > values['pt'][ievent] / 2.: continue
                profiles_mod[yvar][xvar].Fill( values[xvar][ievent],
                                               values['%s_mod_%s' % (yvar, xvar)][ievent],
                                               values['weight'][ievent]
                                               )
                pass

            profiles_mod[yvar][xvar].Fit(fitmod, 'R')

            pass

        for yvar, xvar in itertools.product(yvars, xvars):

            fit = TF1('fit', 'pol1', fit_limits[xvar][0], fit_limits[xvar][1])
            fit.SetLineColor(kRed + 3)

            ps    = profiles    [yvar][xvar]
            psmod = profiles_mod[yvar][xvar]

            ps   .SetLineColor  (kRed + 1)
            ps   .SetMarkerColor(kRed + 1)
            psmod.SetLineColor  (kBlue + 1)
            psmod.SetMarkerColor(kBlue + 1)

            ps   .Draw()
            psmod.Draw('SAME')

            padding = 1.7
            ymin, ymax = getPlotMinMax( [ps], False, padding = padding, ymin = 0. )
            
            # -- Draw.
            ps.GetYaxis().SetRangeUser(ymin, ymax)
            ps.GetXaxis().SetTitle( "%s %s" % (displayName(xvar), displayUnit(xvar) ) )
            ps.GetYaxis().SetTitle( '#LT%s#GT' % displayName(yvar) )
            
            # -- Text.
            drawText([ "#sqrt{s} = 13 TeV",
                       "Inclusive #gamma events",
                       "Trimmed anti-k_{t}^{R=1.0}",
                       "Req. 1 #gamma with p_{T} > 155 GeV",
                       "Jet p_{T} > %d GeV" % int(pTcut),
                       #"Jet M > 20 GeV",
                       #"Jet M < p_{T} / 2 (boosted regime)",
                       ])
            
            # -- Legend.
            legend = drawLegend([ps, psmod],
                                ['Uncorrected', 'Corrected'],
                                'LP',
                                width = 0.30
                                )
            
            legend.SetTextSize(0.033)
            
            # -- Show.
            gPad.Update()
            wait()

            savename = "profile_%s_vs_%s.pdf" % (yvar, xvar)
            print "Savename: '%s'" % savename
            #c.SaveAs(savename)
            
            
            pass # end: loop variables
        
        pass


    # ==============================================================
    # Improved substructure profile plots.
    # --------------------------------------------------------------

    if True:

        #values['rhoPrime'] = [0 for _ in xrange(Nevents)]

        #for ievent in xrange(Nevents):
            #values['rhoPrime'][ievent] = rho(values['m'][ievent], values['pt'][ievent])
            #pass

        cutsdict = {
            #'rho' : [
            #    (-6, -5),
            #    (-5, -4),
            #    (-4, -3),
            #    (-3, -2),
            #    (-2, -1),
            #    ]
            'pt': [
                ( 150,  200),
                #( 200,  250),
                #( 250,  300),
                #( 300,  400),
                ( 400,  500),
                #( 600,  750),
                #( 750,  1000),
                ( 900, 1100),
                #(1100, 1500)
                ]
            }



        #xvars   = ['rho', 'rhoPrime', 'm'] # ['rho', 'rhoPrime', 'rhoDDT']
        xvars   = ['rho',    'rhoPrime',    'rhoDDT',
                   'rho_ut', 'rhoPrime_ut', 'rhoDDT_ut']
        #yvars   = ['tau21', 'tau21_mod_rhoPrime', 'D2', 'D2mod'] # ['tau21', 'D2'] # , 'D2mod']
        yvars   = ['tau21', 'tau21_ut']
        cutvars = [p[0] for p in cutsdict.items()]
        
        profiles = { yvar : { xvar : { cutvar : [] for cutvar in cutvars } for xvar in xvars } for yvar in yvars  }

        xaxislimits = {
            'pt':          ( 0, 2000),
            'm':           ( 0,  300),
            'rho':         (-8,    0),
            'rho_ut':      (-8,    0),
            'rhoPrime':    (-4,    4),
            'rhoPrime_ut': (-4,    4),
            'rhoDDT':      (-2,    6),
            'rhoDDT_ut':   (-2,    6),
            }

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
        # ^^^ TEST: Use option 's' to display RMS rather than error on the mean.

        def sortBy (d, sortKey):
            ''' Function for sorting the values in the dictionary 'd' (assumed to be lists of equal length) by the values on key 'sortKey'. '''
            keys = [key for key in d]
            if not sortKey in keys:
                print "WARNING: Key '%s' is not present in dictionary to be sorted."
                return
            
            idx = keys.index(sortKey)
            d = { k: array('d', zip(*sorted(zip(*[d[key] for key in keys]), key = lambda tup: tup[idx]))[i]) for i,k in enumerate(keys)}
            return d

        for ievent in xrange(Nevents):
            #if values['m'] [ievent] <  20.: continue # TEMP!!
            if values['m'][ievent] > values['pt'][ievent] / 2.: continue # Boosted regime
            for yvar, xvar, cutvar in itertools.product(yvars, xvars, cutvars):
                if yvar == yvars[0]:
                    density[xvar].Fill( values[xvar]    [ievent],
                                        values['weight'][ievent] )
                    pass

                #'''
                if yvar == 'tau21_mod_rhoPrime':
                    if not(fit_limits['rhoPrime'][0] < values['rhoPrime'][ievent] < fit_limits['rhoPrime'][1]):
                        continue
                    pass
                if yvar == 'D2mod':
                    if not(-6 < values['rho'][ievent] < -1):
                        continue
                    pass
                #'''
                for icut, cut in enumerate(cutsdict[cutvar]):
                    if not(cut[0] <= values[cutvar][ievent] < cut[1]): continue
                    ps = profiles[yvar][xvar][cutvar]
                    ps[icut].Fill( values[xvar]    [ievent],
                                   values[yvar]    [ievent],
                                   values['weight'][ievent] )
                    pass # end: loop cuts
                pass # end: loop variables            
            pass # end: loop events

        
        c = TCanvas('c', "", 600, 600)

        for yvar, xvar, cutvar in itertools.product(yvars, xvars, cutvars):

            ps   = profiles[yvar][xvar][cutvar]
            cuts = cutsdict[cutvar]

            padding = 1.7
            ymin, ymax = getPlotMinMax( ps, False, padding = padding, ymin = 0. )
            ymax = min(ymax, 25)

            # -- Density.
            density[xvar].SetLineColor(kGray + 2)
            density[xvar].SetFillColor(kGray)
            density[xvar].Scale( ymax / getMaximum(density[xvar]) * 0.2 )
            density[xvar].Smooth(4)

            density[xvar].Draw('HIST LC')

            # -- Draw.
            for icut, _ in enumerate(cuts):
                col = (kRed if (icut * 2) < 5 else kBlue) + ((icut * 2) % 5)
                ps[icut].SetLineColor  (col)
                ps[icut].SetMarkerColor(col)
                ps[icut].Draw('SAME' if icut >= 0 else '')
                pass
            
            # -- Axis titles.            
            for p in ps + [density[xvar]]:
                p.GetYaxis().SetRangeUser(ymin, ymax)
                p.GetXaxis().SetTitle( displayName(xvar) )
                p.GetYaxis().SetTitle( '#LT%s#GT' % displayName(yvar) )
                pass

            ps[0].Draw('AXIS SAME')
            
            # -- Text.
            drawText([ "#sqrt{s} = 13 TeV",
                       "Inclusive #gamma events",
                       "Trimmed anti-k_{t}^{R=1.0}",
                       "Req. 1 #gamma with p_{T} > 155 GeV",
                       #"Jet M > 20 GeV",
                       "Jet M < p_{T} R / 2",
                       ("#rho' #in  [%3.1f, %3.1f]" % (fit_limits['rhoPrime'][0], fit_limits['rhoPrime'][1]) if yvar == 'tau21_mod_rhoPrime' else ""),
                       ("#rho #in  [%3.1f, %3.1f]" % (-6, -1) if yvar == 'D2mod' else ""),
                       #"Jet %s slices plotted" % displayName(cutvar)
                       ])
            
            cutText = TLatex()
            cutText.DrawLatexNDC( 0.62,
                                  0.76 + 0.05,
                                  "Jet %s slices:" % displayName(cutvar) )

            # -- Legend.
            legend = drawLegend(ps,
                                ['[%d, %d] %s' % (cut[0], cut[1], displayUnit(cutvar)) for cut in cuts],
                                ['LP' for _ in cuts],
                                ymax = 0.74 + 0.05)
            
            legend.SetTextSize(0.033)
            
            # -- Lines.
            line = TLine()
            line.SetLineColor(kGray + 1)
            line.SetLineWidth(1)
            
            text = TLatex()
            text.SetTextSize(0.023)
            text.SetTextColor(kGray + 2)
            
            ydraw = ymax * 0.40
            
            xaxis = ps[0].GetXaxis
            xmin, xmax = xaxis().GetXmin(), xaxis().GetXmax()
            
            offset = (0.005 * (xmax - xmin), 0.005 * (ymax - ymin))
            
            for cut in cuts[:3]: #(reversed(cuts) if yvar == 'tau21' else cuts):
                clip = [False, False]


                Mmin, Mmax = (20., 200.)
                if xvar == 'rho':
                    x1 = rho(Mmin, cut[0]) 
                    x2 = rho(Mmax, cut[0]) 
                elif xvar == 'rhoDDT':
                    x1 = rhoDDT(Mmin, cut[0]) 
                    x2 = rhoDDT(Mmax, cut[0])                     
                elif xvar == 'rhoPrime':
                    x1 = rhoPrime(Mmin, cut[0]) 
                    x2 = rhoPrime(Mmax, cut[0])                     
                else:
                    continue

                xm = (x1 + x2)/2.

                if x1 < xmin:
                    x1 = xmin
                    clip[0] = True
                    pass

                if x2 > xmax:
                    x2 = xmax
                    clip[1] = True
                    pass

                line.DrawLine(x1, ydraw, x2, ydraw)
                if not clip[0]:
                    line.DrawLine(x1, ydraw + offset[1], x1, ydraw - offset[1])
                    pass
                if not clip[1]:
                    line.DrawLine(x2, ydraw + offset[1], x2, ydraw - offset[1])
                    pass

                text.SetTextFont(42)
                if not clip[0]:
                    text.SetTextAlign(11)
                    text.DrawLatex(x1, ydraw + 2 * offset[1], "M_{J} = %d GeV" % Mmin)
                    pass
                
                if not clip[1]:
                    text.SetTextAlign(31)
                    text.DrawLatex(x2, ydraw + offset[1], "%d GeV" % Mmax)
                    pass
                
                text.SetTextFont(52)
                #text.SetTextAlign(23)
                text.SetTextAlign(21)
                if clip[0]:
                    text.SetTextAlign(11)
                    xm = xmin + (xmax - xmin) * 0.04
                    pass

                text.DrawLatex(xm, ydraw + 2 * offset[1], "(%s = %d %s)" % (displayName(cutvar), cut[0], displayUnit(cutvar)))
                
                ydraw += 0.06 * (ymax - ymin)
                pass
            
            # -- Show.
            gPad.Update()
            wait()

            pass

        pass


    # ==============================================================
    # Kolmogorov-Smirnov test of different substructure cuts.
    # --------------------------------------------------------------

    if True:

        #cutvar = 'tau21_mod_rhoPrime'
        #cutvar = 'D2mod'
        #cutvar = 'D2'
        cutvar = 'tau21'
        if   cutvar == 'D2':
            cutmin = 0.
            cutmax = 5.
        elif cutvar == 'D2mod':
            cutmin = 0.
            cutmax = 7.
        else:
            cutmin = 0.
            cutmax = 1.0
            pass         
        
        Ncuts = 10
        cuts = range(Ncuts + 1)
        cuts = map(lambda v : v / float(Ncuts) * cutmax + cutmin, cuts)

        cuts   = cuts[1:-1]
        Ncuts -= 2
        width = (cuts[1] - cuts[0]) / 2.

        ''' >>> Test '''
        cuts = [0.40]
        Ncuts = 1
        width = 1.
        ''' <<< Test '''

        h_KS = TH1F('h_KS', "", Ncuts + 1, cuts[0] - width, cuts[-1] + width)

        c = TCanvas('c', "", 600, 600)

        for icut, cut in enumerate(cuts):

            print "Cut: %s < %4.2f" % (cutvar, cut)

            # Clear
            h_pass = TH1F('h_pass', "", 50, 0, 200)
            h_fail = TH1F('h_fail', "", 50, 0, 200)

            h_pass.SetLineColor  (kRed)
            h_pass.SetMarkerColor(kRed)
            h_fail.SetLineColor  (kBlue)
            h_fail.SetMarkerColor(kBlue)


            # Fill
            print "rhoPrime range: [%f, %f]" % (fit_limits['rhoPrime'][0], fit_limits['rhoPrime'][1])
            for ievent in xrange(Nevents):
                if cutvar == 'D2mod':
                    if not (-6 < values['rho'][ievent] < -1):
                        continue
                    pass
                elif cutvar == 'tau21_mod_rhoPrime': 
                    if not (fit_limits['rhoPrime'][0] < values['rhoPrime'][ievent] < fit_limits['rhoPrime'][1]):
                        continue
                    pass
                if values[cutvar][ievent] < cut:
                    h_pass.Fill( values['m']     [ievent],
                                 values['weight'][ievent] )
                else:
                    h_fail.Fill( values['m']     [ievent],
                                 values['weight'][ievent] )
                pass

            # Check
            if h_pass.Integral() == 0:
                print " -- No events passed."
                continue

            if h_fail.Integral() == 0:
                print " -- No events failed."
                continue
            
            # Compute
            h_pass.Scale( 1./h_pass.Integral() )
            h_fail.Scale( 1./h_fail.Integral() )
            
            #ymin, ymax = getPlotMinMax([h_pass, h_fail], False, padding = 0.2)

            h_pass.GetYaxis().SetRangeUser(0, 0.30) #ymax)
            h_pass.GetXaxis().SetTitle('Jet mass [GeV]')
            h_pass.GetYaxis().SetTitle('Number of jet (normalised)')


            h_pass.Draw('HIST')
            h_fail.Draw('HIST SAME')

            drawLegend([h_pass, h_fail], 
                       ['%s < %4.2f' % (displayName(cutvar), cut), '%s > %4.2f' % (displayName(cutvar), cut)], 
                       ['L', 'L'], 
                       width = 0.25)

            drawText(["#sqrt{s} = 13 TeV",
                      "Inclusive #gamma events",
                      "Trimmed anti-k_{t}^{R=1.0}",
                      "Req. 1 #gamma with p_{T} > 155 GeV",
                      "Req. #geq 1 jet with p_{T} > 150 GeV",
                      #"Jet M > 20 GeV",
                      "Jet M < p_{T} R / 2",
                      ] + 
                     (["#rho' #in  [-1, 2]"] if cutvar == 'tau21_mod_rhoPrime' else []) +
                     (["#rho #in  [-6, -1]"] if cutvar == 'D2mod' else [])
                     )

            p = h_pass.KolmogorovTest(h_fail)
            print " -- %d: %5.3f" % (icut, p)
            wait()
            
            h_KS.SetBinContent( icut + 1, p)

            # Done
            del h_pass
            del h_fail

            pass

        h_KS.Draw()
        gPad.Update()
        wait()

        pass





    return




    # ==============================================================
    # Compute modified D2 variable.
    # --------------------------------------------------------------

    DDTmin = -6.0
    DDTmax = -1.0

    profile        = TProfile("<D2> vs.     rho",    "", 50, -14, 0, -1000, 2000.)
    profilemod     = TProfile("<D2mod>     vs. rho", "", 50, -14, 0, -1000, 2000.)
    profilemodthis = TProfile("<D2modthis> vs. rho", "", 50, -14, 0, -1000, 2000.)

    for ievent in xrange(Nevents):
        profile.Fill( vals['rho'][ievent], 
                      vals['D2'][ievent],
                      vals['weight'][ievent] )
        pass

    fit    = TF1("fit",    'pol1', DDTmin, DDTmax)
    fitmod = TF1("fitmod", 'pol1', DDTmin, DDTmax)
    profile.Fit("fit", 'R0')

    vals['D2modthis'] = [vals['D2'][i] + (fit.Eval(DDTmin) - fit.Eval(vals['rho'][i])) for i in xrange(Nevents)]

    for ievent in xrange(Nevents):
        profilemod.Fill( vals['rho'][ievent], 
                         vals['D2mod'][ievent],
                         vals['weight'][ievent] )

        profilemodthis.Fill( vals['rho'][ievent], 
                             vals['D2modthis'][ievent],
                             vals['weight'][ievent] )
        pass

    profilemodthis.Fit("fitmod", 'R0')

    profilemod.SetLineColor(kBlue)
    profilemod.SetMarkerColor(kBlue)
    profilemodthis.SetLineColor(kRed)
    profilemodthis.SetMarkerColor(kRed)

    profile.GetYaxis().SetRangeUser(0., 20.)
    profile.GetXaxis().SetTitle('#rho')
    profile.GetYaxis().SetTitle('#LTD_{2}#GT')

    profile.Draw()
    profilemod.Draw('same')
    profilemodthis.Draw('same')

    fit.SetLineColor(kGray + 1)
    fit.Draw('same')
    fitmod.SetLineColor(kRed + 2)
    fitmod.Draw('same')

    wait()
    #return

    # ==============================================================


    # Defining histograms.
    print " == Defining histograms."

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
            ( 800., 1200.),
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

    cuts = {
        'tau21': {},
        'D2'   : {},
        'D2mod': { 
            'rho' : (DDTmin, DDTmax),
            },
        'pt'   : {
            #'tau21' : (0.0, 0.38),
            #'D2' : (0.0, 1.5),
            'D2mod' : (0.0, 2.3),
            'rho' : (-6, -1),
            },
        'rho'  : {
            #'tau21' : (0.0, 0.38),
            #'D2' : (0.0, 1.5),
            'D2mod' : (0.0, 2.5),
            },
        }

    histograms = dict()
    histograms['tau21']  = [ TH1F('h_tau21_%d' % ibin, "", 50, 0, 200.) for ibin in range(len(binsdict['tau21'])) ]
    histograms['D2']     = [ TH1F('h_D2_%d'    % ibin, "", 50, 0, 200.) for ibin in range(len(binsdict['D2'])) ]
    histograms['D2mod']  = [ TH1F('h_D2mod_%d' % ibin, "", 50, 0, 200.) for ibin in range(len(binsdict['D2mod'])) ]
    histograms['pt']     = [ TH1F('h_pt_%d'    % ibin, "", 50, 0, 200.) for ibin in range(len(binsdict['pt'])) ]
    histograms['rho']    = [ TH1F('h_rho_%d'   % ibin, "", 50, 0, 200.) for ibin in range(len(binsdict['rho'])) ]

    # Filling histograms.
    print " == Filling %d histograms from data arrays." % sum([len(hists) for _, hists in histograms.items()])

    h_d2mod = TH1F('h_d2mod', "", 50, -2, 7)
    h_d2mod_pass1 = TH1F('h_d2mod_pass1', "", 50, -2, 7)
    h_d2mod_pass2 = TH1F('h_d2mod_pass2', "", 50, -2, 7)
    h_d2mod_pass1.SetLineColor  (kRed)
    h_d2mod_pass1.SetMarkerColor(kRed)
    h_d2mod_pass2.SetLineColor  (kBlue)
    h_d2mod_pass2.SetMarkerColor(kBlue)
    for ievent in xrange(Nevents):
        h_d2mod.Fill( vals['D2mod'][ievent], vals['weight'][ievent] )
        h_d2mod_pass1.Fill( vals['D2mod'][ievent], vals['weight'][ievent] )
        if (vals['rho'][ievent] < -6.) or (vals['rho'][ievent] > -1.): continue
        h_d2mod_pass2.Fill( vals['D2mod'][ievent], vals['weight'][ievent] )
        pass
    #h_d2mod.GetYaxis().SetRangeUser(4e-02, 4e+03)
    h_d2mod.Draw()
    h_d2mod_pass1.Draw('same')
    h_d2mod_pass2.Draw('same')
    gPad.SetLogy()
    gPad.Update()
    wait()

    for ievent in xrange(Nevents):
        for histvar, hists in histograms.iteritems():
            bins = binsdict[histvar]
            for ibin, bin in enumerate(bins):
                # -- Has to be within bin.
                if not(bin[0] <= vals[histvar][ievent] < bin[1]): continue
                # -- Has to pass all cuts.
                if False in [p[0] <= vals[k][ievent] < p[1] for k, p in cuts[histvar].iteritems()]: continue
                # -- Fill.
                hists[ibin].Fill( vals['m'][ievent], vals['weight'][ievent] )
                pass
            pass
        pass

    log     = True
    padding = 1.0

    # Drawing histograms.
    print " == Drawing histograms."
    c = TCanvas('c', "", 600, 600)
    c.SetLogy(log)

    for histvar, hists in histograms.iteritems():
        print " ---- %s" % histvar
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
        
        bins = binsdict[histvar]

        Nentries = len(hists)
        ymax = 0.90
        xmax = 0.90
        xmin = 0.65
        ymin = ymax - Nentries * fontSizeM * 1.2
        
        legend = TLegend(xmin, ymin, xmax, ymax)
        decimals = (1 if histvar in ['tau21', 'D2', 'D2mod'] else 0)
        for ibin, h in enumerate(hists):
            displayvar = (displayName[histvar] if histvar in displayName else histvar)
            unit       = (displayUnit[histvar] if histvar in displayUnit else '')
            legend.AddEntry(h, "%s #in [%3.*f, %3.*f) %s" % ( displayvar, 
                                                              int(decimals), 
                                                              bins[ibin][0], 
                                                              int(decimals), 
                                                              bins[ibin][1], 
                                                              unit ),
                            'L')
            pass
        legend.Draw()
        
        drawText(lines = ["#sqrt{s} = 13 TeV", 
                          "Inclusive #gamma events", 
                          "Trimmed anti-k_{t}^{R=1.0}",
                          "Req. 1 #gamma with p_{T} > 155 GeV", 
                          ] 
                 + ([] if histvar == 'pt' else ["Jets with p_{T} > 150 GeV"])
                 #+ (["Requiring #tau_{21} < 0.38"] if histvar in ['pt', 'rho'] else []))
                 + ["Requiring %s #in [%3.1f, %3.1f) %s" % ((displayName[cutvar] if cutvar in displayName else cutvar),
                                                            p[0], 
                                                            p[1],
                                                            (displayUnit[cutvar] if cutvar in displayUnit else '')) 
                    for cutvar, p in cuts[histvar].iteritems()]
                 #+ (["Requiring D_{2} < 1.5"] if histvar in ['pt', 'rho'] else [])
                 )
        
        c.Update()
        wait()
        pass # end: loop histograms dict

    return

    # -----------------------------------------------------------------------------
   
    axes2D = [ #(20,   0.0, 3000.0, 0.0, 200.0),
        #(20, -14.0,    0.0, 0.0,   2.0),
        (60, -14.0,    0.0, 0.0,  10.0),
        #(20,  -5.0,    8.0, 0.0,   2.0),
        #(60,  -5.0,    8.0, 0.0,  10.0),
        ]
    
    labels2D = [ #('Jet p_{T} [GeV]', 'Jet M [GeV]'), 
        #('Jet #rho; log(m^{2}/p_{T}^{2})', 'Jet #tau_{21}'), 
        ('Jet #rho; log(m^{2}/p_{T}^{2})', 'Jet D_{2}^{#beta=1}'), 
        #('Jet #rho^{DDT}; log[m^{2}/(p_{T} #times 1 GeV)]', 'Jet #tau_{21}'), 
        #('Jet #rho^{DDT}; log[m^{2}/(p_{T} #times 1 GeV)]', 'Jet D_{2}^{#beta=1}'), 
        ]
    
    # Loop all plots:
    for iplot, plot in enumerate(plots2D):
        h = dict()
        
        ranges = [ ( 300,  400), 
                   ( 500,  600),
                   ( 700,  800),
                   #(1000, 1500), 
                   #(2000, 4000),
                   ]
        
        for r in ranges:
            name = '%d,%d' % r
            h[name] = TProfile('h_' + name + '_%d' % iplot, "", *axes2D[iplot])
            pass
        
        hitems = sorted(h.items(), key = lambda p: int(p[0].split(',')[0]), reverse = False)
        print hitems 
        for i, (k,v) in enumerate(hitems):
            v.SetLineColor  (kRed - 0 + i)
            v.SetMarkerColor(kRed - 0 + i)
            v.GetYaxis().SetRangeUser(axes2D[iplot][3], axes2D[iplot][4] * 1.5)
            pass
        
        for k,v in h.items():
            v.SetDirectory(0) # Keep in memory after input file is closed!
            defaultStyleHist(v)
            pass
        
        TProfile.AddDirectory(False) # Keep in memory after file is closed!
        
        # Loop files and perform plotting.    
        for ipath, path in enumerate(paths):
            
            DSID = None
            
            # Getting tree.
            f = TFile.Open(path, 'READ')
                
            for event in f.Get('outputTree'):
                DSID = event.DSID
                break
                
            # -- Only consider samples for which we have chosen to provide cross section information.
            if DSID not in xsec:
                continue
            if getType(DSID) == 'signal':
                continue
            
            t = f.Get(plot.split(':')[0])
            
            # Styling histograms.
            for k,v in hitems:
                v.GetXaxis().SetTitle( labels2D[iplot][0] )
                v.GetYaxis().SetTitle( '#LT' + labels2D[iplot][1] + '#GT')
                pass

            # Filling histograms. (Efficient?)
            if t:
                
                # -- Fill vectors.
                N = t.GetEntries()
                
                vec_x  = [0 for i in range(N)]
                vec_y  = [0 for i in range(N)]
                vec_pt = [0 for i in range(N)]
                
                x  = array('d', [0])
                y  = array('d', [0])
                pt = array('d', [0])
                
                t.SetBranchAddress( plot.split(':')[2], x )
                t.SetBranchAddress( plot.split(':')[1], y )
                if   plot.split(':')[2] == 'plot_fatjet_pt':
                    pt = x
                elif plot.split(':')[1] == 'plot_fatjet_pt':
                    pt = y
                else:
                    t.SetBranchAddress( 'plot_fatjet_pt', pt )
                    pass
                
                weight = xsec[DSID]
                
                i = 0
                while t.GetEntry(i):
                    vec_x [i] = x [0] #min(max(x[0], axes2D[iplot][1] + eps), axes2D[iplot][2] - eps)
                    vec_y [i] = y [0]
                    vec_pt[i] = pt[0]                        
                    i += 1
                    pass
                
                # -- Sort vectors.
                vec_x, vec_y, vec_pt = zip(*sorted(zip(vec_x, vec_y, vec_pt), key = lambda p: p[2]))

                # -- Fill TProfiles.
                for i in range(N):
                    name = ''
                    this_pt = vec_pt[i]
                    for r in ranges:
                        if r[0] <= this_pt < r[1]:
                            name = '%d,%d' % r
                            pass
                        pass
                    if name == '':
                        continue
                    
                    h[name].Fill( vec_x[i],
                                  vec_y[i],
                                  weight)
                    pass
                
                pass
            
            f.Close()            
            pass

        # Canvas
        c = TCanvas('c', "", int(600 / 0.85), 600)
        defaultStyleCanvas(c, colorbar = True)
        c.SetLogz(False)
            
        # Draw
        # -- Histograms
        for i,(k, v) in enumerate(hitems):
            v.Draw('PE ' + ('SAME' if i > 0 else ''))
            pass

        # -- Perform - and draw - fit
        if doFit:
            for v, k in hitems:
                print v
                fit = TF1('fit', 'pol1', -7.5, -0.66)
                fit.SetLineWidth(1)
                
                k.Fit(fit, 'R')
                fit.DrawCopy('SAME')
                
                pass
            pass
            
        # -- Text
        drawText(c, [ "#sqrt{s} = 13 TeV",
                      "Inclusive #gamma background",
                      "Trimmed anti-k_{t}^{R=1.0} jets", 
                      #"#geq 1 jet with p_{T} > 250 GeV",
                      "Req. 1 #gamma with p_{T} > 155 GeV",
                      "d#phi(J,#gamma) > #pi/2",
                      ])
            
        # -- Legend
        Nhist = len(hitems)
        ystep = 1.2 * fontSizeM
        
        lxmin, lxmax = (0.6, 0.80)
        lymin, lymax = (0.84 - Nhist * ystep, 0.84)
        
        legend = TLegend(lxmin, lymin, lxmax, lymax)
        legend.SetTextSize(fontSizeM)
        legend.SetBorderSize(0)
        legend.SetFillStyle(0)
        for k,v in hitems:
            legend.AddEntry(v, k, 'LE')
            pass
        legend.Draw()
        
        # -- Lines and text
        if 'rho' in plot.split(':')[2]: # and False:
            line = TLine()
            line.SetLineWidth(2)
            line.SetLineStyle(2)
            
            text = TLatex()
            text.SetTextFont(42)
            text.SetTextSize(fontSizeS * 0.7)
            text.SetTextAngle(30.)
            for r in ranges:
                ymindraw = axes2D[iplot][3]
                ymaxdraw = axes2D[iplot][3] + 0.35 * (axes2D[iplot][4] * 2.0 - axes2D[iplot][3])
                mass = 20.
                if 'rhoDDT' in plot.split(':')[2]:
                    xdraw = math.log( pow(mass, 2.) / pow(r[0], 1.) )
                else:
                    xdraw = math.log( pow(mass, 2.) / pow(r[0], 2.) )
                    pass
                line.DrawLine(xdraw, ymindraw, xdraw, ymaxdraw)
                text.DrawLatex(xdraw, ymaxdraw, "M = %2.0f GeV at p_{T} = %3.0f GeV" % (mass, r[0]))
                pass
            pass

        # -- Show
        c.Update()
        c.SaveAs('vs'.join( plot.split(':')[1:] ) + '.pdf')
        #wait()
         
        pass # end: loop plots.
            
    return 0



if __name__ == '__main__':
    main()
    pass
