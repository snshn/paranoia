#pragma once

#include <png.h>
#include <jpeglib.h>

#define PNG 0
#define JPG 1
#define GIF 2

#define TILE_SIZE 128

typedef struct img_ {
    unsigned int sizeX, sizeY;
    unsigned char *raw_data;
    unsigned int channels;
    unsigned int tSize;
} img_t;

//int isPowOfTwo(int v);
//int read_ppm(const char * file, img_t *img);

void scale_image(img_t *img);
void save_image(img_t *img, int output_format, char *filename);

int thumb_create(char *input_file_path, unsigned int size, char *out_file_name);
