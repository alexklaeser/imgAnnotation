#ifndef numeric_functions_HPP
#define numeric_functions_HPP

#include <cmath>

	
inline
bool fuzzyEqual(const double& x1, const double& x2)
{
    return fabs(x1 - x2) <= 1e-10 * fabs(x1 + x2);
}

inline
bool fuzzyLower(const double& x1, const double& x2)
{
    return x1 < x2 - 1e-10 * fabs(x1 + x2);
}

inline
bool fuzzyLowerEqual(const double& x1, const double& x2)
{
    return x1 <= x2 + 1e-10 * fabs(x1 + x2);
}

inline
bool fuzzyGreater(const double& x1, const double& x2)
{
    return x1 > x2 + 1e-10 * fabs(x1 + x2);
}

inline
bool fuzzyGreaterEqual(const double& x1, const double& x2)
{
    return x1 >= x2 - 1e-10 * fabs(x1 + x2);
}

inline
bool fuzzyEqual(const float& x1, const float& x2)
{
    return fabs(x1 - x2) <= 1e-5 * fabs(x1 + x2);
}

inline
bool fuzzyLower(const float& x1, const float& x2)
{
    return x1 < x2 - 1e-5 * fabs(x1 + x2);
}

inline
bool fuzzyLowerEqual(const float& x1, const float& x2)
{
    return x1 < x2 + 1e-5 * fabs(x1 + x2);
}

inline
bool fuzzyGreater(const float& x1, const float& x2)
{
    return x1 > x2 + 1e-5 * fabs(x1 + x2);
}

inline
bool fuzzyGreaterEqual(const float& x1, const float& x2)
{
    return x1 > x2 - 1e-5 * fabs(x1 + x2);
}

inline
double interpolateXMaximum(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double x1Sqr = pow(x1, 2);
	double x2Sqr = pow(x2, 2);
	double x3Sqr = pow(x3, 2);
	return 0.5 * (y2 * (x3Sqr - x1Sqr) + y3 * (x1Sqr - x2Sqr) + y1 * (x2Sqr - x3Sqr)) /
			(y2 * (x3 - x1) + y3 * (x1 - x2) + y1 * (x2 - x3));
}

#endif

