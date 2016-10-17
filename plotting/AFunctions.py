from ROOT import TFile, TTree, TH1, TProfile, TStyle, TCanvas
from array import array
import math
import sys

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


def drawText (lines = [], c = None, pos = 'NW', qualifier = 'Internal simulation'):
    ''' Draw text on TPad, including ATLAS line. '''

    if not c: c = gPad
    c.cd()

    t = TLatex()

    h = c.GetWh()
    w = c.GetWw()

    offset = 0.05
    ystep = t.GetTextSize() * 1.25
    scale = (w/float(h) if w > h else h/float(w))

    x =       c.GetLeftMargin() + offset * scale
    y = 1.0 - c.GetTopMargin()  - offset - t.GetTextSize() * 1.2

    t.DrawLatexNDC(x, y, "#scale[1.15]{#font[72]{ATLAS} }#scale[1.05]{%s}" % qualifier)
    y -= ystep * 1.25

    for line in lines:
        t.DrawLatexNDC(x, y, line)
        y -= ystep;
        pass

    return


def drawLegend (histograms, names, types = None, c = None,
                xmin = None,
                xmax = None,
                ymin = None,
                ymax = None,
                horisontal = 'R',
                vertical   = 'T',
                width = 0.40):
    ''' Draw legend on TPad. '''

    if not c: c = gPad
    c.cd()

    N = len(histograms)
    if N != len(names):
        print "drawLegend: WARNING, number of histograms (%d) and names (%d) don't match." % (N, len(names))
        return None

    if types and N != len(types):
        print "drawLegend: WARNING, number of histograms (%d) and provided types (%d) don't match." % (N, len(types))
        return None

    if not types:
        types = ["" for _ in range(N)]
        pass

    fontsize = gStyle.GetLegendTextSize()

    offset = 0.04
    height = N * fontsize * 1.25

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
            ymax = 1. - c.GetTopMargin() - offset
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

    #print xmin, ymin, xmax, ymax

    legend = TLegend(xmin, ymin, xmax, ymax)
    
    for (h,n,t) in zip(histograms, names, types):
        legend.AddEntry(h, n, t)
        pass

    legend.Draw()

    # -- Make global (i.e. don't delete when going out of scope)
    from ROOT import SetOwnership
    SetOwnership( legend, 0 ) 

    return legend


def getPlotMinMax (histograms, log, padding = 1.0, ymin = None):
    ymax = max([h.GetMaximum() for h in histograms])
    ymin = ((1e-05 if log else 0.5) if not ymin else ymin)

    if ymax < ymin: ymax = 2 * ymin
    if log:
        ymax = math.exp(math.log(ymax) + (math.log(ymax) - math.log(ymin)) * padding)
    else:
        ymax = ymax + (ymax - ymin) * padding
        pass

    return (ymin, ymax)


def loadData (paths, treename, variables, prefix = '', xsec = None, ignore = None, DSIDvar = 'DSID'):
    ''' Read in data arrays from TTree. '''
    
    values = dict()

    print ""
    print "getData: Reading data from %d files." % len(paths)

    loadingCharacters  = ['\\', '|', '/', '-']
    iLoadingCharacter  = 0
    nLoadingCharacters = len(loadingCharacters)

    if len(paths) == 0: return 
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

            if 'weight' not in values:
                values['weight'] = []
                pass

            pass 

        # -- Ignore signal samples.
        if ignore and ignore(DSID):
            continue

        t = f.Get(treename)

        if not t:
            "getData: Could not get tree '%s' from file '%s'." % (treename, path)
            continue
        N = t.GetEntries()

        # -- Set up objects for reading tree.
        branch = dict()
        for var in variables:
            branch[var] = array('d', [0])
            
            if var not in values:
                values[var] = array('d', [])
                pass
            
            t.SetBranchAddress( prefix + var, branch[var] )
            pass

        # -- Read tree
        i = 0
        while t.GetEntry(i):
            for var in variables:
                values[var].append( branch[var][0] )
                pass
            i += 1

            if i % 10000 == 0:
                print "\rgetData:  [%s]" % loadingCharacters[iLoadingCharacter],
                sys.stdout.flush()
                iLoadingCharacter = (iLoadingCharacter + 1) % nLoadingCharacters
                pass
            pass

        if xsec:
            weight = xsec[DSID]
            values['weight'] += [weight for _ in range(N)]
            pass
        
        pass

    print ""
    print "getData: Done."
    print ""

    return values
