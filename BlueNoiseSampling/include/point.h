#ifndef BNS_POINT_H
#define BNS_POINT_H

#include <limits>

extern const float f_inf;

class Point {
public:
	Point(const float _x = f_inf, const float _y = f_inf) : x(_x), y(_y) {}

	float x, y;

	bool isUsed() const { return x != f_inf; }
};

#endif //BNS_POINT_H
