# Dear emacs, this is -*- python -*-
''' Script for computing generalised designed de-correlated taggers (GDDT).

...

'''

# Basic.
import sys
import itertools
from array import array
from functools import reduce
from multiprocessing import Pool
import time

# Local utilitiy files.
from AStyle     import *
from AFunctions import *
from wquantiles import *

# ROOT.
from ROOT import *

# Try to import necessary external packages.
# Require correct environment setup on lxplus.
try:
    import numpy as np
    from root_numpy import fill_profile, fill_hist, hist2array

    from scipy import interpolate, stats, ndimage

    from sklearn.kernel_ridge import KernelRidge
    from sklearn.grid_search import GridSearchCV
    from sklearn.cross_validation import train_test_split
    from sklearn import preprocessing
    from sklearn.base import clone
    from sklearn.ensemble import BaggingRegressor

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
# ...
def computeProfileVec (vecx, vecy, vecz, binsx, binsy, weights = None, option = '', cls = TProfile2D):
    matrix = np.column_stack((vecx, vecy, vecz))
    return computeProfileMat(matrix, binsx, binsy, weights, option, cls)

# --------------------------------------------------------------------
# ...
def computeHistVec (vecx, vecy, binsx, binsy, weights = None, option = '', cls = TH2F):
    matrix = np.column_stack((vecx, vecy))
    return computeProfileMat(matrix, binsx, binsy, weights, option, cls)

# --------------------------------------------------------------------
# ...
def computeProfileMat (matrix, binsx, binsy, weights = None, option = '', cls = TProfile2D):
    
    if weights is None:
        weights = np.ones((matrix.shape[0],))
    elif len(weights) != matrix.shape[0]:
        ValueError("Number of samples (%d) and weights (%d) do not agree." % (matrix.shape[0], len(weights)))
        pass

    nx = len(binsx) - 1
    ny = len(binsy) - 1

    if cls == TProfile2D:
        profile = cls('profile', "", nx, binsx, ny, binsy, option)
        fill_profile(profile, matrix, weights = weights)
    else:
        profile = cls('hist',    "", nx, binsx, ny, binsy)
        fill_hist(profile, matrix, weights = weights)
        pass

    means  = np.zeros((nx,ny))
    errors = np.zeros((nx,ny))
    for (i,j) in itertools.product(xrange(nx), xrange(ny)):
        means [i,j] = profile.GetBinContent(j + 1, i + 1)
        errors[i,j] = profile.GetBinError  (j + 1, i + 1)
        pass

    return means, errors


# --------------------------------------------------------------------
# Run CV fold, for parameter grid search.
def runCVfold (clf, X, z, w, binsx, binsy, frac_test, frac_holdout = 0.):

    # Define minimum number of MC entries each bin needs to have in order to be considered. Done to remove isolated bins with excessive weights.
    min_entries = 5

    # Setup coordinate mesh.
    midpointsx = (binsx[1:] + binsx[:-1]) * 0.5
    midpointsy = (binsy[1:] + binsy[:-1]) * 0.5
    meshx, meshy = np.meshgrid(midpointsx, midpointsy)
    meshX = np.column_stack((meshx.ravel(), meshy.ravel()))

    # Generate 'train' and 'test' splits.
    X_train, X_test, z_train, z_test, w_train, w_test = train_test_split(X, z, w, test_size=frac_test + frac_holdout) 

    # Hold out samples, if requested.
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
    
    # Fill the 'train' profile.
    train_mean, train_err = computeProfileVec(X_train[:,0], X_train[:,1], z_train, binsx, binsy, w_train)
    train_entries, _      = computeHistVec   (X_train[:,0], X_train[:,1], binsx, binsy)
    
    train_err[np.where(train_err == 0)]               = 9999.
    train_err[np.where(train_entries <= min_entries)] = 9999.
    train_weight = np.power(train_err, -1.)
    
    # Fit the 'train' profile.
    clf.fit(meshX, train_mean.ravel(), sample_weight = train_weight.ravel())
    
    # Fill the 'test' profile.
    test_mean, test_err = computeProfileVec(X_test[:,0], X_test[:,1], z_test, binsx, binsy, w_test)
    test_entries, _     = computeHistVec   (X_test[:,0], X_test[:,1], binsx, binsy)
    
    test_err[np.where(test_err == 0)]               = 9999.
    test_err[np.where(test_entries <= min_entries)] = 9999.
    test_weight = np.power(test_err, -1.)
    
    # Get 'test' prediction for the coordinate mesh.
    z_pred = clf.predict(meshX).reshape(test_mean.shape)

    # Compute the CV score: mean squared errors.
    z_test = np.array(test_mean)
    w_test = np.array(test_weight)
    
    score = np.mean(np.power((z_pred - z_test) * w_test, 2.))

    # Return CV score.
    return score

