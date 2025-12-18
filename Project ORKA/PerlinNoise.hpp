#pragma once

#include "Basics.hpp"

Double noise1D(Double x);
Double noise2D(Double x, Double y);
Double noise3D(Double x, Double y, Double z);
Double noise4D(Double x, Double y, Double z, Double w);

Double noise01(Double x);
Double noise01(Double x, Double y);
Double noise01(Double x, Double y, Double z);
Double noise01(Double x, Double y, Double z, Double w);

Double octaveNoise(Double x, Int octaves);
Double octaveNoise(Double x, Double y, Int octaves);
Double octaveNoise(Double x, Double y, Double z, Int octaves);
Double octaveNoise(Double x, Double y, Double z, Double w, Int octaves);

Double octaveNoise01(Double x, Int octaves);
Double octaveNoise01(Double x, Double y, Int octaves);
Double octaveNoise01(Double x, Double y, Double z, Int octaves);
Double octaveNoise01(Double x, Double y, Double z, Double w, Int octaves);
