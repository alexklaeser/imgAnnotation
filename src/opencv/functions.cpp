#include "functions.h"
#include <opencv/cv.h>
#include <iostream>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include <geometry/Box.hpp>

using std::cout;
using std::endl;

IplImageWrapper getSubImage(const IplImageWrapper& srcImg, const Box<int>& srcBox, const Size<int>& dstSize, bool fillBorders, bool setMask)
{
	// allocate memory for images
	IplImageWrapper tmpImg(cvSize(srcBox.getWidth(), srcBox.getHeight()), srcImg->depth, srcImg->nChannels);
	IplImageWrapper outImg(cvSize(dstSize.getWidth(), dstSize.getHeight()), srcImg->depth, srcImg->nChannels);
	Box<int> imgBox(0, 0, srcImg->width, srcImg->height);

	// check whether the srcBox lies within the image
	if (!imgBox.intersects(srcBox)) {
		cvSetZero(outImg);
		return outImg;
	}

	// see from where we have to copy how much (srcBox could lie outside the image plane)
	Box<int> croppedBox = srcBox & imgBox;
	int xTmpStart = std::max<int>(0, -srcBox.getLeft());
	int xTmpEnd = xTmpStart + std::max<int>(0, std::min<int>(tmpImg->width, croppedBox.getWidth()));
	int yTmpStart = std::max<int>(0, -srcBox.getTop());
	int yTmpEnd = yTmpStart + std::max<int>(0, std::min<int>(tmpImg->height, croppedBox.getHeight()));
	int xSrcStart = std::max<int>(0, srcBox.getLeft());
	int ySrcStart = std::max<int>(0, srcBox.getTop());

	// see how many bytes we have to copy per pixel
	int typeSize = 0;
	switch(srcImg->depth)
	{
		case IPL_DEPTH_8U:
		case IPL_DEPTH_8S:
			typeSize = 1;
			break;
		case IPL_DEPTH_16U:
		case IPL_DEPTH_16S:
			typeSize = 2;
			break;
		case IPL_DEPTH_32S:
		case IPL_DEPTH_32F:
			typeSize = 4;
			break;
		case IPL_DEPTH_64F:
			typeSize = 8;
			break;
		default:
			typeSize = 0;
	}
	int bytesPerPixel = typeSize * srcImg->nChannels;

	// copy the pixels row by row from the source into the temp image
	assert(xTmpEnd - xTmpStart + xSrcStart <= srcImg->width);
	assert(yTmpEnd - yTmpStart + ySrcStart <= srcImg->height);
	assert(xTmpEnd - xTmpStart + xSrcStart >= 0);
	assert(yTmpEnd - yTmpStart + ySrcStart >= 0);
	cvSetZero(tmpImg);
	for (int iy = 0; iy < yTmpEnd - yTmpStart; ++iy) {
		memcpy(tmpImg->imageData + tmpImg->widthStep * (iy + yTmpStart) + bytesPerPixel * xTmpStart,
				srcImg->imageData + srcImg->widthStep * (iy + ySrcStart) + bytesPerPixel * xSrcStart,
				bytesPerPixel * (xTmpEnd - xTmpStart));
	}
	
	// make borders by copying pixel values
	// borders left and right
	if (fillBorders && yTmpStart < yTmpEnd && xTmpStart < xTmpEnd) {
		char* pSrc(0), *pDest(0), *pLine(0);
		for (int iy = 0; iy < yTmpEnd - yTmpStart; ++iy) {
			pLine = tmpImg->imageData + tmpImg->widthStep * (iy + yTmpStart);
			pSrc = pLine + bytesPerPixel * xTmpStart;
			pDest = pLine;
			for (int ix = 0; ix < xTmpStart; ++ix, pDest += bytesPerPixel)
				memcpy(pDest, pSrc, bytesPerPixel);
			pSrc = pLine + bytesPerPixel * (xTmpEnd - 1);
			pDest = pLine + bytesPerPixel * (xTmpEnd);
			for (int ix = xTmpEnd; ix < tmpImg->width; ++ix, pDest += bytesPerPixel)
				memcpy(pDest, pSrc, bytesPerPixel);
		}
		
		// borders top and bottom
		pSrc = tmpImg->imageData + tmpImg->widthStep * yTmpStart;
		pDest = tmpImg->imageData;
		for (int iy = 0; iy < yTmpStart; ++iy, pDest += tmpImg->widthStep)
			memcpy(pDest, pSrc, tmpImg->widthStep);
		pSrc = tmpImg->imageData + tmpImg->widthStep * (yTmpEnd - 1);
		pDest = tmpImg->imageData + tmpImg->widthStep * yTmpEnd;
		for (int iy = yTmpEnd; iy < tmpImg->height; ++iy, pDest += tmpImg->widthStep)
			memcpy(pDest, pSrc, tmpImg->widthStep);
	}

	// resize the temp image to the final size
	cvResize(tmpImg, outImg, CV_INTER_AREA);

	// set the mask if wanted
	if (setMask && !imgBox.contains(srcBox)) {
		// get position in extract image
		Box<int> mask = croppedBox;
		mask.translate(-1 * srcBox.getTopLeft());
		
		// resize mask
		mask.scale(double(outImg->width) / tmpImg->width, double(outImg->height) / tmpImg->height);
		outImg.setMask(mask);
	}
	
	return outImg;
}

