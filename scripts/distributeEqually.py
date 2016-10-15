# Dear emacs, this is -*- python -*-
import sys

def putInSmallestBatch (value, batches):
    ''' Function to put a given value into the smallest batch, out of a given set. '''

    # Compute size of each batch.
    sizes = map(sum, batches)

    # Get index of smalles batch.
    smallestIndex = sizes.index(min(sizes))

    # Add current value to batch.
    batches[smallestIndex].append(value)

    # Return index of batch.
    return smallestIndex


def distributeEqually (values, numBatches):
    ''' Function to distribute a number of values equally into a given number of batches. '''

    # Define vectors to store indicies and batch contents.
    batchIndices = [-1 for _ in range(len(values))]
    batches      = [[] for _ in range(numBatches)]

    # Loop through values from, largest to smallest.
    sortedIndices = sorted(range(len(values)), key = lambda idx: values[idx], reverse = True)
    for idx in sortedIndices:
        # -- Store index of batch in which to put current value.
        batchIndices[idx] = putInSmallestBatch(values[idx], batches)
        pass

    # Return array of batch indices.
    return batchIndices

# Main function definition.
def main ():

    # Default number of batches.
    numBatches = 10

    # Input commandline arguments.
    argv = sys.argv[1:]
    
    # Try to get number of batches as commandline input
    idx = None
    try:
        idx = argv.index('-n')
    except:
        pass
    try:
        idx = argv.index('-N')
    except:
        pass
    if idx is not None:
        numBatches = int(argv[idx+1])
        argv = argv[:idx] + argv[idx+2:]
        pass

    # Create array of values to be distributed.
    values = [int(arg) for arg in argv if not arg.startswith('-')]
    
    # Get batch indices
    batchIndices = distributeEqually(values, numBatches)

    # Make 1-indices.
    batchIndices = [idx + 1 for idx in batchIndices]

    # Print batch indices.
    print " ".join( map(str, batchIndices) )

    return

# Main function call.
if __name__ == '__main__':
    main()
    pass
