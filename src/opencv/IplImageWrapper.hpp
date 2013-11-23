#include "IplImageWrapper.h"
#include <opencv/highgui.h>

#include <iostream>
//using std::std::cout;
//using std::std::endl;

inline
void IplImageWrapper::decrementAndFree() {
//if (IplImageWrapperDebug) {
//	static int ii(0);
//	std::cout << "IplImageWrapper::decrementAndFree() -- address: " << _img << " nRefs: " << (*_nRefs) << " - 1 = " << (*_nRefs - 1) << std::endl;
//	++ii;
//	if (ii > 5)
//		throw int(1);
//}
	if (_nRefs) {
		--(*_nRefs);
		if (*_nRefs == 0) {
			if (_img)
				cvReleaseImage(&_img);
			delete _nRefs;
		}
	}
}

inline
IplImageWrapper::IplImageWrapper(IplImage *newImg, bool isOwner)
	: _img(newImg), _nRefs(isOwner ? new std::size_t(1) : 0), _mask()
{
//std::cout << "IplImageWrapper::IplImageWrapper() -- address: " << _img << " nRefs: " << (*_nRefs) << std::endl;
}

inline
IplImageWrapper::IplImageWrapper(CvSize size, int depth, int channels)
	: _img(cvCreateImage(size, depth, channels)), _nRefs(new std::size_t(1)), _mask()
{
//std::cout << "IplImageWrapper::IplImageWrapper() -- address: " << _img << " nRefs: " << (*_nRefs) << std::endl;
}

inline
IplImageWrapper::IplImageWrapper(std::string fileName)
	: _img(cvLoadImage(fileName.c_str())), _nRefs(new std::size_t(1)), _mask()
{
//std::cout << "IplImageWrapper::IplImageWrapper() -- address: " << _img << " nRefs: " << (*_nRefs) << std::endl;
}

inline
IplImageWrapper::IplImageWrapper(const IplImageWrapper& newImg)
	: _img(newImg._img), _nRefs(newImg._nRefs), _mask(newImg._mask)
{
//if (IplImageWrapperDebug) std::cout << "IplImageWrapper::IplImageWrapper() -- address: " << _img << " nRefs: " << (*_nRefs) << " + 1 = " << (*_nRefs + 1) << std::endl;
	++(*_nRefs);
}

inline
IplImageWrapper::~IplImageWrapper()
{
//if (IplImageWrapperDebug) std::cout << "IplImageWrapper::~IplImageWrapper() -- address: " << _img << " nRefs: " << (*_nRefs) << std::endl;
	decrementAndFree();
}

inline
IplImageWrapper IplImageWrapper::clone() const
{
	IplImage *iplImg = cvCloneImage(this->getReference());
	IplImageWrapper clonedImg(iplImg);
	if (_mask)
		clonedImg.setMask(*_mask);
	return clonedImg;
}

inline
IplImageWrapper::operator IplImage*()
{
	return _img;
}

inline
IplImageWrapper::operator const IplImage*() const
{
	return _img;
}

inline
IplImageWrapper::operator const bool() const
{
	return _img;
}

inline
IplImageWrapper::operator bool()
{
	return _img;
}

inline
IplImage* IplImageWrapper::getReference()
{
	return _img;
}

inline
const IplImage* IplImageWrapper::getReference() const
{
	return _img;
}

inline
std::size_t IplImageWrapper::numOfReferences() const
{
	return _nRefs ? 0 : (*_nRefs);
}

inline
IplImage* IplImageWrapper::operator->()
{
	return _img;
}

inline
const IplImage* IplImageWrapper::operator->() const
{
	return _img;
}

inline
bool IplImageWrapper::hasMask() const
{
	return _mask;
}

inline
Box<int> IplImageWrapper::getMask() const
{
	return *_mask;
}

inline
void IplImageWrapper::setMask(const Box<int>& mask)
{
	_mask = boost::optional<Box<int> >(mask);
}

inline
void IplImageWrapper::clearMask()
{
	_mask = boost::optional<Box<int> >();
}