template <typename ValueType>
void _computeIntegralImage(const IplImageWrapper& srcImg, IplImageWrapper& dstImg)
{
	// process the first row
	ValueType* xSrc = (ValueType*)srcImg->imageData;
	double* xDst = (double*)dstImg->imageData;
	double rowSum = 0;
	for (int x = 0; x < dstImg->width; ++x, ++xSrc, ++xDst) {
		rowSum += *xSrc;
		*xDst = rowSum;
	}

	// starting at the second row, apply dynamic programming in order to
	// compute always the next based on the previous sum
	char* rowSrc = srcImg->imageData + srcImg->widthStep;
	char* rowDst = dstImg->imageData + dstImg->widthStep;
	char* rowDstRowAbove = dstImg->imageData;
	for (int y = 1; y < dstImg->height; ++y,
			rowSrc += srcImg->widthStep, rowDst += dstImg->widthStep, rowDstRowAbove += dstImg->widthStep) {
		// get double pointers
		xSrc = (ValueType*)rowSrc;
		xDst = (double*)rowDst;
		double* xDstRowAbove = (double*)rowDstRowAbove;

		// keep a copy of the previous sum in the row
		rowSum = 0;

		// process the row
		for (int x = 0; x < dstImg->width; ++x, ++xSrc, ++xDst, ++xDstRowAbove) {
			rowSum += *xSrc;
			*xDst = rowSum + *xDstRowAbove;
		}
	}
}

void computeIntegralImage(const IplImageWrapper& srcImg, IplImageWrapper& dstImg)
{
	// method is only working for one channel images and double precision floating point
	if (srcImg->nChannels != 1)
		throw std::runtime_error("computeIntegralImage(): srcImg is not a gray image!");
	if (dstImg->nChannels != 1)
		throw std::runtime_error("computeIntegralImage(): dstImg is not a gray image!");
	if (dstImg->depth != IPL_DEPTH_64F)
		throw std::runtime_error("computeIntegralImage(): format of dstImg is not double!");

	// check the dimensions .. should be equal
	if (dstImg->width != srcImg->width || dstImg->height != srcImg->height)
		throw std::runtime_error("computeIntegralImage(): srcImg and dstImg do not have the same dimensions!");

	if (srcImg->depth == IPL_DEPTH_32F)
		_computeIntegralImage<float>(srcImg, dstImg);
	else if (srcImg->depth == IPL_DEPTH_64F)
		_computeIntegralImage<double>(srcImg, dstImg);
	else
		throw std::runtime_error("computeIntegralImage(): format of srcImg is neither double nor float!");
}

void copyRawData(IplImageWrapper& img, unsigned char* pSrc, int lineSizeBytes) {
	std::size_t bytesPerPixel = 0;
	switch (img->depth) {
	case IPL_DEPTH_8U:
	case IPL_DEPTH_8S:
		bytesPerPixel = 1;
		break;
	case IPL_DEPTH_16U:
	case IPL_DEPTH_16S:
		bytesPerPixel = 2;
		break;
	case IPL_DEPTH_32S:
	case IPL_DEPTH_32F:
		bytesPerPixel = 4;
		break;
	case IPL_DEPTH_64F:
		bytesPerPixel = 8;
		break;
	default:
		bytesPerPixel = 0;
	}
	std::size_t bytesPerLine = bytesPerPixel * img->nChannels * img->width;
	std::size_t strideDest = img->widthStep;
	std::size_t strideSrc = lineSizeBytes > 0 ? lineSizeBytes : bytesPerLine;
	char* pDest = img->imageData;
	for (int iRow = 0; iRow < img->height; ++iRow, pSrc += strideSrc, pDest += strideDest)
		memcpy(pDest, pSrc, bytesPerLine);
}


