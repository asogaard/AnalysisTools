import os, sys
from sys import argv

# XML parser.
import xml.etree.ElementTree as ET

# I/O
if len(argv) < 2:
    print "Please specify the GRL path."
    sys.exit()

if not os.path.isfile(argv[1]) :
    print "Provided path '%s' doesn't point to a file." % ( argv[1] )
    sys.exit()

if not argv[1].split('.')[-1] == "xml":
    print "Provided path '%s' doesn't have the right suffix ('.xml', found '%s')." % ( argv[1], "." + argv[1].split('.')[-1] )
    sys.exit()

# Variables.
path = '.'
if len(argv[1].split('/')) > 1:
    path = '/'.join(argv[1].split('/')[0:-1])
    pass
GRLname = argv[1].split('/')[-1]

outputFile = path + '/' + GRLname.replace('.xml', '.txt')
output = open(outputFile, 'w')

tree = ET.parse('/'.join([path, GRLname]))
root = tree.getroot()

# Parse GRL XML.
for LB in root.iter('LumiBlockCollection'):
    output.write( LB.find('Run').text + "\n" )
    for rng in LB.findall('LBRange'):
        output.write( "\t" + str(rng.get('Start')) + "," + str(rng.get('End')) + "\n" )

        pass
    pass

# Finalise.
output.close()
