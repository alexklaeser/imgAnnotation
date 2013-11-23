#ifndef numeric_functions_H
#define numeric_functions_H



bool fuzzyEqual(const double& x1, const double& x2);

bool fuzzyLower(const double& x1, const double& x2);

bool fuzzyLowerEqual(const double& x1, const double& x2);

bool fuzzyGreater(const double& x1, const double& x2);

bool fuzzyGreaterEqual(const double& x1, const double& x2);

bool fuzzyEqual(const float& x1, const float& x2);

bool fuzzyLower(const float& x1, const float& x2);

bool fuzzyLowerEqual(const float& x1, const float& x2);

bool fuzzyGreater(const float& x1, const float& x2);

bool fuzzyGreaterEqual(const float& x1, const float& x2);

double interpolateXMaximum(double x1, double y1, double x2, double y2, double x3, double y3);


#endif

