#ifndef GABORUTIL_H
#define GABORUTIL_H 

#include <limits>
#include <climits>
#include <cmath>
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;
using std::numeric_limits;

// 算莫顿码，用作随机种子
inline unsigned morton(unsigned x, unsigned y) {
    unsigned m = 0;
    const unsigned one = 1, len = sizeof(unsigned) * CHAR_BIT;
    for (unsigned i = 0; i < len; i++) {
        m |= ((x & (one << i)) << i) | ((y & (one << i)) << (i + 1));
    }
    return m;
}

// 获取小数位
inline float frac(float _x) { return _x - floor(_x); }

/* Procedural Noise using Sparse Gabor Convolution
 * 附录的伪随机数生成器
 */
class PseudoRandomNumberGenerator {
public:
    PseudoRandomNumberGenerator(unsigned _s = 1) : x(_s) {}

    /*
     * 获取以lambda为均值的泊松分布的随机数
     * 定理:若lambda>0,x是整数,ui~(0,1),且
     * e^-lambda在x到x+1个随机变量ui的连乘之间,
     * 那么x是一个以lambda为均值的泊松分布的随机变量
     */
    unsigned poisson(float lambda) {
        float middle = exp(-lambda);
        double mului = rand01();
        unsigned res = 0;
        while (mului > middle) {
            mului *= rand01();
            ++res;
        }
        return res;
    }

    /*
     * 重置随机数种子
     */
    void reSeed(unsigned s) { x = s; }

    /*
     * 获得0-1之间的随机数
     */
    float rand01() {
        x *= magicNumber;
        return float(x) / umax;
    }

    /*
     * 获得一个范围里的随机数
     */
    float randin(float min, float max) {
        return (max - min) * rand01() + min;
    }

private:
    unsigned x;
    const float umax = numeric_limits<unsigned>::max();
    const unsigned magicNumber = 3039177861;
};

#endif // GABORUTIL_H
