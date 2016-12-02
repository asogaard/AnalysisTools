from ROOT  import *
from array import *
from collections import namedtuple

import math
import sys

try:
    import numpy as np
    from root_numpy import tree2array
except:
    print "Make numpy and root_numpy available in order to use e.g. 'loadDataFast'."
    pass


# Global utility variables.
eps = 1E-09
pi  = 3.14159265359

# Utility functions.
def wait ():
    ''' Generic wait function. '''
    raw_input('...')
    return



def loadXsec (path):
    ''' Load cross section weights from file. '''
    xsec = dict()
    with open(path, 'r') as f:
        for l in f:
            line = l.strip()
            if line == '' or line.startswith('#'):
                continue
            fields = [f.strip() for f in line.split(',')]
            if int(fields[2]) == 0:
                continue
            xsec[int(fields[0])] = float(fields[1]) / float(fields[2]) * float(fields[3])
            pass
        pass
    return xsec



def getMaximum (h):
    ''' Get *actual* maximum bin in histogram or similar. '''
    if type(h) in [TF1, TEfficiency]:
        return -1
    N = h.GetXaxis().GetNbins()
    return max([h.GetBinContent(i + 1) for i in range(N)])



def drawText (lines = [], c = None, pos = 'NW', qualifier = 'Internal simulation'):
    ''' Draw text on TPad, including ATLAS line. '''
    if not c: c = gPad
    c.cd()

    # Checks.
    if lines is None:
        lines = []
        pass

    t = TLatex()

    h = c.GetWh()
    w = c.GetWw()

    offset = 0.05
    ystep = t.GetTextSize() * 1.25
    scale = (w/float(h) if w > h else h/float(w))

    x =       c.GetLeftMargin() + offset * scale
    y = 1.0 - c.GetTopMargin()  - offset - t.GetTextSize() * 1.0

    t.DrawLatexNDC(x, y, "#scale[1.15]{#font[72]{ATLAS} }#scale[1.05]{%s}" % qualifier)
    y -= ystep * 1.25

    for line in lines:
        t.DrawLatexNDC(x, y, line)
        y -= ystep;
        pass

    c.Update()

    return



def drawLegend (histograms, names, types = None, c = None,
                xmin = None,
                xmax = None,
                ymin = None,
                ymax = None,
                header = None,
                categories = None,
                horisontal = 'R',
                vertical   = 'T',
                width = 0.30):
    ''' Draw legend on TPad. '''
    if not c: c = gPad
    c.cd()

    N = len(histograms)
    if N != len(names):
        print "drawLegend: WARNING, number of histograms (%d) and names (%d) don't match." % (N, len(names))
        pass

    if types is None:
        types = ''
        if drawOpt == 'HIST':
            types = 'LE'
            pass
        pass

    if type(types) == str:
        types = [types] * N
        pass

    if types and N != len(types):
        print "drawLegend: WARNING, number of histograms (%d) and provided types (%d) don't match." % (N, len(types))
        return None

    fontsize = gStyle.GetLegendTextSize()

    offset = 0.04
    height = (min(N, len(names)) + (0 if header == None else 1) + (len(categories) if categories else 0))* fontsize * 1.25

    # -- Setting x coordinates.
    if not (xmin or xmax):
        if   horisontal.upper() == 'R':
            xmax = 1. - c.GetRightMargin() - offset
        elif horisontal.upper() == 'L':
            xmin = c.GetLeftMargin() + offset
        else:
            print "drawLegend: Horisontal option '%s' not recognised." % horisontal
            return
        pass
    if xmax and (not xmin):
        xmin = xmax - width
        pass
    if xmin and (not xmax):
        xmax = xmin + width
        pass

    # -- Setting y coordinates.
    if not (ymin or ymax):
        if   vertical.upper() == 'T':
            ymax = 1. - c.GetTopMargin() - offset * 1.5
        elif vertical.upper() == 'B':
            ymin = c.GetBottomMargin() + offset
        else:
            print "drawLegend: Vertical option '%s' not recognised." % vertical
            return
        pass
    if ymax and (not ymin):
        ymin = ymax - height
        pass
    if ymin and (not ymax):
        ymax = ymin + height
        pass

    legend = TLegend(xmin, ymin, xmax, ymax)

    if header != None:
        legend.AddEntry(None, header, opt)
        pass

    for (h,n,t) in zip(histograms, names, types):
        legend.AddEntry(h, n, t)
        pass

    if categories:
        for icat, (name, hist, opt) in enumerate(categories):
            legend.AddEntry(hist, name, opt)
            pass
        pass

    legend.Draw()

    # -- Make global (i.e. don't delete when going out of scope)
    from ROOT import SetOwnership
    SetOwnership( legend, 0 ) 

    c.Update()

    return legend



def getPlotMinMax (histograms, log, padding = None, ymin = None):
    ''' Get optimal y-axis plotting range given list of histograms (or sim.). '''
    padding = padding if padding else 1.0
    ymax = max(map(getMaximum, histograms))
    ymin = (ymin if ymin is not None else (1e-05 if log else 0.))

    if ymax < ymin: ymax = 2 * ymin
    if log:
        ymax = math.exp(math.log(ymax) + (math.log(ymax) - math.log(ymin)) * padding)
    else:
        ymax = ymax + (ymax - ymin) * padding
        pass

    return (ymin, ymax)



