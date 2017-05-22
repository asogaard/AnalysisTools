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
from snippets.functions import *

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
    treename = 'BoostedJet+ISRgamma/Nominal/EventSelection/Pass/Jet_tau21DDT/Postcut'
    prefix   = 'Jet_' # 'leading_LargeRadiusJets_'

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
        ((361039, 361062), "QCD"), # Sherpa gamma + jet
        #((100080, 100080), "QCD"), # Sherpa gamma + jet
        ((305435, 305439), "W"), # Sherpa gamma + W
        ((305440, 305444), "Z"), # Sherpa gamma + Z

        # Signals.
        ((308363, 308363), "mRp100_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph150_mRp100_mD10_gSp5_gD1
        ((308364, 308364), "mRp130_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph195_mRp130_mD10_gSp5_gD1
        ((308365, 308365), "mRp160_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph240_mRp160_mD10_gSp5_gD1
        ((308366, 308366), "mRp190_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph285_mRp190_mD10_gSp5_gD1
        ((308367, 308367), "mRp220_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph330_mRp220_mD10_gSp5_gD1

        ]

    # Get titles.
    new_ranges = list()
    counter = 0
    for DSIDs, name in ranges:
        if name.startswith('mR'):
            m = re.search('mR([0-9]*)p([0-9]+)*', name)
            masstext = '%s%s' % (m.group(1), m.group(2).ljust(3, '0').lstrip('0'))
            title = 'signal_%s' % masstext
            mass = int(masstext.rjust(4, '0'))
        else:
            title = name
            mass = counter
            counter += 1
            pass
        title = title
        new_ranges.append((DSIDs, name, title, mass))
        pass
    ranges = new_ranges

    # Write to signal file(s).
    signalfile = TFile('ISRgamma_signals.root', 'RECREATE')
    for DSIDs, _, title, mass in sorted(ranges, key = lambda t : t[3]):
        if mass <= 2: continue

        print "Filling tree for '%s'." % title
        msk = np.where((data['DSID'] >= DSIDs[0]) & (data['DSID'] <= DSIDs[1]))

        # Fill tree
        M = [tuple(el) for el in np.hstack((np.atleast_2d(data['m']     [msk].ravel()).T,
                                            np.atleast_2d(data['weight'][msk].ravel()).T)).tolist()]
        arr = np.array(M, dtype=[('m_J',    'f4'),
                                 ('weight', 'f4'),
                                 ])

        tree = array2tree(arr, title)
        tree.Write()
        pass

    signalfile.Close()

    # Writing to background file(s)
    backgroundfile = TFile('ISRgamma_backgrounds.root', 'RECREATE')
    for DSIDs, _, title, mass in sorted(ranges, key = lambda t : t[3]):
        if mass > 2: continue

        print "Filling tree for '%s'." % title
        msk = np.where((data['DSID'] >= DSIDs[0]) & (data['DSID'] <= DSIDs[1]))
        
        # Fill tree
        M = [tuple(el) for el in np.hstack((np.atleast_2d(data['m']     [msk].ravel()).T,
                                            np.atleast_2d(data['weight'][msk].ravel()).T)).tolist()]
        arr = np.array(M, dtype=[('m_J',    'f4'),
                                 ('weight', 'f4'),
                                 ])

        tree = array2tree(arr, title)
        tree.Write()
        pass

    backgroundfile.Close()

    return


# Main function call.
if __name__ == '__main__':
    main()
    pass
