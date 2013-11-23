#include <cassert>
#include <boost/numeric/conversion/converter.hpp>
#include <iostream>

template <typename ValueType, typename RandomGenerator>
void _addGaussianNoise(IplImageWrapper& img, double sigma, RandomGenerator& rng)
{
//	assert(IplImageDepth<ValueType>() == img->depth);

	typedef boost::normal_distribution<double> NormalDistribution;
	typedef boost::variate_generator<RandomGenerator&, NormalDistribution> NormalGenerator;

	NormalGenerator rngNorm(rng, NormalDistribution(0, sigma));

	// go through each row, column, and channel and add Gaussian noise
	char* pRow = img->imageData;
	for (int y = 0; y < img->height; ++y, pRow += img->widthStep) {
		ValueType* pData = reinterpret_cast<ValueType*>(pRow);
		for (int x = 0; x < img->width; ++x)
			for (int c = 0; c < img->nChannels; ++c, ++pData) {
				double tmp = rngNorm() + *pData;
				tmp = std::min<double>(tmp, boost::numeric::bounds<ValueType>::highest());
				tmp = std::max<double>(tmp, boost::numeric::bounds<ValueType>::lowest());
				*pData = static_cast<ValueType>(tmp);
			}
	}
}

template <typename RandomGenerator>
void addGaussianNoise(IplImageWrapper& img, double sigma, RandomGenerator& rng)
{
	switch(img->depth)
	{
		case IPL_DEPTH_8U:
			_addGaussianNoise<unsigned char, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_8S:
			_addGaussianNoise<char, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_16U:
			_addGaussianNoise<unsigned short, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_16S:
			_addGaussianNoise<short, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_32S:
			_addGaussianNoise<int, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_32F:
			_addGaussianNoise<float, RandomGenerator>(img, sigma, rng);
			break;
		case IPL_DEPTH_64F:
			_addGaussianNoise<double, RandomGenerator>(img, sigma, rng);
			break;
		default:
			assert(0);
	}
}

//template <typename ValueType>
//double _getSubPixel(const IplImageWrapper& img, const Point& point)
//{
//	// method is only working for one channel images and double precision floating point
//	assert(srcImg->nChannels == 1);
//	assert(dstImg->depth == IPL_DEPTH_64F || dstImg->depth == IPL_DEPTH_32F);
//
//	// check that point lies within the image
//	if (point.getX() < 0 || point.getX() > img->width - 1 || point.getY() < 0 || point.getY() > img->height - 1)
//		return 0;
//
//	int left = static_cast<int>(floor(point.getX()));
//	int right = static_cast<int>(ceil(point.getX()));
//	int top = static_cast<int>(floor(point.getY()));
//	int bottom = static_cast<int>(ceil(point.getY()));
//
//	// interpolate in x direction
//	ValueType valTop;
//	ValueType valBottom;
//	if (left == right) {
//		valTop = CV_IMAGE_ELEM(img, ValueType, top, right);
//		valBottom = CV_IMAGE_ELEM(img, ValueType, bottom, right);
//	}
//	else {
//		ValueType factor1 = point.getX() - left;
//		ValueType factor2 = right - point.getX();
//		valTop = factor1 * CV_IMAGE_ELEM(img, ValueType, top, left) + factor2 * CV_IMAGE_ELEM(img, ValueType, top, right);
//		valBottom = factor1 * CV_IMAGE_ELEM(img, ValueType, bottom, left) + factor2 * CV_IMAGE_ELEM(img, ValueType, bottom, right);;
//	}
//
//	// interpolate in y direction
//	ValueType val;
//	if (top == bottom) {
//		val = valBottom;
//	}
//	else {
//		ValueType factor1 = point.getY() - top;
//		ValueType factor2 = bottom - point.getY();
//		val = factor1 * valTop + factor2 * valBottom;
//	}
//
//	return val;
//}

inline
double getIntegralRegion(const IplImageWrapper& integralImg, const Box<int>& box)
{
	int top = std::min(box.getTop() - 1, integralImg->height - 1);
	int bottom = std::min(box.getBottom() - 1, integralImg->height - 1);
	int left = std::min(box.getLeft() - 1, integralImg->width - 1);
	int right = std::min(box.getRight() - 1, integralImg->width - 1);
	double sumTopLeft(0), sumTopRight(0), sumBottomLeft(0), sumBottomRight(0);
	if (top >= 0) {
		if (left >= 0)
			sumTopLeft = CV_IMAGE_ELEM(integralImg, double, top, left);
		if (right >= 0)
			sumTopRight = CV_IMAGE_ELEM(integralImg, double, top, right);
	}
	if (bottom >= 0) {
		if (left >= 0)
			sumBottomLeft = CV_IMAGE_ELEM(integralImg, double, bottom, left);
		if (right >= 0)
			sumBottomRight = CV_IMAGE_ELEM(integralImg, double, bottom, right);
	}
	return sumBottomRight + sumTopLeft - sumBottomLeft - sumTopRight;
//	double sumTopLeft(0), sumTopRight(0), sumBottomLeft(0), sumBottomRight(0);
//	if (box.getTop() >= 0) {
//		if (box.getLeft() >= 0)
//			sumTopLeft = getSubPixel(integralImg, box.getTopLeft());
//		if (box.getRight() >= 0)
//			sumTopRight = getSubPixel(integralImg, box.getTopRight());
//	}
//	if (box.getBottom() >= 0) {
//		if (box.getLeft() >= 0)
//			sumBottomLeft = getSubPixel(integralImg, box.getBottomLeft());
//		if (box.getRight() >= 0)
//			sumBottomRight = getSubPixel(integralImg, box.getTopRight());
//	}
//	return sumBottomRight + sumTopLeft - sumBottomLeft - sumTopRight;
}



