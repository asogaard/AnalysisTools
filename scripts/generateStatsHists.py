#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Script for producing histograms for the statistical procedure. """

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

    outdir = '/eos/atlas/user/a/asogaard/Analysis/2016/BoostedJetISR/StatsInputs/2017-06-19/'

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
    categories = [
        "Nominal",
        "TF_UP",
        "TF_DOWN",
        "LARGER_JET_Comb_Baseline_Kin__1up",
        "LARGER_JET_Comb_Baseline_Kin__1down",
        "LARGER_JET_Comb_Modelling_Kin__1up",
        "LARGER_JET_Comb_Modelling_Kin__1down",
        "LARGER_JET_Comb_TotalStat_Kin__1up",
        "LARGER_JET_Comb_TotalStat_Kin__1down",
        "LARGER_JET_Comb_Tracking_Kin__1up",
        "LARGER_JET_Comb_Tracking_Kin__1down",
        "LARGER_JET_Rtrk_Baseline_Sub__1up",
        "LARGER_JET_Rtrk_Baseline_Sub__1down",
        "LARGER_JET_Rtrk_Modelling_Sub__1up",
        "LARGER_JET_Rtrk_Modelling_Sub__1down",
        "LARGER_JET_Rtrk_TotalStat_Sub__1up",
        "LARGER_JET_Rtrk_TotalStat_Sub__1down",
        "LARGER_JET_Rtrk_Tracking_Sub__1up",
        "LARGER_JET_Rtrk_Tracking_Sub__1down",
        "PHOTON_EG_RESOLUTION_ALL__1down",
        "PHOTON_EG_RESOLUTION_ALL__1up",
        "PHOTON_EG_SCALE_ALL__1down",
        "PHOTON_EG_SCALE_ALL__1up",
        ]

    treename = 'BoostedJet+ISRgamma/{cat}/EventSelection/Pass/Jet_tau21DDT/Postcut'
    prefix   = 'Jet_'
    getvars  = ['m']

    # Load data.
    print "-- Load data."
    data = dict()
    for cat in categories:
        data[cat] = loadDataFast(paths, treename.format(cat=cat), getvars, prefix, xsec)
        pass

    # Check output.
    print "-- Check output exists."
    if False in [bool(data[cat]) for cat in categories]:
        print "WARNING: No data was loaded."
        return


    # Fill output histograms.
    # ----------------------------------------------------------------
    print "\nFill output histograms."

    # Format: (isMC, (DSID_min, DSID_max), "name")
    ranges = {
        # Backgrounds
        'bkg': [
            (True, (100000 + mass, 100000 + mass), "bkg_%03d" % mass) for mass in np.linspace(100, 220, (220 - 100) / 5 + 1, endpoint=True)# data | TF
            ],

        # Background (GBS)
        'gbs': [
            (True, (400000, 400000), "gbs"), # data | TF
            ],

        # W (qq) + gamma
        'W': [
            (True, (305435, 305439), "W"), # Sherpa gamma + W
            ],

        # Z (qq) + gamma
        'Z': [
            (True, (305440, 305444), "Z"), # Sherpa gamma + Z
            ],

        # Signals.
        'sig': [
            (True, (308363, 308363), "mRp100_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph150_mRp100_mD10_gSp5_gD1
            (True, (308364, 308364), "mRp130_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph195_mRp130_mD10_gSp5_gD1
            (True, (308365, 308365), "mRp160_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph240_mRp160_mD10_gSp5_gD1
            (True, (308366, 308366), "mRp190_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph285_mRp190_mD10_gSp5_gD1
            (True, (308367, 308367), "mRp220_mD10_gSp5_gD1"), # MGPy8EG_N30LO_A14N23LO_dmA_jja_Ph330_mRp220_mD10_gSp5_gD1
            ],

        # W/Z (qq) + gamma
        'data': [
            (False, (0, 1E+07), "data"), # Sherpa incl. gamma
            ],
        }

    # Signals. (interpolated)
    for mass in np.linspace(100, 220, (220 - 100) / 5 + 1, endpoint=True):
        if mass in [100, 130, 160, 190, 220]: continue
        ranges['sig'].append( (True, (200000 + mass, 200000 + mass), "mRp%03d_xyz" % mass))
        pass

    # Get titles.
    for key in ranges:
        new_ranges = list()
        for isMC, DSIDs, name in ranges[key]:
            title = name
            if name.startswith('mR'):
                m = re.search('mR([0-9]*)p([0-9]+)*', name)
                masstext = '%s%s' % (m.group(1), m.group(2).ljust(3, '0').lstrip('0'))
                title = 'signal_%s' % masstext
                pass
            new_ranges.append((isMC, DSIDs, title))
            pass
        ranges[key] = new_ranges
        pass

    # Write to file(s).
    for key in ranges:
        for isMC, DSIDs, title in ranges[key]:
            print "Filling tree for '%s'." % title
            f = TFile(outdir + '/ISRgamma_%s.root' % title, 'RECREATE')

            for cat in categories:
                if 'data' in key.strip().lower() and cat != 'Nominal': continue
                print " -- %s" % cat,

                msk = np.where((data[cat]['isMC'] == isMC) & (data[cat]['DSID'] >= DSIDs[0]) & (data[cat]['DSID'] <= DSIDs[1]))

                empty = (np.sum(msk) == 0) # No events with this variation -> use 'Nominal' instead
                if empty:
                    print "(defaulting to 'Nominal')"
                    msk = np.where((data['Nominal']['DSID'] >= DSIDs[0]) & (data['Nominal']['DSID'] <= DSIDs[1]))
                else:
                    print ""
                    pass

                w = 36.1 if (isMC and DSIDs[0] > 300000 and DSIDs[1] < 400000) else 1.
                if w != 1:
                    print "----> Weight:", w
                    pass

                # Fill tree
                M = [tuple(el) for el in np.hstack((np.atleast_2d(data[cat if not empty else 'Nominal']['m']     [msk].ravel()).T,
                                                    np.atleast_2d(data[cat if not empty else 'Nominal']['weight'][msk].ravel() * w).T)).tolist()]

                arr = np.array(M, dtype=[('mJ',    'f4'),
                                         ('weight', 'f4'),
                                         ])
            
                t = array2tree(arr, title.upper() + '_' + cat)
                t.Write()
                pass

            f.Close()
            pass

        pass

    return


# Main function call.
if __name__ == '__main__':
    main()
    pass
