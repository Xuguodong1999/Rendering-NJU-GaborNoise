#ifndef GABORKERNEL_H
#define GABORKERNEL_H 

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class GaborKernel {
public:
    GaborKernel(float _K = 1.0, float _a = 0.125, float _F0 = 0.25, float _w0 = 0)
            : K(_K), a(_a), F0(_F0), w0(_w0) {}

    // 2-3节，公式(6)
    // Gabor核在空间域的取值
    // 单独显示时，输入原分辨率下的坐标，按1/16进行插值
    float atSpatial(float _x, float _y) const {
        const float envelop = K * exp(-M_PI * a * a * (_x * _x + _y * _y)),
                harmonic = cos(2.0 * M_PI * F0 * (_x * cos(w0) + _y * sin(w0)));
        return envelop * harmonic;
    }

    // 2-3节，公式(7)
    // Gabor核在频域的取值，输入归一化的坐标
    float atFrequency(float _fx, float _fy) const {
        const float fc0 = F0 * cos(w0),
                fs0 = F0 * sin(w0);
        return K *
               (exp(-M_PI / a / a * (powf(_fx - fc0, 2) + powf(_fy - fs0, 2))) +
                exp(-M_PI / a / a * (powf(_fx + fc0, 2) + powf(_fy + fs0, 2)))
               ) / 2.0 / a / a;
    }

    void setGaussianMagnitude(float _K) { K = _K; }

    void setGaussianWidth(float _a) { a = _a; }

    void setHarmonicMagnitude(float _F0) { F0 = _F0; }

    void setHarmonicOrientation(float _w0) { w0 = _w0; }

    void setParameter(float _K = 1.0, float _a = 0.125, float _F0 = 0.25, float _w0 = 0) {
        setGaussianMagnitude(_K);
        setGaussianWidth(_a);
        setHarmonicOrientation(_w0);
        setHarmonicMagnitude(_F0);
    }

private:
    float K, a, F0, w0;
};

#endif // GABORKERNEL_H
