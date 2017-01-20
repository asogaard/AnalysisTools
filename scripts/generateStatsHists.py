#!/usr/bin/env python
# -*- coding: utf-8 -*-

''' Script for producing histograms for the statistical procedure. '''

# Basic
import sys
import re

# Scientific
from ROOT import *
from root_numpy import fill_hist, array2tree

import numpy as np

# Local include(s)
from AFunctions import *

# Main function.
def main ():

    # Setup.
    # ----------------------------------------------------------------

    print "\nSetup."

    # Validate arguments.
    print "-- Validate arguments."
    validateArguments(sys.argv)

    # Load cross sections files.
    print "-- Load cross sections file."
    xsec = loadXsec('../share/sampleInfo.csv')

    # Get list of file paths to plot from commandline arguments.
    print "-- Get list of input paths."
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]

    # Specify which variables to get.
    print "-- Specify variables to read."
    treename = 'BoostedJet+ISRgamma/EventSelection/Pass/tau21DDT_0p50/Postcut'
    prefix   = 'leadingfatjet_'

    getvars  = ['m']

    # Load data.
    print "-- Load data."
    data = loadDataFast(paths, treename, getvars, prefix, xsec)

    # Check output.
    print "-- Check output exists."
    if not data:
        print "WARNING: No data was loaded."
        return

    # Initialise total number of jets.
    print "-- Initialise number of (good) jets."
    N = len(data[getvars[0]])


    # Fill output histograms.
    # ----------------------------------------------------------------
    print "\nFill output histograms."


    hists = list()
    ranges = [
        # Backgrounds.
        ((361039, 361062), "estimate"), # Sherpa gamma + jet
        ((305435, 305439), "W"), # Sherpa gamma + W
        ((305440, 305444), "Z"), # Sherpa gamma + Z

        # Signals.
        ((305159, 305159), "mRp1_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp1_mD10_gSp3_gD1
        ((305160, 305160), "mRp2_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp2_mD10_gSp3_gD1
        ((305161, 305161), "mRp3_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp3_mD10_gSp3_gD1
        ((305162, 305162), "mRp4_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp4_mD10_gSp3_gD1
        ((305163, 305163), "mRp5_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp5_mD10_gSp3_gD1
        
        ((305367, 305367), "mRp015_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp015_mD10_gSp3_gD1
        ((305368, 305368), "mRp025_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp025_mD10_gSp3_gD1
        ((305369, 305369), "mRp035_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp035_mD10_gSp3_gD1
        ((305370, 305370), "mRp045_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp045_mD10_gSp3_gD1
        ((305371, 305371), "mRp055_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp055_mD10_gSp3_gD1
        ((305372, 305372), "mRp065_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp065_mD10_gSp3_gD1
        ((305373, 305373), "mRp075_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp075_mD10_gSp3_gD1
        ((305374, 305374), "mRp085_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp085_mD10_gSp3_gD1
        
        ((305465, 305465), "mRp25_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp25_mD10_gSp3_gD1
        #((305466, 305466), "mRp25_mD10_gSp2_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp25_mD10_gSp2_gD1
        #((305467, 305467), "mRp25_mD10_gSp1_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp25_mD10_gSp1_gD1
        ((305468, 305468), "mRp35_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp35_mD10_gSp3_gD1
        #((305469, 305469), "mRp35_mD10_gSp2_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp35_mD10_gSp2_gD1
        #((305470, 305470), "mRp35_mD10_gSp1_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp35_mD10_gSp1_gD1
        ((305471, 305471), "mRp45_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp45_mD10_gSp3_gD1
        #((305472, 305472), "mRp45_mD10_gSp2_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp45_mD10_gSp2_gD1
        ((305474, 305474), "mRp55_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp55_mD10_gSp3_gD1
        #((305475, 305475), "mRp55_mD10_gSp2_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp55_mD10_gSp2_gD1
        #((305476, 305476), "mRp55_mD10_gSp1_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp55_mD10_gSp1_gD1
        #((305477, 305477), "mRp75_mD10_gSp4_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp75_mD10_gSp4_gD1
        ((305478, 305478), "mRp75_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp75_mD10_gSp3_gD1
        #((305479, 305479), "mRp75_mD10_gSp2_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp75_mD10_gSp2_gD1
        ((305481, 305481), "mRp95_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mRp95_mD10_gSp3_gD1
        #((305482, 305482), "mR1p5_mD10_gSp4_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mR1p5_mD10_gSp4_gD1
        ((305483, 305483), "mR1p5_mD10_gSp3_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph100_mR1p5_mD10_gSp3_gD1
        ]

    # Get titles.
    new_ranges = list()
    counter = 0
    for DSIDs, name in ranges:
        if name.startswith('mR'):
            m = re.search('mR([0-9]*)p([0-9]+)*', name)
            masstext = '%s%s' % (m.group(1), m.group(2).ljust(3, '0').lstrip('0'))
            title = 'Signal_%s' % masstext
            mass = int(masstext.rjust(4, '0'))
        else:
            title = name
            mass = counter
            counter += 1
            pass
        title = 'mJ_' + title
        new_ranges.append((DSIDs, name, title, mass))
        pass
    ranges = new_ranges

    # Write to file.
    outfile = TFile('ISRgamma_histograms.root', 'RECREATE')
    for DSIDs, name, title, mass in sorted(ranges, key = lambda t : t[3]):
        print "Filling histogram for '%s' (%d)." % (name, mass)
        msk = np.where((data['DSID'] >= DSIDs[0]) & (data['DSID'] <= DSIDs[1]))

        # Fill histogram
        hist = TH1F(title, "", 30, 100., 250.)
        hist.Sumw2()
        fill_hist(hist, data['m'][msk], weights = data['weight'][msk])
        hist.Write()
        pass

    for DSIDs, _, title, mass in sorted(ranges, key = lambda t : t[3]):
        print "Filling tree for '%s'." % name
        msk = np.where((data['DSID'] >= DSIDs[0]) & (data['DSID'] <= DSIDs[1]))

        # Fill tree
        arr = np.array(data['m'][msk].ravel(),
                       dtype=[('m', np.float32),
                              ])
        tree = array2tree(arr, 'tree_' + title)
        tree.Write()
        pass

    outfile.Close()

    # ...

    return


# Main function call.
if __name__ == '__main__':
    main()
    pass
