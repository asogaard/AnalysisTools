#!/usr/bin/env python

import os, sys
import glob

try:
	import pyAMI.client
	client = pyAMI.client.Client('atlas')
except:
	print ""
	print "Remember to setup AMI:"
	print " $ source /afs/cern.ch/atlas/software/tools/pyAMI/setup.sh"
	print " $ ami auth"
	print ""
	sys.exit(0)


def tryGetInfo (client, basedataset, fields):

	# Get original AOD.
	command = ['GetDatasetProv', 'logicalDatasetName=%s' % basedataset]
        result  = client.execute(command, format = 'dict_object')

	dataset = None
	for row in result.get_rows():
		if int(row['distance']) > 0:
			continue
		if row['dataType'] == 'AOD':
			dataset = row['logicalDatasetName']
			break
		pass

	if not dataset:
		print "Warning: Original AOD for dataset '%s' was not found." % basedataset
		return


	# Initialise output dict.
	output = { field: None for field in fields }

	# (1) Get dataset info from AMI.
	command = ['GetDatasetInfo', 'logicalDatasetName=%s' % dataset]
	result  = client.execute(command, format = 'dict_object')

	for row in result.get_rows():
		for field in fields:
			if field in row and output[field] is None:
				output[field] = row[field]
				pass
			pass
		pass

	# -- Break early if all fields are set already.
	if not (False in [val is None for _, val in output.iteritems()]):
		return output

	# (2) Get physics parameters for dataset from AMI.
	command = ['GetPhysicsParamsForDataset', 'logicalDatasetName=%s' % dataset]
	result  = client.execute(command, format = 'dict_object')

	for row in result.get_rows():
		name = row['paramName']
		if name in fields and output[name] is None:
			output[name] = row['paramValue']
			pass
		pass

	# Status message.
	for key, val in output.iteritems():
		if val is None:
			print "Warning: Key '%s' was not set." % key
			pass
		pass

	return output


def main ():

	# Checks.
	if len(sys.argv) < 2:
		print "" 
		print "Please provide the path to a list of dataset for which to get AMI information. Use as:"
		print " $ python %s ./path/to/datasets.txt [more/paths]" % sys.argv[0]
		print ""
		return

	# Whether to write the output lines to file.
	write = True

	# Get (list of) input file(s), possibly including wildcards.
	paths = sys.argv[1:]

	# Get globbed (list of) input file(s).
	lists = [path for g in paths for path in glob.glob(g)]
	
	# Get all datasets) to query.
       	datasets = set()
	for l in lists:
		with open(l, 'r') as f:
			for dataset in f:
				dataset = dataset.strip()
				if dataset == '' or dataset.startswith('#'):
					continue
				datasets.add(dataset)
				pass
			pass
		pass
	
	# Open output file.
	if write:
		outfile = open('share/weightsMC.csv', 'w')
		pass
	
	# Loop datasets for which to get information.
	for dataset in sorted(list(datasets)):
		info = tryGetInfo(client, dataset, ['crossSection', 'totalEvents', 'physicsShort', 'datasetNumber', 'genFiltEff'])

		DSID = int  (info['datasetNumber']     )
		xSec = float(info['crossSection']      ) * 1e+06 # [nb] -> [fb]
		nEve = int  (info['totalEvents']       ) 
		fEff = float(info['genFiltEff'])
		Phys = str  (info['physicsShort'])
			
		printline = "%6d, %1.7e, %8d, %0.4e, %s" % ( DSID,  xSec, nEve, fEff, Phys )
		print printline

		if write:
			outfile.write(printline + "\n")
			pass
		

		pass # end: loop datasets
	
	if write:
		outfile.Close()
		pass

	return


if __name__ == '__main__':
	main()
	pass
