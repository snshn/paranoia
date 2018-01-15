#pragma once

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <png.h>
#include <math.h>

typedef struct png_ {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
} png_t;

int read_png(const char *file, img_t *img);
int open_png_writer(const char *file, png_t *png, int channels, int sizeX, int sizeY);
void close_png_writer(png_t *png);
