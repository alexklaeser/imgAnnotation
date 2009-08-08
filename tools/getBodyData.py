#!/home/clear/local/bin64/python

import sys
import re

HELP_USAGE = """

Usage: getBodyData.py <annotationFile>

"""

regKeyVal = re.compile(r"([a-zA-Z0-9_\-]+)\s*:\s*(.+)($|#)")

def addObject(bodies, iFile, iObj):
	# if the current object has an body_id entry, keep it
	if iObj.has_key('body_id'):
		iBody = iObj['body_id']
		if not bodies.has_key(iFile):
			bodies[iFile] = {}
		if not bodies[iFile].has_key(iBody):
			bodies[iFile][iBody] = []
		bodies[iFile][iBody].append(iObj)

def readBodies(annotationFile):
	bodies = {}
	iFile = ''
	iObj = {}
	with open(annotationFile) as file:
		# read in all lines in the file
		for iLine in file:
			# got a new filename
			m = regKeyVal.match(iLine)
			if iLine and not iLine.startswith('#') and m:
				# got a new key value 
				k = m.group(1)
				v = m.group(2)
				
				# check for a new filename
				if 'file' == k:
					iFile = v
					addObject(bodies, iFile, iObj)
					iObj = {}
				elif 'object' == k:
					addObject(bodies, iFile, iObj)
					iObj = {}
				else:
					iObj[k] = v
				
		# process the last object
		addObject(bodies, iFile, iObj)

	return bodies

def main(argv):
	# check options
	args = sys.argv[1:]
	if len(args) == 0 or args[0] in ("--help", "-h"):
		print HELP_USAGE
		return

	# read all annotated bodies
	bodies = readBodies(args[0])
	for iFile, iBodies in bodies.iteritems():
		print '#file:', iFile
		for iBody in iBodies.itervalues():
			print '#body'
			for iObj in iBody:
				print iObj['type'], iObj['view'], iObj['h_direction'], iObj['bbox']


if __name__ == "__main__":
	main( sys.argv )


