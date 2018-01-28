#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" Script for producing histograms for the statistical procedure. """

# Basic
import sys, glob
import re

# Scientific
import ROOT 
from root_numpy import tree2array, fill_hist

import numpy as np


# Main function.
def main ():

    inputdir  = '/eos/atlas/user/a/asogaard/Analysis/2016/BoostedJetISR/StatsInputs/2017-06-28/'
    outputdir = '/eos/atlas/user/a/asogaard/Analysis/2016/BoostedJetISR/StatsInputs/2017-07-10/'
    
    inputpaths  = glob.glob(inputdir + '/ISRgamma_*.root')
    outputpaths = [p.replace(inputdir, outputdir).replace('ISRgamma', 'hist_ISRgamma') for p in inputpaths]

    for inputpath, outputpath in zip(inputpaths,outputpaths):
        print "Processing '%s'" % inputpath

        infile  = ROOT.TFile(inputpath,  'READ')
        outfile = ROOT.TFile(outputpath, 'RECREATE')
        categories = [key.GetName() for key in infile.GetListOfKeys()]

        for category in categories:
            print "-- '%s'" % category
            tree = infile.Get(category)
            array = tree2array(tree)
            #hist = ROOT.TH1F(category, "", 30, 100, 250)
            hist = ROOT.TH1F(category, "", 32, 100, 260)
            fill_hist(hist, array['mJ'], weights=array['weight'])
            
            # TF shape/norm ...

            outfile.cd()
            hist.Write()
            pass

        outfile.Write()
        outfile.Close()
        infile.Close()
        pass

    return
    

# Main function call.
if __name__ == '__main__':
    main()
    pass
