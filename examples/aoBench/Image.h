//
// Created by denn nevera on 06/11/2020.
//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

struct Image {

    Image(size_t width, size_t height)
            : width(width), height(height),
              pix(new float[3*width*height])
    {};

    ~Image() { delete[] pix; }

    size_t width, height;
    float *pix;

    void savePPM(const char *fName) const;
};
