#ifndef BNS_DISPLAY_H
#define BNS_DISPLAY_H

#include "point.h"
#include <string>
#include <vector>

void display(
	const int w, const int h,
	const std::vector<Point>& points,
	const std::string& displayName = "image",
	bool save = false
);

void display(
	const std::vector<std::vector<float >>& powerMap,
	const std::string& displayName = "image",
	bool save = false
);

#endif //BNS_DISPLAY_H
