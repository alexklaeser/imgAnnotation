#!/bin/bash

if [ "$1" = --help ]; then
	echo
	echo "You can use the convertAnnotation script as tool for piping. Pipe"
	echo "on the stdin the old annotation file and it puts on the stdout the"
	echo "file converted to the new format."
	echo
	exit
fi

sed -e '
	# the bounding box
	s/minXYMaxXY/bbox/

	# the object type and the object id
	s/object/object: -1\ntype/

	# the views
	/viewFrontal/ i view: front
	/viewSide/ i view: side

	# the directions
	/directionRight/ i h_direction: right
	/directionLeft/ i h_direction: left

	# the characters
	/characterBuffy/ i character: buffy
	/characterRiley/ i character: riley
	/characterWillow/ i character: willow
	/characterGiles/ i character: giles
	/characterXander/ i character: xander
	/characterOthers/ i character: others

	# set the tags as comment line
	s/^tags:/# tags:/'
