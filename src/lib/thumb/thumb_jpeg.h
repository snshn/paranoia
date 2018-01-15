#pragma once

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <math.h>

#define JPEG_QUALITY 90

struct my_error_mgr
{
    struct jpeg_error_mgr pub;

    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

typedef struct jpg_ {
    FILE * fp;
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
} jpg_t;

int read_jpg(const char *file, img_t *img);
int open_jpg_writer(const char *file, jpg_t *jpg, int channels, int sizeX, int sizeY);
void close_jpg_writer(jpg_t *jpg);
