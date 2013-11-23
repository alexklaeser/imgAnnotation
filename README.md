Image Annotation Tool
=====================

This tool facilitates the annotation of objects in images with bounding boxes. It has been written in Qt, it therefore is portable to different platforms.

The features of this tool are as follows:
* annotation of objects with bounding boxes
* support for easily modifying, copying and pasting bounding boxes
* zoom in/out
* per object, freely chosen "fix points" can mark additional points on objects (these can be used to align annotated objects more accurately)
* the rotation of an object can be annotated (with fix points)
* each object can have freely chosen properties (numbers, text)
* the tool can even by used to visualize detection results (ordered by a score value)
* a subset of visible objects in an image can be chosen
* support for properties of images (numbers, text)

If OpenCV is enabled, the following features are available, as well:
* selection of objects by filtering over properties
* alignment and extraction of selected objects
* generation of random samples per annotated object by applying various distortion methods

In the Image Annotation Tool, you have a menu help / shortcuts that gives you a short list of the most common short cuts. Otherwise, keep the cursor for a longer while over buttons or text fields (in the extract dialog) to get some more hints and information.

Compilation
===========

On Ubuntu, I used the following lines to compile the tool:

	  sudo apt-get install build-essential libqt4-core libqt4-dev libqt4-gui qt4-dev-tools libboost1.53-all-dev
	  qmake imgAnnotation.pro
	  make
	  # compilation is done and you can launch the tool
	  ./imgAnnotation

Screenshots
===========

Everyone loves screenshots, so here they are...

Main window
-----------

Main window with two bounding boxes annotated, for each bounding box alignment points have been specified:
![](https://raw.github.com/alexklaeser/imgAnnotation/master/screenshots/main_window.png)

Image extraction dialog
-------------------------

Filter criteria for objects:
![](https://raw.github.com/alexklaeser/imgAnnotation/master/screenshots/extraction1_object_selection.png)

Parameters for image normalization:
![](https://raw.github.com/alexklaeser/imgAnnotation/master/screenshots/extraction2_normalization.png)

For the final image extraction, a subset of images can be selected:
![](https://raw.github.com/alexklaeser/imgAnnotation/master/screenshots/extraction3_subset_selection.png)

Sampling parameters for extracted images:
![](https://raw.github.com/alexklaeser/imgAnnotation/master/screenshots/extraction4_resampling.png)

Format of alignment data file
=============================

The format in which the database is saved is as follows. All annotations belonging to a specific images follow a line indicating the image filename:

file: &lt;imageFileName&gt;

The path can be absolute or relative (to the position of the annotation database file). 

An object anotation is indicated by a line of the format:

object: &lt;ID&gt;

The ID is a unique and positive integer number or -1 in which case an unique ID is assigned when the database file is loaded into the annotation tool.

All properties belonging to an object follow the 'object:...' line. Their format is given by:

&lt;key&gt;: &lt;value&gt;

In general, keys and values can be chosen arbitrarily. Nevertheless, there are two properties which are of particular interest to the tool and which need to be in a specific format:

bbox: &lt;x&gt;, &lt;y&gt;, &lt;width&gt;, &lt;height&gt;
can also be
bbox: &lt;x&gt;, &lt;y&gt;, &lt;width&gt; x &lt;height&gt;

and

fixpoints: &lt;x1&gt;, &lt;y1&gt;, &lt;x2&gt;, &lt;y2&gt;, ...

The property 'bbox' indicates position and size of the object by a rectangle. X, y, width, and height can be floating point values. The property 'fixpoints' indicates x and y position of a set of points that can optionally be used to align annotated objects and to normalize them with respect to rotation. For "fixpoints", the number of points needs to be even.

License conditions
==================

The Image Annotation Tool for image annotations with pixelwise masks

Copyright (C) 2007 Alexander Klaeser

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

