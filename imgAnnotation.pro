# the target
TARGET = imgAnnotation
DEFINES += NO_OPENCV # comment out this line if you want to use opencv
OPENCV_ROOT = $(HOME)
#OPENCV_ROOT = c:/OpenCV2.0
#OPENCV_SUFFIX = 200

# some project options
TEMPLATE = app
CONFIG += release \
          warn_on \
          qt

# dirs for automatically generated files
MOC_DIR = src/tmp
OBJECTS_DIR = src/tmp
UI_DIR = src/ui

# source files
FORMS += src/*.ui
HEADERS += src/*.h \
	src/geometry/*.h \
	src/numeric/*.h
SOURCES += src/*.cpp

# lib/include dirs
INCLUDEPATH += src $${INCLUDEPATH}

# add opencv libraries and include path
!contains(DEFINES, NO_OPENCV) {
	LIBS += -lcv$${OPENCV_SUFFIX} -lcxcore$${OPENCV_SUFFIX} -L$${OPENCV_ROOT}/lib
	INCLUDEPATH += $${OPENCV_ROOT}/include
}
