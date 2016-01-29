#ifndef _PERLINNOISE_H_
#define _PERLINNOISE_H_

#include <iostream>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include "icg_common.h"

class PerlinNoise {
    //--- The permutation vector
    std::vector<int> p;
public:
    // Initialize with the reference values for the permutation vector
    PerlinNoise();
    // Get a noise value, for 2D images z can have any value
    double noise(double x, double y, double z);
    double noise(vec3 p);

private:
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};

#endif // _PERLINNOISE_H_