IplImageWrapper convert2GrayFloatImg(const IplImageWrapper& orgImg)
{
	IplImageWrapper img;
	convert2GrayFloatImg(orgImg, img);
	return img;
}

void convert2GrayFloatImg(const IplImageWrapper& orgImg, IplImageWrapper& grayFloatImg)
{
	assert(orgImg);
	
	// make sure that the image has the correct size and type
	if (!grayFloatImg || grayFloatImg->width != orgImg->width || grayFloatImg->height != orgImg->height 
			|| grayFloatImg->depth != IPL_DEPTH_32F || grayFloatImg->nChannels != 1) {
		CvSize size = cvGetSize(orgImg);
		grayFloatImg = IplImageWrapper(size, IPL_DEPTH_32F, 1);
	}

	// convert to gray valued image (if necessary)
	CvSize size = cvGetSize(orgImg);
	IplImageWrapper grayImg;
	if (1 == orgImg->nChannels) {
		grayImg = orgImg;
	}
	else if (3 == orgImg->nChannels) {
		grayImg = IplImageWrapper(size, orgImg->depth, 1);
		cvCvtColor(orgImg, grayImg, CV_BGR2GRAY);
	}
	else
		assert(false);

	// convert the image to the right scale (i.e., to [0; 1])
	if (IPL_DEPTH_8U == grayImg->depth)
		cvConvertScale(grayImg, grayFloatImg, 1.0 / 255.0);
	else if (IPL_DEPTH_8S == grayImg->depth)
		cvConvertScale(grayImg, grayFloatImg, 1.0 / 255.0, 0.5);
	else if (IPL_DEPTH_16U == grayImg->depth)
		cvConvertScale(grayImg, grayFloatImg, 1.0 / 65535.0);
	else if (IPL_DEPTH_16S == grayImg->depth)
		cvConvertScale(grayImg, grayFloatImg, 1.0 / 65535.0, 0.5);
	else if (IPL_DEPTH_32S == grayImg->depth)
		cvConvertScale(grayImg, grayFloatImg, 1.0 / 4294967295.0, 0.5);
	else
		// floating point .. is assumed to be already normalized
		cvConvertScale(grayImg, grayFloatImg);
	
	if (orgImg.hasMask())
		grayFloatImg.setMask(orgImg.getMask());
}


template <typename T>
void _setZeroOutsideMask(IplImageWrapper& img, const Box<int>& mask)
{
	if (!mask.isValid())
		return;
	
	// set the pixel values outside the mask to 0
	char* pRow = img->imageData;
	for (int iy = 0; iy < img->height; ++iy, pRow += img->widthStep) {
		// initiate pixel pointers
		T* pImg = (T*)pRow;

		// go through all pixels in a row
		for (int ix = 0; ix < img->width; ++ix) {
			// set the magnitude to zero if current point doesn't lie in the mask
			Point<int> pos(ix, iy);
			if (!mask.contains(pos)) {
				for (int ic = 0; ic < img->nChannels; ++ic, ++pImg)
					*pImg = T(0);
			}
			else
				pImg += img->nChannels;
		}
	}
}

void setZeroOutsideMask(IplImageWrapper& img, const Box<int>& mask)
{
	if (IPL_DEPTH_8U == img->depth)
		_setZeroOutsideMask<uint8_t>(img, mask);
	else if (IPL_DEPTH_8S == img->depth)
		_setZeroOutsideMask<int8_t>(img, mask);
	else if (IPL_DEPTH_16U == img->depth)
		_setZeroOutsideMask<uint16_t>(img, mask);
	else if (IPL_DEPTH_16S == img->depth)
		_setZeroOutsideMask<int16_t>(img, mask);
	else if (IPL_DEPTH_32S == img->depth)
		_setZeroOutsideMask<int32_t>(img, mask);
	else if (IPL_DEPTH_32F == img->depth)
		_setZeroOutsideMask<float>(img, mask);
	else if (IPL_DEPTH_64F == img->depth)
		_setZeroOutsideMask<double>(img, mask);
	else
		// wrong format
		throw std::runtime_error("setZeroOutsideMask(): unknown IPL depth format!");
}