LegendOptions = namedtuple('LegendOptions', ['histograms', 'names', 'types', 'c', 'xmin', 'xmax', 'ymin', 'ymax', \
'header', 'categories', 'horisontal', 'vertical','width'])
LegendOptions.__new__.__defaults__ = ('LP', None, None, None,  None,  None, None, None, 'R', 'T', 0.30)

TextOptions = namedtuple('TextOptions', ['lines', 'c', 'pos', 'qualifier'])
TextOptions.__new__.__defaults__ = ([], None, 'NW','Internal simulation',)

def makePlot (pathHistnamePairs,
              legendOpts = None, #LegendOptions([], []),
              textOpts   = None, #TextOptions(),
              ymin = None,
              ymax = None,
              logy = False,
              padding = None,
              xtitle = None,
              ytitle = None,
              ztitle = None,
              colours = None, 
              linewidths = None,
              markers = None,
              xlines = None,
              ylines = None,
              drawOpt = '',
              normalise = False,
              profileRMS = False):
    ''' ... '''

    # Variable declarations
    if not colours:
        colours = [kRed   + i * 2 for i in range(3)] + \
                  [kBlue  + i * 2 for i in range(3)] + \
                  [kGreen + i * 2 for i in range(3)]
        pass

    if not markers:
        markers = [20] * len(pathHistnamePairs)
        pass

    if not linewidths:
        linewidths = [2] * len(pathHistnamePairs)
        pass

    # Loop all pairs of paths and histograms.
    # -- Assuming list(tuple(*,*)) structure.
    histograms = list()
    if len(pathHistnamePairs) > 0 and type(pathHistnamePairs[0]) in [tuple, list]:
        for entry in pathHistnamePairs:
            if type(entry) is list:
                print "WARNING: You're passing a list of lists to makePlot."
                continue

            # Check if entry is histogram.
            if type(entry) in [TH1F, TH2F, TProfile, TEfficiency]:
                h = entry
            else:
                # Otherwise, read from file.
                path, histname = entry
                
                # -- Open file
                f = TFile.Open(path, 'READ')
                
                # -- Get histogram.
                print "histname: '%s'" % histname
                h = f.Get(histname)
                ''' If TTree? '''
                
                # -- Keep in memory after file is closed.
                h.SetDirectory(0)
                if   isinstance(h, TH2):
                    TH2.AddDirectory(False)
                elif isinstance(h, TH1):
                    TH1.AddDirectory(False)
                    pass
                
                pass

            # Common, regardless of how the histogram was obtained.
            # -- Normalise.
            if normalise and h.Integral() > 0:
                h.Scale(1./h.Integral())
                pass
            
            # -- Make TProfile show RMS
            if isinstance(h, TProfile) and profileRMS:
                h.SetErrorOption('s')
                pass

            # -- Append to list of histograms to be plotted.
            histograms.append(h)
            
            pass    
    else:
        histograms = pathHistnamePairs
        pass

    # Style.
    (ymin, _ymax) = getPlotMinMax(histograms, logy, ymin = ymin, padding = padding)
    ymax = ymax if ymax else _ymax
    for i,h in enumerate(histograms):

        if i < len(colours):
            h.SetLineColor  (colours[i])
            h.SetMarkerColor(colours[i])
            h.SetMarkerStyle(markers[i])
            h.SetLineWidth  (linewidths[i])
            pass

        # -- Axes.
        if type(h) in [TEfficiency]:
            h.SetTitle(';%s;%s;%s' % ( (xtitle if xtitle else ''),
                                       (ytitle if ytitle else ''),
                                       (ztitle if ztitle else ''),
                                       ))
            continue
        if xtitle: h.GetXaxis().SetTitle(xtitle)
        if ytitle: h.GetYaxis().SetTitle(ytitle)

        h.GetYaxis().SetRangeUser(ymin, ymax)

        pass

    # Canvas.
    c = TCanvas('c', "", int(600 * (7./6. if drawOpt and 'Z' in drawOpt else 1.)), 600)
    if drawOpt and 'Z' in drawOpt:
        c.SetRightMargin(0.15)
        pass
    c.SetLogy(logy)

    # Draw axes.
    histograms[0].Draw('AXIS')
    #for i,h in enumerate(histograms):
    #    h.Draw(drawOpt + ('SAME' if i > 0 else ''))
    #    pass

    # Lines
    if xlines or ylines:
        c.Update()

        xmin = gPad.GetUxmin() # c.GetFrame().GetX1()
        xmax = gPad.GetUxmax() # c.GetFrame().GetX2()
        ymin = ymin if ymin else gPad.GetUymin() # c.GetFrame().GetY1()
        ymax = ymax if ymax else gPad.GetUymax() # c.GetFrame().GetY2()

        line = TLine()
        line.SetLineColor(kGray + 2)
        line.SetLineStyle(2)

        # -- x-axis
        if xlines:
            for xline in xlines:
                line.DrawLine(xline, ymin, xline, ymax)
                pass
            pass

        # -- y-axis
        if ylines:
            for yline in ylines:
                line.DrawLine(xmin, yline, xmax, yline)
                pass
            pass

        pass

    # Draw histograms.
    for i,h in enumerate(histograms):
        h.Draw(drawOpt + ' SAME')
        pass

    # Text.
    if textOpts:
        drawText( *[v for _,v in textOpts._asdict().items()] )
        pass

    # Legend.
    if legendOpts:
        legendOpts = legendOpts._replace(histograms = histograms)
        drawLegend( *[v for _,v in legendOpts._asdict().items()] )
        pass

    # Update.
    c.Update()
    return c



