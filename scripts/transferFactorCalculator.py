# Dear emacs, this is -*- python -*-
''' Function for calculating DDT pass/fail transfer factor.

...

'''

import sys

# Main function.
def transferFactorCalculator (path):

    # Scipy bi-linear spline representation data object.
    # Cf. [https://docs.scipy.org/doc/scipy-0.14.0/reference/generated/scipy.interpolate.bisplrep.html]
    tck = [ np.array([    1.25,   1.25,   1.25,   1.25,    6.75,    6.75,    6.75,    6.75]), # Knots, x
            np.array([  450.,   450.,   450.,   450.,   1950.,   1950.,   1950.,   1950.]),   # Knots, y
            np.array([  0.18261346,  0.2174425 ,  0.37762574,  0.10284952,  0.26108651, # Spline coefficients
                       -0.14541588,  0.05701827,  0.27361263,  0.54531852,  0.77814774,
                        0.2479843 ,  0.23509468, -0.04597834, -0.47218929,  0.01928886,
                       -0.09066243]), 
            3, # Spline degree, x
            3] # Spline degree, y

    # Limits for the transfer factor map. Return zero if outside.
    limits = { 'x': (  1.,    7.),
               'y': (400., 2000.) }

    with open(path) as f:
        for line in f:

            # Read coordinates as: " x,y\n"
            coords = map(float, [field.strip() for field in line.split(',')])

            # Check limits.
            if (limits['x'][0] <= coords[0] <= limits['x'][1]) and \
               (limits['y'][0] <= coords[1] <= limits['y'][1]):
                # Calculate and return transfer factor.
                print interpolate.bisplev(coords[0], coords[1], tck)
            else:
                # Return fallback value.
                print 0.
                pass

            pass # end: loop lines
        pass # end: open file

    return

if __name__ == '__main__':

    # Try to import necessary packages.
    # Require correct environment setup on lxplus.
    try:
        import numpy as np
        from scipy import interpolate

        # Perform input check
        if len(sys.argv) < 2:
            print "Please specify as path, as"
            print " $ python %s <path-to-coordinates-file>" % (sys.argv[0])
        else:
            transferFactorCalculator(sys.argv[1])
            pass

    except:
        print "ERROR: numpy and scipy were not set up properly."
        print "See e.g. [http://rootpy.github.io/root_numpy/start.html]."
        pass

    pass
