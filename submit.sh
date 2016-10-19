#!/bin/bash

# -- Variables, default.
MAXNUMBATCHES=20 # Will submit *no more that* $MAXNUMBATCHES jobs
EXECUTE=""
INPUT=()

ADD_INPUT=false

# -- Commandline arguments.
while [[ $# > 1 ]]; do
key="$1"
case $key in
    -b|--batches)
    MAXNUMBATCHES="$2"
    shift # past argument
    ;;
    -e|--execute)
    EXECUTE="$2"
    shift # past argument
    ;;
    #-i|--input)
    #INPUT="$2"
    #shift # past argument
    #;;
    --default)
    DEFAULT=YES
    ;;
    *)
        # unknown option
        ADD_INPUT=true
    ;;
esac
if [[ $ADD_INPUT == true ]]; then
    INPUT+=("$1")
    ADD_INPUT=false
fi
shift # past argument or value
done

if [[ -n $1 ]]; then
    INPUT+=("$1")
fi

# -- Checking.
if [[ -z "$EXECUTE" ]]; then
echo "Please specify a program to execute using the -e or --execute option."
elif [[ -z "$INPUT" ]]; then
echo "Please specify a (list of) input(s)."
elif (( $MAXNUMBATCHES < 1 )); then
echo "Please specify a positive maximal number of batches (${MAXNUMBATCHES})."
elif (( $MAXNUMBATCHES > 20 )); then
echo "Please specify a reasonable (< 21) maximal number of batches (${MAXNUMBATCHES})."
else 
echo ""

# -- Get size of all input files.
OIFS=$IFS
IFS=' '
FILESIZES=()
let SUMFILESIZE=0
let MAXFILESIZE=0
for THISINPUT in ${INPUT[@]}; do
    LSSTRING=`ls -l ${THISINPUT}`
    FIELDS=($LSSTRING)
    let FILESIZE=${FIELDS[4]}
    FILESIZES+=($FILESIZE)
    let SUMFILESIZE=$(( $SUMFILESIZE + $FILESIZE ))
    let MAXFILESIZE=$(( $FILESIZE > $MAXFILESIZE ? $FILESIZE : $MAXFILESIZE ))
done

# -- Set number of batches to the optimal number (at most $MAXNUMBATCHES)
let OPTNUMBATCHES=$(( $SUMFILESIZE / $MAXFILESIZE + 1 ))
let NUMBATCHES=$(( $OPTNUMBATCHES > $MAXNUMBATCHES ? $MAXNUMBATCHES : $OPTNUMBATCHES ))
echo " Submitting ${#INPUT[@]} files to ${NUMBATCHES} batches out of at most ${MAXNUMBATCHES}."

# -- Use python script to distribute input files equally into batches, by file size.
DISTRIBUTE_SCRIPT="scripts/distributeEqually.py"
if [ ! -f ${DISTRIBUTE_SCRIPT} ]; then
    echo "Error: The script '${DISTRIBUTE_SCRIPT}' couldn't be found. I need it to create smart batches."
# @TODO: Make fallback to less smart batch structure?
else 

BATCHINDICES=(`python ${DISTRIBUTE_SCRIPT} -n ${NUMBATCHES} ${FILESIZES[@]}`)

# -- Create log directory, if necessary.
LOGDIR="logs"
mkdir -p ${LOGDIR}

# -- Submit optimised batches.
IFS=$'\n'
for BATCHINDEX in $(seq 1 $NUMBATCHES); do

    # Create array to store the input files in this batch.
    BATCH=()

    # Collect all inputfiles, which have been assigned to the current batch.
    let INPUTINDEX=0
    for INPUTFILE in ${INPUT[@]}; do
	if (( ${BATCHINDICES[$INPUTINDEX]} == $BATCHINDEX )); then
	    BATCH+=($INPUTFILE)
	fi
	INPUTINDEX=$(( $INPUTINDEX + 1 ))
    done
    
    # Submit current batch.
    IFS=' ' # Make sure that the input files are separated by spaces, not newlines.
    nohup $EXECUTE ${BATCH[@]} >${LOGDIR}/log${BATCHINDEX}.out 2>&1 &
done

# -- Reset the internal fields separator.
IFS=$OIFS

echo ""

fi # endif: 'scripts/distributeEqually.py' exists
fi # endif: numberof batches is reasonable
