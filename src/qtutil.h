#ifndef QTUTIL_H
#define QTUTIL_H 

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <map>
#include <vector>
#include <algorithm>

using std::pair;
using std::vector;
using std::make_pair;

// 灰度图结果可视化，线性归一化到0-255
inline const QPixmap vec2gray(const vector<vector<float>> &vec) {
    float mmax = numeric_limits<float>::min(),
            mmin = numeric_limits<float>::max();
    for (auto &i:vec) {
        for (auto &j:i) {
            if (mmax < j)mmax = j;
            if (mmin > j)mmin = j;
        }
    }
    float delta = mmax - mmin;
    const size_t width = vec.size(), height = vec.at(0).size();
    QImage img(width, height, QImage::Format_RGB32);
#pragma omp parallel for
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            const float val = (vec[i][j] - mmin) / delta * 255.0;
            img.setPixel(i, j, QColor(val, val, val).rgb());
        }
    }
    return QPixmap::fromImage(img);
}
// 颜色查找表
// 使用在键(浮点数)值(RGB)对数组中插值取色的策略
using ColorMapData=vector<pair<float, QColor>>;

class ColorMap {
public:
    ColorMap() {}

    // 添加一组键值对节点
    void add(const float key, const QColor &color) {
        m_colorMapData.emplace_back(make_pair(key, color));
    }

    // 添加完以后必须排序
    void sort() {
        std::sort(m_colorMapData.begin(), m_colorMapData.end(),
                  [=](const pair<float, QColor> &a, const pair<float, QColor> &b) {
                      return a.first < b.first;
                  });
    }

    QColor get(const float key) const {
        if (key < firstKey()) {
            return interp(0, firstKey(), key, Qt::black, getColor(0));
        } else if (key > lastKey()) {
            return interp(lastKey(), 1, key, getColor(m_colorMapData.size() - 1), Qt::black);
        }
        for (size_t i = 0; i < m_colorMapData.size() - 1; i++) {
            const auto &c1 = m_colorMapData[i];
            const auto &c2 = m_colorMapData[i + 1];
            if (c1.first <= key && key <= c2.first)
                return interp(c1.first, c2.first, key, c1.second, c2.second);
        }
        return Qt::black;
    }

private:
    ColorMapData m_colorMapData;

    QColor getColor(size_t i) const {
        return m_colorMapData[i].second;
    }

    float lastKey() const {
        if (m_colorMapData.size() <= 0)return -1;
        else {
            return m_colorMapData[m_colorMapData.size() - 1].first;
        }
    }

    float firstKey() const {
        if (m_colorMapData.size() <= 0)return -1;
        else {
            return m_colorMapData[0].first;
        }
    }

    float interp(float x, float x0, float x1, float y0, float y1) const {
        auto dx = x1 - x0;
        if (abs(dx) < 1e-6)
            return (y0 + y1) / 2.0;
        return y0 + (x - x0) / dx * (y1 - y0);
    }

    QColor interp(float key1, float key2, float key, const QColor &c1, const QColor &c2) const {
        auto delta = (key2 - key1);
        auto r = interp(key, key1, key2, c1.red(), c2.red());
        auto g = interp(key, key1, key2, c1.green(), c2.green());
        auto b = interp(key, key1, key2, c1.blue(), c2.blue());
        return QColor(r, g, b);
    }
};

inline QColor operator/(const QColor &a, const float &b) {
    return QColor(a.red() / b, a.green() / b, a.blue() / b);
}

// 纹理结果可视化
inline const QPixmap vec2rgb(const vector<vector<float>> &vec, const ColorMap &cm) {
    float mmax = numeric_limits<float>::min(),
            mmin = numeric_limits<float>::max();
    for (auto &i:vec) {
        for (auto &j:i) {
            if (mmax < j)mmax = j;
            if (mmin > j)mmin = j;
        }
    }
    float delta = mmax - mmin;
    const size_t width = vec.size(), height = vec.at(0).size();
    QImage img(width, height, QImage::Format_RGB32);
#pragma omp parallel for
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            const float val = (vec[i][j] - mmin) / delta * 255;
            img.setPixel(i, j, cm.get(val).rgb());
        }
    }
    return QPixmap::fromImage(img);
}

#endif //QTUTIL_H
