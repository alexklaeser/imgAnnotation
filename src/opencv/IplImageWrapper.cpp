#include "IplImageWrapper.h"
#include <geometry/Box.hpp>
//#include <opencv/highgui.h>

IplImageWrapper& IplImageWrapper::operator=(const IplImageWrapper& img) {
	if (_img != img._img) {
//cout << "IplImageWrapper::operator=() -- address: " << _img << " nRefs: " << (*_nRefs) << " + 1 = " << (*_nRefs + 1) << endl;
		decrementAndFree();
		_img = img._img;
		_nRefs = img._nRefs;
		if (_nRefs)
			++(*_nRefs);
	}
	_mask = img._mask;
	return *this;
}
