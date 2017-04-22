#ifndef NOISE_H
#define NOISE_H
#include "icg_common.h"
#include "perlin/perlin.h"
#include <vector>

class Noise {
public:
    Noise(double h, double l, double o):
        H(h), lacunarity(l), octaves(o){}

    /**
     * @brief fBm: Evaluated noise at point
     * @param point: position vector
     * @return fBm noise at point
     */
    double fBm(vec3 point){
        double value = 0.0;
        double remainder;

        /// @todo Inner loop of fractal construction
        for (int i = 0; i < octaves; i++) {
            value += perlin.noise(point) * pow(lacunarity, -H*i);
            point *= lacunarity;
        }

        /// @todo Handle octaves remainder
        remainder = octaves - (int)octaves;
        if (remainder) { /** add in 'octaves' remainder */
            value += remainder * perlin.noise(point) * pow(lacunarity, -H*(int)octaves);
        }
        return value;
    }

    /**
     * @brief HybridMultifractal: additive/multiplicative multifractal terrain model
     * > good parameter values:
     * > H     : 0.25
     * > offset: 0.7
     * @param point: position vector
     * @param offset
     * @return noise at point
     */
    double HybridMultifractal(vec3 point, double offset){
        static int first = 1;
        static double* exparray;
        /// @todo precompute and store spectral weights
        if (first) {
            /// @todo seize required memory for exponent_array
            exparray = new double[(int)octaves];
            double frequency = 1.0;
            for (int i = 0; i < octaves; ++i) {
                /// @todo compute weight for each frequency
                exparray[i] = pow(frequency, -H);
                frequency *= lacunarity;
            }
            first = 0;
        }

        /// @todo get first octave of function
        double result = (perlin.noise(point) + offset) * exparray[0];
        double weight = result;

        /// @todo increase frequency
        point *= lacunarity;

        /// @todo spectral construction inner loop, where the fractal is built
        for (int i = 1; i < octaves; ++i) {
            /// @todo prevent divergence
            if (weight > 1.0){ weight = 1.0; }

            /// @todo get next higher frequency
            double signal = (perlin.noise(point) + offset) * exparray[i];

            /// @todo add it in, weighted by previous freq's local value
            result += weight * signal;

            /// @todo update the (monotonically decreasing) weighting value
            weight *= signal;

            /// @todo increase frequency
            point *= lacunarity;
        }

        /// @todo take care of remainder in 'octaves'
        double remainder = octaves - (int)octaves;
        if (remainder) {
            /// @todo 'i' and spatial freq. are preset in loop above
            result += remainder * perlin.noise(point) * exparray[(int)octaves];
        }

        return result;
    }

    /**
     * @brief RidgedMultifractal
     * > good parameter values:
     * > H     : 1.0
     * > offset: 1.0
     * > gain  : 2.0
     * @param point: position vector
     * @param offset
     * @param gain
     * @return noise at point
     */
    double RidgedMultifractal(vec3 point, double offset, double gain) {
          static int first = 1;
          static double* exponent_array;

          /// @todo precompute and store spectral weights
          if (first) {
                /// @todo seize required memory for exponent_array
                exponent_array = new double[(int)octaves];
                double frequency = 1.0;
                for (int i = 0; i < octaves; i++) {
                      /// @todo compute weight for each frequency
                      exponent_array[i] = pow(frequency, -H);
                      frequency *= lacunarity;
                }
                first = 0;
          }

          /// @todo get absolute value of the first octave
          double signal = abs(perlin.noise(point));

          /// @todo invert and translate (note that "offset" should be ~= 1.0)
          signal = offset - signal;

          /// @todo square the signal, to increase "sharpness" of ridges
          signal *= signal;

          /// @todo assign initial values
          double result = signal;
          double weight = 1.0;

          for(int i = 1; i < octaves; ++i) {
                /// @todo increase the frequency
                point *= lacunarity;

                /// @todo weight successive contributions by previous signal
                weight = signal * gain;
                if ( weight > 1.0 ) {
                    weight = 1.0;
                } else if ( weight < 0.0 ) {
                    weight = 0.0;
                }

                signal = abs(perlin.noise(point));
                signal = offset - signal;
                signal *= signal;

                /// @todo weight the contribution
                signal *= weight;
                result += signal * exponent_array[i];
          }
          return result;

    }

    float operator()(float x, float y, float z, double offset = 0.0, double gain = 0.0){
        if (offset == 0.0 && gain == 0.0){
            return fBm(vec3(x, y, z));
        } else if (gain == 0.0) {
            return HybridMultifractal(vec3(x, y, z), offset);
        } else {
            return RidgedMultifractal(vec3(x, y, z), offset, gain);
        }
    }

private:
    PerlinNoise perlin;
    double H;  ///< Fractal increment parameter
    double lacunarity;  ///< the gap between successive frequencies
    double octaves;  ///< The number of frequencies in the fBm
};

#endif
