#ifndef BNS_POISSONDISK_H
#define BNS_POISSONDISK_H
#include "point.h"
#include <vector>

class PoissonDisk {
	int width, height, cellSize, k, gridWidth, gridHeight;
	float r;
	std::vector<std::vector<Point>> backgroundGrid;

	inline Point getNeighbor(const Point& p) const;

	bool isValidNeighbor(const Point& p) const;

	inline bool isInArea(const Point& p) const;

	void setGrid(const Point& point);

	inline void set(const float _r, int _width, int _height, int _k);

public:
	void fastPoissonDiskSampling(std::vector<Point>& result, const float _r = 4, int _width = 320, int _height = 320, int _k = 20);
	void fastPoissonDiskSampling(std::vector<Point>& result, const int num = 100);
	int getWidth()const { return width; }
	int getHeight()const { return height; }
	float getR()const { return r; }
};

void getFourierPowerSpectrum(
	std::vector<std::vector<float>>& result,
	const int size, const int w, const int h,
	const std::vector<Point>& samplePoints
);
float getDiscrepancy(const int w, const int h,
	const std::vector<Point>& samplePoints
);
inline float squareDistance(const Point& a, const Point& b);

#endif //BNS_POISSONDISK_H
