# Dear Emacs, this is -*- python -*-

import os, sys
import glob

try:
	import pyAMI.atlas.api as atlasAPI
	import pyAMI.client
	client = pyAMI.client.Client('atlas')
	#atlasAPI.init()
except:
	print ""
	print "Remember to setup AMI:"
	print " $ source /afs/cern.ch/atlas/software/tools/pyAMI/setup.sh"
	print " $ ami auth"
	print ""
	sys.exit(0)

def main ():

	print "" 

	write = False

	if len(sys.argv) < 2:
		print "Please provide the path to a list of dataset for which to get AMI information. Use as:"
		print " $ python %s ./path/to/datasets.txt [more/paths]" % sys.argv[0]
		print ""
		return

	paths = sys.argv[1:]

	lists = [path for g in paths for path in glob.glob(g)]
	
	# Get all dataset ID's (DSIDs) to query.
       	patterns = set()
	for l in lists:
		with open(l, 'r') as f:
			for line in f:
				line = line.strip()
				if line == '' or line.startswith('#'):
					continue
				patterns.add(line)
				pass # end: loop lines
			pass
		pass # end: loop lists
	
	# Open output file.
	if write:
		outfile = open('share/weightsMC.csv', 'w')
		pass
	
	# Looping patterns (DSIDs).
	for pattern in sorted(list(patterns)):

		DAODs = atlasAPI.list_datasets(client, 
					       patterns=[pattern],
					       fields=['job_config','prodsys_status'])
		
		for DAOD in DAODs:
			
			if DAOD['prodsys_status'] != "ALL EVENTS AVAILABLE":
				continue
			'''
			MC: Need Xsec, GenFiltEff, Nevents (k-factor?)		
			'''
			
			DSID = 0
			xSec = 0.
			nEve = 0
			fEff = 0.
			Phys = ""
			
			# Get original AOD.
			ancestors = atlasAPI.get_dataset_prov(client, DAOD['ldn'])
			AODs = ancestors['node']

			for AOD in AODs:
				
				if AOD['dataType'] != 'AOD' or int(AOD['distance']) > 0:
					continue
					
				AODname = AOD['logicalDatasetName']
				parentAOD = atlasAPI.list_datasets(client, patterns=[AODname])
				
				if len(parentAOD) != 1:
					print "Error: Could not find single matching AOD dataset for:", AODname
					continue
				
				info     = atlasAPI.get_dataset_info(client, parentAOD[0]['ldn'])[0]
				infoDAOD = atlasAPI.get_dataset_info(client, DAOD['ldn'])[0]

				if '.recon.' in info['logicalDatasetName']: info = infoDAOD

				DSID = int  ( info['datasetNumber']     )
				xSec = float( info['crossSection']      ) * 1e+06 # [nb] -> [fb]
				nEve = int  ( info['totalEvents']       ) 
				if 'genFiltEff' in info:
					fEff = float( info['genFiltEff'] )
				else:
					fEff = 1.0
					pass
				Phys = str  ( info['physicsShort']      )
				
				break
			
			printline = "%6d, %20.10f, %8d, %0.4e, %s" % ( DSID,  xSec, nEve, fEff, Phys )
			print printline

			if write:
				outfile.write(printline + "\n")
				pass
		
			pass # end: loop DAODs
		pass # end: loop patterns
	
	if write:
		outfile.Close()
		pass

	return


if __name__ == '__main__':
	main()
	pass
