#include "poissondisk.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
using namespace std;
#ifndef M_PI
#define M_PI 3.14159265358979323846 
#endif // !M_PI
// Fast PoissonDisk Sampling算法，输入间距r、采样区域长宽，输出采样点
void PoissonDisk::fastPoissonDiskSampling(
	std::vector<Point>& result, const float _r, int _width, int _height, int _k
) {
	set(_r, _width, _height, _k);
	backgroundGrid.resize(gridWidth);
	for (auto& i : backgroundGrid) { i.resize(gridHeight, Point()); }
	vector<Point> activeList;
	Point start(rand() % width, rand() % height);
	activeList.push_back(start);
	result = { start };
	setGrid(start);
	while (!activeList.empty()) {
		size_t i = rand() % activeList.size();
		const Point center = activeList[i];
		activeList.erase(activeList.begin() + i);
		for (int ii = 0; ii < k; ii++) {
			Point&& neighbor = getNeighbor(center);
			if (isInArea(neighbor))
				if (isValidNeighbor(neighbor)) {
					activeList.push_back(neighbor);
					result.emplace_back(neighbor);
					setGrid(neighbor);
				}
		}
	}
	backgroundGrid.clear();
}
// 另一种输入方式：输入采样点个数，估算采样图长宽，输出采样点
void PoissonDisk::fastPoissonDiskSampling(std::vector<Point>& result, const int num) {
	fastPoissonDiskSampling(result, 8, sqrt(num * 107), sqrt(num * 107));
}

void getFourierPowerSpectrum(std::vector<std::vector<float>>& result,
	const int size, const int w, const int h,
	const vector<Point>& samplePoints
) {
	result.resize(2 * size); // [2*size, 2*size]
	for (auto& i : result) { i.resize(2 * size, 0); }
	const float N = samplePoints.size();
	vector<Point> tmpSample = samplePoints;
	for (auto& p : tmpSample) {
		p.x *= (2.0 * M_PI / (float)w);
		p.y *= (2.0 * M_PI / (float)h);
	}
	#pragma omp parallel for
	for (int i = -size; i < size; i++) {
		for (int j = -size; j < size; j++) {
			float s = 0, c = 0, tmp;
			for (const auto& p : tmpSample) {
				tmp = i * p.x + j * p.y;
				c += cos(tmp);
				s += sin(tmp);
			}
			result[i + size][j + size] = 255.0 / N * (c * c + s * s);
		}
	}
}

float getDiscrepancy(const int w, const int h,
	const vector<Point>& samplePoints
) {
	const float N = samplePoints.size(), S = w * h;
	float n, s, result = numeric_limits<float>::min();
	for (int x = 1; x < w; x++) {
		for (int y = 1; y < h; y++) {
			n = 0;
			s = x * y;
			for (auto& j : samplePoints) {
				n += (j.x <= x && j.y <= y);
			}
			result = max(result, fabs(n / N - s / S));
		}
	}
	return result;
}

inline float squareDistance(const Point& a, const Point& b) {
	const float dx = a.x - b.x, dy = a.y - b.y;
	return dx * dx + dy * dy;
}

inline Point PoissonDisk::getNeighbor(const Point& p) const {
	float radius = r * sqrt(3 * (float)rand() / (float)(RAND_MAX)+1); // 1-2 r
	float angle = 2 * M_PI * (float)rand() / (float)(RAND_MAX);// 0-360
	Point res = p;
	res.x += cos(angle) * radius;
	res.y += sin(angle) * radius;
	return res;
}
bool PoissonDisk::isValidNeighbor(const Point& p) const {
	const int xIndex = floor(p.x / cellSize),
		yIndex = floor(p.y / cellSize);
	if (backgroundGrid[xIndex][yIndex].isUsed())
		return false;
	const float rr = r * r;
	const int minX = max(xIndex - 2, 0),
		minY = max(yIndex - 2, 0),
		maxX = min(xIndex + 2, gridWidth - 1),
		maxY = min(yIndex + 2, gridHeight - 1);
	for (int i = minX; i <= maxX; i++) {
		for (int j = minY; j <= maxY; j++) {
			const Point& neighbor = backgroundGrid[i][j];
			if (neighbor.isUsed() && squareDistance(p, neighbor) < rr)
				return false;
		}
	}
	return true;
}

bool PoissonDisk::isInArea(const Point& p) const {
	return p.x > 0 && p.x < width&& p.y > 0 && p.y < height;
}

void PoissonDisk::setGrid(const Point& point) {
	backgroundGrid[floor(point.x / cellSize)][floor(point.y / cellSize)] = point;
}

void PoissonDisk::set(const float _r, int _width, int _height, int _k) {
	r = _r;
	k = _k;
	height = _height;
	width = _width;
	cellSize = r / sqrt(2);
	gridWidth = floor(1 + width / cellSize);
	gridHeight = floor(1 + height / cellSize);
}
