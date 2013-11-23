#ifndef opencv_FUNCTIONS_H_
#define opencv_FUNCTIONS_H_

#include <opencv/IplImageWrapper.h>
#include <geometry/Box.h>
#include <geometry/Size.h>
#include <boost/random.hpp>

IplImageWrapper getSubImage(const IplImageWrapper& srcImg, const Box<int>& srcBox, const Size<int>& dstSize, bool fillBorders = false, bool setMask = false);

/**
 * Computes the integral images, such that dstImg(x, y) = sum(x', y') for all
 * x', y' >= 0 with x' <= x and y' <= y.
 *
 * In contrast to the OpenCV version, the image sizes of the original and integral
 * image are equal.
 *
 * NOTE: works only for double format and one channel images.
 */
void computeIntegralImage(const IplImageWrapper& srcImg, IplImageWrapper& dstImg);

double getIntegralRegion(const IplImageWrapper& integralImg, const Box<int>& box);

//double getSubPixel(const IplImageWrapper& img, const Point& point);

template <typename RandomGenerator>
void addGaussianNoise(IplImageWrapper& img, double sigma, RandomGenerator& rng);

/**
 * expects the image to be already initialized to the correct size
 * and copies then the raw data pointer according to the image size 
 * (taking into account the aligned image rows of IplImage).
 * Note that OpenCV needs the BGR format.
 */
void copyRawData(IplImageWrapper& img, unsigned char* pSrc, int lineSizeBytes = 0);

IplImageWrapper convert2GrayFloatImg(const IplImageWrapper& orgImg);
void convert2GrayFloatImg(const IplImageWrapper& orgImg, IplImageWrapper& grayFloatImg);

void setZeroOutsideMask(IplImageWrapper& img, const Box<int>& mask);

#include "functions.hpp"

#endif /*opencv_FUNCTIONS_H_*/
