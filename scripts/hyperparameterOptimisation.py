#!/usr/bin/env python
# -*- coding: utf-8 -*-

''' Script for computing generalised designed de-correlated taggers (GDDT).

...

'''

# Basic.
import sys
import os
import itertools
import pickle
import getpass
from time import gmtime, strftime

#from array import array
from multiprocessing import Pool
import time

# Local utilitiy files.
from common import *

# ROOT.
from ROOT import *

# Try to import necessary external packages.
# Require correct environment setup on lxplus.
try:
    import numpy as np

    from sklearn.kernel_ridge import KernelRidge
    from sklearn.cross_validation import train_test_split, KFold
    from sklearn import preprocessing
    from sklearn.base import clone

    from scipy import ndimage

    import matplotlib as ml
    from matplotlib import rc
    import matplotlib.pyplot as plt
except:
    print "ERROR: Scientific python packages were not set up properly."
    print " $ source ~/pythonenv.sh"  
    print "or see e.g. [http://rootpy.github.io/root_numpy/start.html]."
    pass

# Utility function(s).

# --------------------------------------------------------------------
# Run CV fold, for parameter grid search.
#def runCVfold (clf, X, z, w, binsx, binsy, frac_test, frac_holdout = 0., t = 'mean'):
def runCVfold (clf, X, z, w, binsx, binsy, t, train, test):

    #if seed is None:
    #    raise ValueError("runCVfold: 'seed' cannot be 'None'.")

    if not t in ['mean', 'std']:
        raise ValueError("runCVfold: 't' not in ['mean', 'std'].")

    # Setup coordinate mesh.
    midpointsx = (binsx[1:] + binsx[:-1]) * 0.5
    midpointsy = (binsy[1:] + binsy[:-1]) * 0.5
    meshx, meshy = np.meshgrid(midpointsx, midpointsy)
    meshX = np.column_stack((meshx.ravel(), meshy.ravel()))

    # Generate 'train' and 'test' splits.
    '''
    N = z.size
    # -- Get list of all indices
    indices = np.arange(N)
    # -- Shuffle them _in the same order for each fold_.
    np.random.seed(seed)
    np.random.shuffle(indices)
    # -- Compute the correct batch size.
    #batch_size = int(N / num_folds) + (1 if N % num_folds > 0 else 0)
    # -- Divide the shuffled indices into 'num_folds' batches
    #batches = [indices[i*batch_size:(i+1)*batch_size] for i in range(num_folds)]
    batches = np.array_split(indices, num_folds)
    # -- Extract the test/validation indices for the current fold.
    test_indices  = batches.pop(fold)
    # -- Combine the remaining batches to form the training set.
    train_indices = np.concatenate(batches)
    '''

    X_train = X[train,:]
    X_test  = X[test,:]
    z_train = z[train]
    z_test  = z[test]
    w_train = w[train]
    w_test  = w[test]
    #X_train, X_test, z_train, z_test, w_train, w_test = train_test_split(X, z, w, test_size=frac_test + frac_holdout) 

    # Hold out samples, if requested.
    '''
    if frac_holdout > 0.:

        # Define number of 'test-and-holdout', and actual 'test', examples.
        num_test_and_holdout = len(w_test)
        num_test = int(num_test_and_holdout * frac_test / (frac_test + frac_holdout))
        
        # Generate indices for _actual_ (reduced) 'test' set.
        idx_test = np.random.choice(num_test_and_holdout, num_test, False)
        
        # Resize test arrays.
        X_test = X_test[idx_test,:]
        z_test = z_test[idx_test]
        w_test = w_test[idx_test]
        
        pass
        '''

    # Fill the 'train' profile.
    train_mean, train_err, train_weight = project(X_train[:,0], X_train[:,1], z_train, w_train, binsx, binsy, t)
    
    # Fit the 'train' profile.
    clf.fit(meshX, train_mean.ravel(), sample_weight = train_weight.ravel())
    
    # Fill the 'test' profile.
    test_mean, test_err, test_weight = project(X_test[:,0], X_test[:,1], z_test, w_test, binsx, binsy, t)

    # Get 'test' prediction for the coordinate mesh.
    z_pred = clf.predict(meshX).reshape(test_mean.shape)

    # Compute the CV score: mean squared errors.
    z_test = np.array(test_mean)
    s_test = np.array(test_err)
    
    score = np.mean(np.power((z_pred - z_test) / s_test, 2.))

    # @TEMP
    '''
    #kernel = np.array([[1,1,1],
    #                   [1,0,1],
    #                   [1,1,1]])

    
    #kernel_expectation = ndimage.convolve(test_weight, kernel, mode='constant', cval=0.0) / np.sum(kernel)
    #deviation = np.abs(test_weight - kernel_expectation)
    #relative_deviation = deviation / kernel_expectation

    #print "------ Fold %d/%d | Batch size = %d | Score: %.02f" % (fold + 1, num_folds, z_test.size, score)
    #print clf.dual_coef_
    vmin, vmax = 0.00, 4.00
    fig, (ax1, ax2, ax3, ax4) = plt.subplots(1, 4, sharex = True, sharey = True, figsize = (21,5))
    #fig.suptitle(str(fold + 1))
    im = ax1.imshow(train_mean,                  vmin = vmin, vmax = vmax, origin = 'lower', interpolation = 'none')
    ax2.imshow(z_pred.reshape(train_mean.shape), vmin = vmin, vmax = vmax, origin = 'lower', interpolation = 'none')
    ax3.imshow(test_mean,                        vmin = vmin, vmax = vmax, origin = 'lower', interpolation = 'none')
    ax4.imshow(test_weight,                                                origin = 'lower', interpolation = 'none')

    #fig.subplots_adjust(right=0.85)
    cbar_ax = fig.add_axes([0.92, 0.15, 0.025, 0.7])
    fig.colorbar(im, cax=cbar_ax, label = 'coloours!')
    plt.show()
    '''

    # Return CV score.
    return score


