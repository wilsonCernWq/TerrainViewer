#ifndef PERLINNOISE_H
#define PERLINNOISE_H
#include "icg_common.h"
#include <vector>

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

#endif