def loadDataFast (paths, treename, branches, prefix = '', xsec = None, ignore = None, DSIDvar = 'DSID', Nevents = 29):
    
    print ""
    print "loadDataFast: Reading data from %d files." % len(paths)

    ievent = 0

    # Initialise data array.
    data = None

    # Initialise DSID variable.
    DSID = None

    # Loop paths.
    for ipath, path in enumerate(paths):

        # Print progress.
        print "\rloadDataFast:   [%-*s]" % (len(paths), '-' * (ipath + 1)),
        sys.stdout.flush()

        # Get file.
        f = TFile(path, 'READ')

        # Get tree.
        t = f.Get(treename)

        # Get DSID.
        for event in f.Get('outputTree'):
            DSID = eval('event.%s' % DSIDvar)
            break

        # Check whether to explicitly ignore.
        if ignore and DSID and ignore(DSID):
            print "Ignoring DSID %d." % DSID
            continue

        # Load new data array.
        arr = tree2array(t,
                         branches = [prefix + br for br in branches],
                         include_weight = True,
                         )                         
        # Add cross sections weights.
        if xsec and DSID:

            # Ignore of we didn't provide cross section information.
            if DSID not in xsec:
                print "Skipping DSID %d." % DSID
                continue

            # Scale weight by cross section.
            arr['weight'] *= xsec[DSID]

            pass

        # Append to existing data arra
        if data is None:
            data = arr
        else:
            data = np.concatenate((data, arr))
            pass

        pass

    print ""

    # Change branch names to remove prefix.
    data.dtype.names = [name.replace(prefix, '') for name in data.dtype.names]
    
    # Dict-ify.
    values = dict()
    for branch in data.dtype.names:
        values[branch] = data[branch]
        pass

    return values



def loadData (paths, treename, variables, prefix = '', xsec = None, ignore = None, DSIDvar = 'DSID', Nevents = None):
    ''' Read in data arrays from TTree. '''
    values = dict()

    print ""
    print "loadData: Reading data from %d files." % len(paths)

    loadingCharacters  = ['\\', '|', '/', '-']
    iLoadingCharacter  = 0
    nLoadingCharacters = len(loadingCharacters)

    if len(paths) == 0: return 

    ievent = 0
    for ipath, path in enumerate(paths):

        DSID = None

        # Getting tree.
        f = TFile.Open(path, 'READ')

        if xsec:
            for event in f.Get('outputTree'):
                DSID = eval('event.%s' % DSIDvar)
                break

            # -- Only consider samples for which we have chosen to provide cross section information.
            if DSID not in xsec:
                continue

            if 'xsec' not in values:
                values['xsec'] = array('d', [])
                pass

            pass 

        # -- Ignore signal samples.
        if ignore and ignore(DSID):
            print "Ignoring DSID %d." % DSID
            continue

        t = f.Get(treename)

        if not t:
            "loadData: Could not get tree '%s' from file '%s'." % (treename, path)
            continue
        N = t.GetEntries()

        # -- Set up objects for reading tree.
        branch = dict()
        for var in variables + ['weight'] :
            branch[var] = array('d', [0])
            
            if var not in values:
                values[var] = array('d', [])
                pass
            
            t.SetBranchAddress( var if var == 'weight' else prefix + var, branch[var] )
            pass

        # -- Read tree
        i = 0
        while t.GetEntry(i):
            # -- Break early, if needed.
            if Nevents is not None and ievent == Nevents:
                break
            ievent += 1

            for var in variables + ['weight']:
                values[var].append( branch[var][0] )
                pass
            i += 1

            if i % 10000 == 0:
                print "\rloadData:  [%s]" % loadingCharacters[iLoadingCharacter],
                sys.stdout.flush()
                iLoadingCharacter = (iLoadingCharacter + 1) % nLoadingCharacters
                pass

            pass

        if xsec:
            xsec_weight = xsec[DSID]
            values['xsec'] += array('d', [xsec_weight for _ in xrange(i)]) # N
            pass

        if Nevents is not None and ievent == Nevents:
            break
        
        pass

    # Scale 'weight' to also include cross section
    values['weight'] = array('d', [ w * x for (w,x) in zip(values['weight'], values['xsec'])])
    values.pop('xsec')

    print ""
    print "loadData: Done."
    print ""

    return values
