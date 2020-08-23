#ifndef GABORNOISE_H
#define GABORNOISE_H 

#include "gaborkernel.h"
#include "gaborutil.h"

using std::exp;
using std::sqrt;

class GaborNoise {
public:
    GaborNoise(float _K = 1.0, float _a = 0.125, float _F0 = 0.25, float _w0 = 0, bool _isIsotropic = false,
               float _kernel_radius = 5, float _number_of_impulses_per_cell = 100) {
        setParameter(_K, _a, _F0, _w0, _isIsotropic, _kernel_radius, _number_of_impulses_per_cell);
        setRandomOffset(rand() % 1000);
    }

    /*
     * Procedural Noise using Sparse Gabor Convolution
     * 附录的noise函数，
     */
    float noise(float _x, float _y) {
        _x /= kernel_radius;
        _y /= kernel_radius;
        float res = 0;
        // 确定(_x,_y)所属的网格以及其周围的八个网格
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                res += cell(floor(_x) + i, floor(_y) + j, frac(_x) - i, frac(_y) - j);
            }
        }
        return res;
    }

    float frequency(float _x, float _y) {
        if (isIsotropic) {
            /* 论文3.1节公式14
             */
            PseudoRandomNumberGenerator prng(rand());
            auto wi = prng.rand01();
            float delta = sqrt(_x * _x + _y * _y) - F0;
            return wi * wi * K * K / (4 * sqrt(2) * M_PI * F0 * a * a * a) *
                   exp(-2 * M_PI / a / a * delta * delta);
        } else {
            /* 论文2.4节公式10
             * The power spectrum of band-limited anisotropic noise
             * is the power spectrum of the Gabor kernel
             * scaled by the constant λE(w^2)
             */
            PseudoRandomNumberGenerator prng(rand());
            auto wi = prng.rand01();
            return wi * wi * kernel.atFrequency(_x, _y);
        }
    }

    void setParameter(float _K = 1.0, float _a = 0.125, float _F0 = 0.25, float _w0 = 0, bool _isIsotropic = false,
                      float _kernel_radius = 5, float _number_of_impulses_per_cell = 100) {
        K = _K;
        a = _a;
        F0 = _F0;
        w0 = _w0;
        setIsIsotropic(_isIsotropic);
        setKernelRadius(_kernel_radius);
        setNumberOfImpulsesPerCell(_number_of_impulses_per_cell);
        kernel.setParameter(K, a, F0, w0);
    }

    void setNumberOfImpulsesPerCell(float numberOfImpulsesPerCell) {
        number_of_impulses_per_cell = numberOfImpulsesPerCell;
    }

    void setKernelRadius(float kernelRadius) {
        kernel_radius = kernelRadius;
    }

    void setRandomOffset(unsigned int randomOffset) {
        random_offset = randomOffset;
    }

private:
    GaborKernel kernel;
    float K, a, F0, w0;
    bool isIsotropic;
public:
    void setIsIsotropic(bool _isIsotropic) {
        isIsotropic = _isIsotropic;
    }

private:
    float number_of_impulses_per_cell, kernel_radius;
    unsigned random_offset;

    /*
     * Procedural Noise using Sparse Gabor Convolution
     * 附录的cell函数，
     */
    float cell(int i, int j, float x, float y) {
        // 用莫顿码确保每次生成时网格和随机数生成器的一一对应
        // 用random_offset实现多次生成时结果的不确定性
        auto s = morton(i, j) + random_offset;
        if (s == 0)s = 1;
        // 并行化以后不能把随机数生成器定义为类成员
        PseudoRandomNumberGenerator prng(s);
        auto number_of_impulses = prng.poisson(number_of_impulses_per_cell);
        float res = 0;
        if (isIsotropic) {
            for (unsigned i = 0; i < number_of_impulses; i++) {
                auto xi = prng.rand01(), yi = prng.rand01();
                // wi代表白噪音，随机变量wi均值为0,见2.2节公式3
                auto wi = prng.randin(-1, 1);
                // 各向同性噪声：每一次都重置Gabor核的方向
                kernel.setHarmonicOrientation(prng.randin(0.0, 2.0 * M_PI));
                res += wi * kernel.atSpatial((x - xi) * kernel_radius,
                                             (y - yi) * kernel_radius);
            }
        } else {
            for (unsigned i = 0; i < number_of_impulses; i++) {
                auto xi = prng.rand01(), yi = prng.rand01();
                auto wi = prng.randin(-1, 1);
                res += wi * kernel.atSpatial((x - xi) * kernel_radius,
                                             (y - yi) * kernel_radius);
            }
        }
        return res;
    }
};

#endif // GABORNOISE_H
