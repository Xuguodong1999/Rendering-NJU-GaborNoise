#include "display.h"
#include <CImg.h>
#include <vector>
#include <string>
#include <limits>
#include <iostream>
using namespace std;
using namespace cimg_library;
const float f_inf = std::numeric_limits<float>::infinity();

void display(
	const int w, const int h,
	const vector<Point>& points,
	const string& displayName,
	bool save
) {
	CImg<unsigned char> img(w, h, 1, 1);
	img.fill(0);
	for (auto& p : points) {
		img(p.x, p.y, 0) = 255;
	}
	if (save) {
		img.save((displayName + ".bmp").c_str());
		cout << "save to " << displayName + ".bmp" << endl;
	}
	else {
		img.display(displayName.c_str());
	}
}

void display(
	const vector<vector<float >>& powerMap,
	const string& displayName,
	bool save
) {
	CImg<unsigned char> img(powerMap.size(), powerMap[0].size(), 1, 1);
	img.fill(0);
	cimg_forXY(img, i, j) {
		img(i, j, 0) = powerMap[i][j];
	}
	if (save) {
		img.save((displayName + ".bmp").c_str());
		cout << "save to " << displayName + ".bmp" << endl;
	}
	else {
		img.display(displayName.c_str());
	}
}