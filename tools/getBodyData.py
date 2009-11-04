#!/home/clear/local/bin64/python

import sys
import re
import math

HELP_USAGE = """

Usage: getBodyData.py <annotationFile>

"""

regKeyVal = re.compile(r"([a-zA-Z0-9_\-]+)\s*:\s*(.+)($|#)")
regSplit = re.compile(r"\s*,\s*")

def addObject(bodies, iFile, iObj):
	# if the current object has an body_id entry, keep it
	if iObj.has_key('body_id'):
		iBody = iObj['body_id']
		if not bodies.has_key(iFile):
			bodies[iFile] = {}
		if not bodies[iFile].has_key(iBody):
			bodies[iFile][iBody] = {}
		bodies[iFile][iBody][iObj['type']] = iObj

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

# add output values to list for specified type
def addColumns(myList, type, iBody):
	if iBody.has_key(type):
		iObj = iBody[type]
		l = regSplit.split(iObj['bbox'])
		view = 'NULL'
		if iObj.has_key('view') and iObj.has_key('h_direction'):
			if 'front' == iObj['view']:
				view = 'front'
			elif 'rear' == iObj['view']:
				view = 'rear'
			elif 'side' == iObj['view'] and 'left' == iObj['h_direction']:
				view = 'left'
			elif 'side' == iObj['view']:
				view = 'right'
		sigma = math.log(math.log(float(l[3])))
		myList.extend([l[0], l[1], str(sigma), view])
	else:
		myList.extend(['NULL'] * 4)

def main(argv):
	# check options
	args = sys.argv[1:]
	if len(args) == 0 or args[0] in ("--help", "-h"):
		print HELP_USAGE
		return

	# read all annotated bodies
	bodies = readBodies(args[0])
	print "# format: faceX, faceY, faceSigma, faceView, headX, ..., upperBodyX, ..."
	print "# view = { front, left, right, rear }"
	print "# non existing body parts have the value 'NULL'"
	for iFile, iBodies in bodies.iteritems():
		print '#file:', iFile
		for iBody in iBodies.itervalues():
			out = []
			addColumns(out, 'face', iBody)
			addColumns(out, 'head', iBody)
			addColumns(out, 'headShoulders', iBody)
			print " ".join(out)


if __name__ == "__main__":
	main( sys.argv )