# --------------------------------------------------------------------
# Return 'iterable' in batches of (at most) 'n'.
def batch (iterable, n = 1):
    l = len(iterable)
    for ndx in range(0, l, n):
        yield iterable[ndx:min(ndx + n, l)]
        pass
    pass

# --------------------------------------------------------------------
# Use classifier tp produce predictions (wrapper function)
def predict (clf, data):
    return clf.predict(data)

# --------------------------------------------------------------------
# Use classifier to produce prediction in batches.
def batchPredict (clf, data, batch_size = 100):

    # Define number of example for which to produce prediction.
    num_examples = data.shape[0]

    # Define output vector.
    output = np.zeros((num_examples,))

    # Comput number of batches to be process.
    num_batches = int(num_examples / float(batch_size)) + 1
    
    # Loop batches for prediction.
    for indices in batch(np.arange(num_examples), batch_size):

        # Make prediction for current batch
        output[indices] += clf.predict(data[indices,:])
        pass

    return output

# --------------------------------------------------------------------
# Produce prediction using asynchronous processes.
def asyncPredict (clf, data, num_processes = 10):

    # Create multiprocessing pool.
    pool = Pool()
    
    # Get number of examples for which to produce predictions.
    num_examples = data.shape[0]

    # Compute suitable batch size.
    batch_size = int(num_examples/float(num_processes)) + 1

    # Loop batches.
    results = list()
    for indices in batch(np.arange(num_examples), batch_size):
        
        # Submit prediction as asynchronous process.
        args = [clf, data[indices,:]]
        results.append( pool.apply_async(predict, args) )
        pass

    # Collect predictions.
    predictions = [result.get(timeout = 99999999999) for result in results]

    return np.hstack(predictions)


# --------------------------------------------------------------------
# Set configuration of any object based on dict.
def setConfig(obj, config):
    for attr, val in config.iteritems():
        setattr(obj, attr, val)
        pass
    return obj


# --------------------------------------------------------------------
# Product of dict entries.
def dict_product(dicts):
    return (dict(itertools.izip(dicts, x)) for x in itertools.product(*dicts.itervalues()))


# Epsilon-type value, for avoiding divergences.
eps = 1E-09