# --------------------------------------------------------------------
# Product of dict entries.
def dict_product(dicts):
    return (dict(itertools.izip(dicts, x)) for x in itertools.product(*dicts.itervalues()))


# Main function.
def main ():

    # ==========================================================================
    # Setup.
    # --------------------------------------------------------------------------
    print "Setup."

    # Initialise summary string.
    summary  = "hyperparameterOptimisation.py was run by %s on %s from here.\n" % (getpass.getuser(), strftime("%a, %d %b %Y %H:%M:%S", gmtime()))
    summary += "  %s\n" % os.getcwd()

    # Initialise output directory, and create if necessary.
    output_dir = './hyperparameterOptimisation/'

    checkMakeDirectory(output_dir)

    # Validate input arguments
    validateArguments(sys.argv)

    # Get data, as prescribed in common.py
    data, Njets = getData(sys.argv)
    
    summary += "\nRead following %d files:\n" % (len(sys.argv) - 1)
    for path in sys.argv[1:]:
        summary += "  %s\n" % path
        pass

    # Initalise substructure variables to use
    #substructure_variables = ['tau21'] # ['tau21', 'D2', 'logD2']
    substructure_variables = ['logD2'] 
    #types = ['mean'] # ['mean', 'std' ]
    types = ['std', 'mean'] 
    
    summary += "\nOptimising hyperparameters for substructure variables:\n"
    for subvar in substructure_variables:
        summary += "  %s\n" % subvar
        pass

    # Initialise axis variable names.
    print "-- Initialise axis variable names."
    varx,  vary  = ('logm', 'logpt')

    summary += "\nProfiling substructure variables with as functions of:\n"
    summary += "  %s\n  %s\n" % (varx, vary)
    
    xlabel = displayNameUnit(varx, latex = True)
    ylabel = displayNameUnit(vary, latex = True)
    
    # Scale axis variables to unit range.
    print "-- Scale axis variables to unit range."
    varxscaled = '%s_scaled' % varx
    varyscaled = '%s_scaled' % vary
    
    xscaler = preprocessing.MinMaxScaler()
    yscaler = preprocessing.MinMaxScaler()

    data[varxscaled] = xscaler.fit_transform(data[varx][:, np.newaxis])
    data[varyscaled] = yscaler.fit_transform(data[vary][:, np.newaxis])

    # Save scalers to file.
    pickle.dump( xscaler, open( output_dir + "scaler_%s.pkl" % varx, "wb" ) )
    pickle.dump( yscaler, open( output_dir + "scaler_%s.pkl" % vary, "wb" ) )

    summary += "\nSaving preprocessing scalers as:\n"
    summary += "  %s\n  %s\n" % (output_dir + "scaler_%s.pkl" % varx, output_dir + "scaler_%s.pkl" % vary)

    # Rename axis variables and -labels.
    varx, vary = varxscaled, varyscaled
    
    xlabel += " (scaled)"
    ylabel += " (scaled)"
        
    # Initialise bin and axis variables.
    print "-- Initialise bin and axis variables."
    nbins     = 50
    bins = np.linspace(0, 1, nbins + 1, True)
    midpoints = (bins[1:] + bins[:-1]) * 0.5
    meshx, meshy = np.meshgrid(midpoints, midpoints)
    meshX = np.column_stack((meshx.ravel(), meshy.ravel()))
    
    summary += "\nUsing the following %d bins along the (scaled) x- and y-axes:\n" % nbins
    summary += "  [%s]\n" % (', '.join("%.2f" % point for point in bins))

    pickle.dump( bins, open( output_dir + 'bins.pkl', "wb" ) )        

    # Initialise ease-to-access data variables.
    x = data[varx]
    y = data[vary]
    w = data['weight']
    X = np.column_stack((x,y))


    # ==========================================================================
    # Perform hyperparameter optimisation.
    # --------------------------------------------------------------------------

    # Initialise base regressor.
    base_clf = KernelRidge(kernel = 'rbf')
    summary += "\nUsing KernelRidge regressors with 'rbf' kernel for the non-parametric fitting.\n"

    # Initialising parameter grid(s) for scan.
    print "Initialise parameter grid(s) for scan."
    parameters = {
        'mean' : {
            'alpha' : np.logspace( -9, -3, 2 * 1 + 1, True), # [1.0E-09], # np.logspace( -1, -1, 0 * 1 + 1, True),
            'gamma' : np.logspace( -2,  4, 6 * 2 + 1, True), # ... 8 * 1 + ...
            #'alpha' : np.logspace(-1, 1, 2 * 1 + 1, True),
            #'gamma' : np.logspace( 0, 4, 4 * 5 + 1, True), # ... 8 * 1 + ...
            #'alpha' : [31., 100.],
            #'gamma' : [10., 100., 1000.],
        },
        'std' : {
            'alpha' : np.logspace( -9, -3, 2 * 1 + 1, True),
            'gamma' : np.logspace( -2,  4, 6 * 2 + 1, True), # ... 8 * 1 + ...
            #'alpha' : np.logspace(  0, 4, 4 * 1 + 1, True),
            #'gamma' : np.logspace(  1, 4, 3 * 3 + 1, True), # ... 8 * 1 + ...
       
        }
    }

    summary += "\nScanning the following parameter grid:\n"
    for t in types:
        summary += "  %s\n" % t
        for key, values in parameters[t].iteritems():
            summary += "    %s: [%s]\n" % (key, ', '.join(['%.03e' % val for val in values]))
            pass
        pass

    # Define 'test' and 'holdout' fraction for CV.
    '''
    frac_test    = 1/3.
    frac_holdout = 1/3.
    
    summary += "\n Using the following CV splitting fractions:\n"
    summary += "  Train:    %.02e\n" % (1. - frac_test - frac_holdout)
    summary += "  Test:     %.02e\n" % (frac_test)
    summary += "  Hold-out: %.02e\n" % (frac_holdout)
    '''
    
    # Initialise CV setting.
    cv_folds = 10
    summary += "\nRunning %d CV folds for each parameter configuration.\n" % cv_folds

    # Initialise multiprocessing pool.
    pool = Pool()
    timeout = 999999999999

    for var, t in itertools.product(substructure_variables, reversed(types)):
        
        print "\nPerforming hyperparameter optimisation for %s of %s." % (t, var)

        # Initialise list to hold results for optimisation wrt. current variable.
        cv_results = list()
        
        # Initialise easy-to-access value array for the current substructure variable.
        z = data[var]

        # Compute the number of jobs to be run in total.
        num_jobs = np.prod([len(l) for l in parameters[t].itervalues()]) * cv_folds
        job_digits = int(np.log10(num_jobs)) + 1
        #seed = 21 # Has to be the same for all CV folds in order to ensure that all examples are used for validation exactly once.

        # Loop parameter configurations in grid.
        for i_config, config in enumerate(dict_product(parameters[t]), start = 1):

            print "-- Running %d CV folds for config: %s" % (cv_folds, ', '.join('%s = %.02e' % (key, val) for key, val in config.iteritems()))
            clf = clone(base_clf)
            setConfig(clf, config)

            # Run 'cv_fold' folds in parallel.
            start = time.time()
            #args = [clf, X, z, w, bins, bins, frac_test, frac_holdout]
            args = [clf, X, z, w, bins, bins, t]
            results = list()
            #for fold in xrange(cv_folds):
            cv = KFold(z.shape[0], n_folds = cv_folds, shuffle = True)
            for train, test in cv:
                #results.append( pool.apply_async(runCVfold, args) )
                results.append( pool.apply_async(runCVfold, args + [train, test]) )
                pass
            cv_scores = [result.get(timeout = timeout) for result in results]
            end = time.time()

            # Append current configuration and CV scores to list of results.
            cv_results.append((config, cv_scores))
            
            # Print progress.
            cv_score_mean = np.mean(cv_scores)
            cv_score_std  = np.std (cv_scores)
            print "---> [%*d/%*d] Mean CV score: %6.03f +/- %5.03f (%4.1fs)" % (job_digits, i_config * cv_folds, job_digits, num_jobs, cv_score_mean, cv_score_std, end - start)

            pass


        # Plotting the CV curves for the parameter scans
        xpar = 'gamma'
        xvals = dict()
        yvals = dict()
        yerrs = dict()
        keys  = list()
        for config, cv_scores in cv_results:
            key = ', '.join(['%s: %s' % (displayName(key, latex = True), sci_notation(val, 2)) for key, val in config.items() if key != xpar])
            if key not in yvals:
                xvals[key] = np.array([])
                yvals[key] = np.array([])
                yerrs[key] = np.array([])
                keys.append(key)
                pass
            
            xvals[key] = np.append(xvals[key], config[xpar])
            yvals[key] = np.append(yvals[key], np.mean(cv_scores))
            yerrs[key] = np.append(yerrs[key], np.std (cv_scores))
            pass
        
        title = 'Hyperparameter optimisation for '
        if t == 'mean':
            title += "mean(%s)" % displayName(var, latex = True)
        else:
            title += "RMS(%s)" % displayName(var, latex = True)
            pass

        fig = plt.figure()
        #fig.suptitle('Hyperparameter optimisation for %s of %s' % (t, displayName(var, latex = True)), fontsize = 18)
        fig.suptitle(title, fontsize = 18)
        for ikey, key in enumerate(keys):
            plt.plot(xvals[key], yvals[key], color = colours[ikey % len(colours)], linewidth = 2.0, label = key)
            plt.fill_between(xvals[key], yvals[key] + yerrs[key], yvals[key] - yerrs[key], color = colours[ikey % len(colours)], linewidth = 2.0, alpha = 0.3)
            pass
        
        plt.grid()
        plt.xlabel(displayName(xpar, latex = True))
        plt.ylabel("Cross-validation scores (mean squared error)")
        plt.xscale('log')
        plt.yscale('log')
        #plt.ylim([1.0E-01, 1.0E+02 if t == 'mean' else 1.0E+01])
        plt.ylim([0.2, 20.])
        if len(keys) > 1:
            plt.legend(prop = {'size':16})
            pass
        plt.savefig(output_dir + 'plot_cv_curves__%s_%s__vs__%s__%s.pdf' % (t, var, varx, vary))
        plt.show()
        

        # Get optimal config.
        best_config_index = cv_results.index( min(cv_results, key = lambda pair : np.mean(pair[1])) )
        best_config = cv_results[best_config_index][0]

        print best_config
        print "Score for best config: %6.03f +/- %5.03f" % (np.mean(cv_results[best_config_index][1]), np.std(cv_results[best_config_index][1]))

        output_best_config = 'optimal_hyperparameters__%s__%s__vs__%s__%s.pkl' % (t, var, varx, vary)
        summary += "\nOptimal configuration found for %s of %s was: %s\n" % (t, var, str(best_config))
        summary += "Score for optimal configuration: %6.03f +/- %5.03f\n" % (np.mean(cv_results[best_config_index][1]), np.std(cv_results[best_config_index][1]))
        summary += "Saving optimal hyperparameters to: '%s'\n" % (output_dir + output_best_config)

        # Save optimal hyperparameters.
        pickle.dump( best_config, open( output_dir + output_best_config, "wb" ) )        

        pass # end: loop substructure variables.

    # Saving README file for current run.
    README = open(output_dir + "README.txt", "w")
    README.write(summary)
    README.close()
    
    return


# Main function call.
if __name__ == '__main__':
    main()
    pass
