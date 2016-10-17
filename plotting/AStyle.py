from ROOT import *
from array import array
import math

# Global utility variables.
eps = 1E-09
pi  = 3.14159265359


# Global style variables.
font = 42

fontSizeS = 0.035 # 0.035
fontSizeM = 0.038  # 0.040
fontSizeL = 0.040

kMyBlue  = 1001;
myBlue   = TColor(kMyBlue,   0./255.,  30./255.,  59./255.)
kMyRed   = 1002;
myRed    = TColor(kMyRed,  205./255.,   0./255.,  55./255.)
kMyGreen = kGreen + 2
kMyLightGreen = kGreen - 10


# Custom style definition.
AStyle = TStyle('AStyle', "AStyle")

# -- Canvas colours
AStyle.SetFrameBorderMode(0)
AStyle.SetFrameFillColor(0)
AStyle.SetCanvasBorderMode(0)
AStyle.SetCanvasColor(0)
AStyle.SetPadBorderMode(0)
AStyle.SetPadColor(0)
AStyle.SetStatColor(0)

# -- Canvas size and margins
#AStyle.SetPaperSize(20, 20)

AStyle.SetPadTopMargin   (0.05)
AStyle.SetPadRightMargin (0.05)
AStyle.SetPadBottomMargin(0.15)
AStyle.SetPadLeftMargin  (0.15)

AStyle.SetTitleOffset(1.2, 'x')
AStyle.SetTitleOffset(1.5, 'y')
AStyle.SetTitleOffset(1.6, 'z')

# -- Fonts
AStyle.SetTextFont(font)

AStyle.SetTextSize(fontSizeS)

for coord in ['x', 'y', 'z']:
    AStyle.SetLabelFont(font,      coord)
    AStyle.SetTitleFont(font,      coord)
    AStyle.SetLabelSize(fontSizeM, coord)
    AStyle.SetTitleSize(fontSizeM, coord)
    pass

AStyle.SetLegendFont(font)
AStyle.SetLegendTextSize(fontSizeS)

# -- Histograms
AStyle.SetMarkerStyle(20)
AStyle.SetMarkerSize(1.2)
AStyle.SetHistLineWidth(2)
AStyle.SetLineStyleString(2,"[12 12]") # postscript dashes

AStyle.SetErrorX(0.001) # No x-axis errors
AStyle.SetEndErrorSize(0.) # No errorbar caps

# -- Canvas
AStyle.SetOptTitle(0)
AStyle.SetOptStat(0)
AStyle.SetOptFit(0)

AStyle.SetPadTickX(1)
AStyle.SetPadTickY(1)
AStyle.SetLegendBorderSize(0)

# Colour palette.
def set_palette(name='palette', ncontours=999):
    """Set a color palette from a given RGB list
    stops, red, green and blue should all be lists of the same length
    see set_decent_colors for an example"""

    stops = [0.00, 1.00]
    red   = [0.98,  0./255.]
    green = [0.98, 30./255.]
    blue  = [0.98, 59./255.]

    s = array('d', stops)
    r = array('d', red)
    g = array('d', green)
    b = array('d', blue)

    npoints = len(s)
    TColor.CreateGradientColorTable(npoints, s, r, g, b, ncontours)
    gStyle.SetNumberContours(ncontours)
    return

set_palette()


# Utility functions.
# --------------------------------------------------------------------
def wait ():
    raw_input('...')
    return

def loadXsec (path):
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


# Set (and force) style.
# --------------------------------------------------------------------

gROOT.SetStyle("AStyle")
gROOT.ForceStyle()