# Main function.
def main ():

    # ==========================================================================
    # Setup.
    # --------------------------------------------------------------------------
    print "Setup."

    # pyplot settings.
    rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
    rc('font',**{'family':'serif','serif':['Palatino']})
    rc('text', usetex=True)
    ml.rcParams['image.cmap'] = 'inferno'

    coloursROOT = [kViolet + 7, kAzure + 7, kTeal, kSpring - 2, kOrange - 3, kPink]
    colours     = [(tc.GetRed(), tc.GetGreen(), tc.GetBlue()) for tc in map(gROOT.GetColor, coloursROOT)]

    # Options
    show = True
    save = False

    # Load cross sections files.
    print "-- Load cross sections file."
    xsec = loadXsec('../share/sampleInfo.csv')
    
    # Get list of file paths to plot from commandline arguments.
    print "-- Get list of input paths."
    paths = [arg for arg in sys.argv[1:] if not arg.startswith('-')]
    
    # Specify which variables to get.
    print "-- Specify variables to read."
    treename = 'BoostedJet+ISRgamma/Fatjets/Nominal/BoostedRegime/Postcut'
    prefix   = ''
    
    substructurevars = ['plot_object_%s' % var for var in ['tau21', 'D2']]
    getvars  = ['m', 'pt'] + substructurevars
    
    # Load data.
    print "-- Load data."
    values = loadDataFast(paths, treename, getvars, prefix, xsec,
                          keepOnly = (lambda DSID: 361039 <= DSID <= 361062 )
                          )
    
    # @TOFIX: Temporary fix for inconstistent branch names
    print "-- Rename variables (temporary)."
    for var in substructurevars:
        values[var.replace('plot_object_', '')] = values.pop(var)
        pass
    substructurevars = [var.replace('plot_object_', '') for var in substructurevars]
    
    # Check output.
    print "-- Check output exists."
    if not values:
        print "WARNING: No values were loaded."
        return
    
    # Discard unphysical jets.
    print "-- Discard unphysical jets. No other jets should be excluded."
    msk_good = reduce(np.intersect1d, (np.where(values['pt']    > 0),
                                       np.where(values['m']     > 0),
                                       #np.where(values['m']     < 300.), # @TEMP
                                       np.where(values['tau21'] > 0),
                                       np.where(values['D2']    > 0)))
    
    for var, arr in values.items():
        values[var] = arr[msk_good]
        pass
    
    # Initialise total number of jets.
    print "-- Initialise number of (good) jets."
    Njets = len(values[getvars[0]])
    
    # Compute new variables.
    print "-- Compute new variables:"
    values['rho'] = np.log(np.power(values['m'], 2) / np.power(values['pt'], 2))
    EPS = np.ones(values['m'].shape) * 1.0E-09
    values['logm']  = np.log(np.maximum(EPS, values['m']))
    values['logpt'] = np.log(np.maximum(EPS, values['pt']))
    
    values['logtau21'] = np.log(values['tau21'])
    values['logD2']    = np.log(values['D2'])
    
    substructurevars = ['tau21', 'logD2']
    
    # Initialise axis cariable names.
    print "-- Initialise axis variable names."
    varx,  vary  = ('logm', 'logpt')
    
    xlabel = displayNameUnit(varx, latex = True)
    ylabel = displayNameUnit(vary, latex = True)
    
    # Scale to unit range
    print "-- Scale axis variables to unit range."
    varxscaled = '%s_scaled' % varx
    varyscaled = '%s_scaled' % vary
    
    xscaler = preprocessing.MinMaxScaler()
    yscaler = preprocessing.MinMaxScaler()
    values[varxscaled] = xscaler.fit_transform(values[varx][:, np.newaxis])
    values[varyscaled] = yscaler.fit_transform(values[vary][:, np.newaxis])
    
    varx, vary = varxscaled, varyscaled
    
    xlabel += " (scaled)"
    ylabel += " (scaled)"
    
    
    # ==========================================================================
    # Perform  (G)DDT.
    # --------------------------------------------------------------------------
    print ""
    print "Perform (G)DDT."

    # Initialise bin and axis variables.
    print "-- Initialise bin and axis variables."
    nbins     = 50
    bins = np.linspace(0, 1, nbins + 1, True)
    midpoints = (bins[1:] + bins[:-1]) * 0.5
    meshx, meshy = np.meshgrid(midpoints, midpoints)
    meshX = np.column_stack((meshx.ravel(), meshy.ravel()))
    
    x = values[varx]
    y = values[vary]
    w = values['weight']
    X = np.column_stack((x,y))

    for var in substructurevars:
        
        print "-- Fitting %d data points for %s vs. %s x %s." % (Njets, var, varx, vary)
 
        z = values[var]
       
        # Get mean profile and error.
        print "---- Get mean profile and error."
        mean, mean_err = computeProfileVec(x, y, z, bins, bins, w)
                
        # Initialising parameter grid(s) for scan.
        print "---- Initialise parameter grid(s) for scan."
        parameters = {
            'alpha' : np.logspace(-1, 1, 2 * 1 + 1, True),
            'gamma' : np.logspace( 1, 3, 2 * 1 + 1, True), # ... 8 * 1 + ...
            }

        # Define 'test' and 'holdout' fraction for CV.
        frac_test    = 1/3.
        frac_holdout = 1/3.
        
        # Scan parameter grid.
        base_clf = KernelRidge(kernel = 'rbf')
        cv_folds = 5
        cv_results = list()
        np.random.seed(21)
        pool = Pool()

        # Compute the number of jobs to be run in total.
        num_jobs = np.prod([len(l) for l in parameters.itervalues()]) * cv_folds
        job_digits = int(np.log10(num_jobs)) + 1

        # Loop parameter configurations.
        print "\nPerforming hyperparameter optimisation through grid search:"
        for i_config, config in enumerate(dict_product(parameters), start = 1):

            print "-- Running %d CV folds for config: %s" % (cv_folds, ', '.join('%s = %.02e' % (key, val) for key, val in config.iteritems()))
            clf = clone(base_clf)
            setConfig(clf, config)

            ''' Parallel running: [http://stackoverflow.com/questions/20548628/how-to-do-parallel-programming-in-python]
            '''

            # Run 'cv_fold' folds in parallel.
            start = time.time()
            args = [clf, X, z, w, bins, bins, frac_test, frac_holdout]
            results = list()
            for fold in xrange(cv_folds):
                results.append( pool.apply_async(runCVfold, args) )
                pass
            cv_scores = [result.get(timeout = 99999999999) for result in results]
            end = time.time()

            # Append current configuration and CV scores to list of results.
            cv_results.append((config, cv_scores))
            
            # Print progress.
            cv_score_mean = np.mean(cv_scores)
            cv_score_std  = np.std (cv_scores)
            print "---- [%*d/%*d] Mean CV score: %6.03f +/- %5.03f (%4.1fs)" % (job_digits, i_config * cv_folds, job_digits, num_jobs, cv_score_mean, cv_score_std, end - start)

            pass


        # Plotting the CV curves for the parameter scans
        xpar = 'gamma' # 'C'
        xvals = dict()
        yvals = dict()
        yerrs = dict()
        keys  = list()
        for config, cv_scores in cv_results:
            key = ', '.join(['%s: %.1e' % (key, val) for key, val in config.items() if key != xpar])
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
        
        for ikey, key in enumerate(keys):
            plt.plot(xvals[key], yvals[key], color = colours[ikey % len(colours)], linewidth = 2.0, label = key)
            plt.fill_between(xvals[key], yvals[key] + yerrs[key], yvals[key] - yerrs[key], color = colours[ikey % len(colours)], linewidth = 2.0, alpha = 0.3)
            pass
        
        plt.grid()
        plt.xlabel(xpar)
        plt.ylabel("CV validation scores, mean squared error")
        plt.xscale('log')
        plt.yscale('log')
        if len(keys) > 1:
            plt.legend()
            pass
        plt.show()
        

        # Get optimal config.
        best_config_index = cv_results.index( min(cv_results, key = lambda pair : np.mean(pair[1])) )
        best_config = cv_results[best_config_index][0]

        print best_config
        print "Score for best config: %6.03f +/- %5.03f" % (np.mean(cv_results[best_config_index][1]), np.std(cv_results[best_config_index][1]))

        # Initialise easy-access arrays.
        '''
        Xgrid   = np.column_stack((meshx.ravel(), meshy.ravel() ))
        Xsample = X[sample,:]
        
        mean        = mean.ravel()
        mean_sample = z[sample]
        
        zerr[np.where(zerr == 0)] = 9999.
        w       = np.power(zerr, -1.).ravel()
        wsample = w[sample]
        '''

        # Ensembling.
        print "\nEnsembling KKR classifiers with optimal parameters."
        Nensemble = 2

        clf = clone(base_clf)
        setConfig(clf, best_config)

        ensemble = list()
        np.random.seed()
        for i in range(Nensemble):
            print "-- Classifier %d." % (i + 1)
            
            # Clone best classifier.
            ensemble.append(clone(clf))
            
            # Generate new sample.
            sample = np.random.choice(Njets, int( 0.667 * Njets ), True) #False)
            
            # Initialise easy-access arrays.
        #Xsample = np.column_stack((values[varx][sample], values[vary][sample]))
            zsample = values[var][sample]
            wsample = values['weight'][sample]
            
            profilesample, profileerr = computeProfileVec(values[varx][sample], values[vary][sample], zsample, bins, bins, wsample)
            profile_entries, _        = computeHistVec   (values[varx][sample], values[vary][sample], bins, bins)

            profileerr[np.where(profileerr == 0.)]     = 9999.
            profileerr[np.where(profile_entries <= 5)] = 9999.
            wsample = np.power(profileerr, -1).ravel()
            # For the ensemble classifier.
            
            #print Xsample.shape
            #print profilesample.ravel().shape
            #print wsample.shape
            ensemble[-1].fit(meshX, profilesample.ravel(), wsample)
            
            pass
    #'''
        print "Done!"


        # Compute modified variable using asynchronous processes.
        print "\nPredicting modified variable using asynchronous processes."
        mvar = var + '_GDDT'
        num_processes = 10
        values[mvar] = np.zeros((Njets,))
        for icl, cl in enumerate(ensemble, start = 1):
            print "-- Classifier %d." % icl
            start = time.time()
            values[mvar] += asyncPredict(cl, X, num_processes)
            end  = time.time()
            print "---- Elapsed: %.1fs" % (end - start)
            pass
        values[mvar] /= float(len(ensemble))
        print ""

        # Plotting the profiles and densities
        fig, ax = plt.subplots(2, 2, sharex = True, sharey = True, figsize = (11,9))
        fig.suptitle(r'%s profiles' % displayName(var, latex = True), fontsize=21)
        
        # -- Mesh
        nbins     = 50
        nbinsfine = 300
        vmin, vmax = 0.25, 0.65
        
    #meshx,     meshy     = np.meshgrid(np.linspace(0, 1, nbins,     True),
    #                                   np.linspace(0, 1, nbins,     True))
        meshxfine, meshyfine = np.meshgrid(np.linspace(0, 1, nbinsfine, True),
                                           np.linspace(0, 1, nbinsfine, True))
        
        # -- (1)
    #bins = np.linspace(0, 1, nbins + 1, True)
    #z, zerr = computeProfileVec(values[varx], values[vary], values[var], bins, bins, values['weight'])
        mean = mean.reshape(meshx.shape)
        
        ax[0,0].pcolormesh(meshx, meshy, mean, vmin = vmin, vmax = vmax)
        ax[0,0].set_title('Profile of all data points')
        ax[0,0].set_xlim([0,1])
        ax[0,0].set_ylim([0,1])
        ax[0,0].set_ylabel(ylabel)
        ax[0,0].set_title('Sampling points')
        
        # -- (2)
        print "Computing zpred."
    #zpred = ensemble.predict(X)
        '''

        
        zpred = np.zeros((Njets,1))
        for i, cl in enumerate(ensemble):
            print "-- Classifier %d." % (i + 1)
            zpred += cl.predict(X).reshape(zpred.shape)
            pass
        zpred /= float(len(ensemble))
    '''
        zpred = values[mvar]
        zpredprofile,_ = computeProfileVec(X[:,0], X[:,1], zpred, bins, bins, w)
        
        ax[0,1].pcolormesh(meshx, meshy, zpredprofile, vmin = vmin, vmax = vmax)
        ax[0,1].set_title('Profile of predictions for data points')
        
        # -- (3)
        print "Duuuun."
    #zdiff, _ = computeProfileVec(X[:,0], X[:,1], z.ravel() - zpred, bins, bins, w)
        zdiff = mean.ravel() - zpredprofile.ravel()
        mean_err[np.where(mean_err == 0)] = 9999.
        zpull = zdiff / mean_err.ravel()
        ax[1,0].pcolormesh(meshx, meshy, zpull.reshape(meshx.shape), vmin = -2., vmax = 2., cmap = 'RdBu')
        ax[1,0].set_xlabel(xlabel)
        ax[1,0].set_ylabel(ylabel)
        ax[1,0].set_title(r'Residual pulls ($\pm 2 \sigma$)')
        
        # -- (4)
        print "Computing meshzfine."
        meshXfine = np.column_stack((meshxfine.ravel(), meshyfine.ravel()))
    #meshzfine = ensemble.predict(meshXfine).reshape(meshxfine.shape)
    #'''
        meshzfine = np.zeros(meshxfine.shape)
        for i, cl in enumerate(ensemble):
            print "-- Classifier %d." % (i + 1)
            meshzfine += cl.predict(meshXfine).reshape(meshxfine.shape)
            pass
        meshzfine /= float(len(ensemble))
    #'''
        print "Done!"
        
        im = ax[1,1].pcolormesh(meshxfine, meshyfine, meshzfine, vmin = vmin, vmax = vmax)
        ax[1,1].set_xlabel(xlabel)
        ax[1,1].set_title('Ensamble-averaged SVR estimator,\nbased on sampling')
        
        fig.subplots_adjust(right=0.85)
        cbar_ax = fig.add_axes([0.90, 0.15, 0.025, 0.7])
        fig.colorbar(im, cax=cbar_ax, label = displayNameUnit(var, True))
        
        print "Residual pulls:"
    #msk_pull = np.where(zerr < 9999.)
        print zpull
        print "-- Mean: %.03f" % np.mean(zpull)#[msk_pull])
        print "-- RMS:  %.03f" % np.std (zpull)#[msk_pull])
        
        plt.show()
        
        
        
        
        
        print "Computing "
        
        # ...

        pass

    # ...
    
    return        
    



if __name__ == '__main__':
    main()
    pass
