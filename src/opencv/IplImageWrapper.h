#ifndef IPLIMAGEWRAPPER_H_
#define IPLIMAGEWRAPPER_H_

#include <opencv/cxcore.h>
#include <cstdlib>
#include <string>
#include <geometry/Box.h>
#include <boost/optional.hpp>

/**
 * Wrapper in order to use IplImages more easily in STL containers .. for that we
 * need a destructor that releases an image correctly using cvReleaseImage().
 * We also want to have smart pointers with a reference count in order to copy
 * containers.
 *
 * NOTE: Do not use generic algorithms with this class! (See discussions on auto_ptr,
 *       similar thoughts/problems apply to this smart pointer!)
 */

	
class IplImageWrapper {
protected:
	IplImage* _img;
	std::size_t* _nRefs;
	boost::optional<Box<int> > _mask;

protected:
	void decrementAndFree();

public:
	IplImageWrapper(IplImage *newImg = NULL, bool isOwner = true);

	IplImageWrapper(CvSize size, int depth, int channels);

	IplImageWrapper(std::string fileName);

	IplImageWrapper(const IplImageWrapper& newImg);

	~IplImageWrapper();

	IplImageWrapper clone() const;

	IplImageWrapper& operator=(const IplImageWrapper& img);

	operator IplImage*();

	operator const IplImage*() const;

	operator const bool() const;

	operator bool();

	IplImage* operator->();

	const IplImage* operator->() const;

	IplImage* getReference();

	const IplImage* getReference() const;

	std::size_t numOfReferences() const;

	bool hasMask() const;

	Box<int> getMask() const;

	void setMask(const Box<int>& mask);

	void clearMask();
};

#include "IplImageWrapper.hpp"

#endif /*IPLIMAGEWRAPPER_H_*/
