#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "thumb.h"
#include "thumb_png.h"
#include "thumb_jpeg.h"


int isPowOfTwo(int v)
{
  return !(v & (v-1));
}


/*
static int read_int(FILE *fp)
{
    int k;
    char ch;

    do {
        while (isspace(ch = getc(fp)));
        if (ch == '#')
            while ((ch = getc(fp)) != '\n');
    } while (!isdigit(ch));

    for (k = (ch - '0'); isdigit(ch = getc(fp)); k = 10 * k + (ch - '0'));

    return k;
}


int read_ppm(const char *file, img_t *img)
{
    FILE *fp;
    char magic[3];
    int w, h, max;

    fp = fopen(file, "rb");
    if (!fp) return 2;

    fread(magic, 2, 1, fp);

    if (strncmp(magic, "P6", 2) != 0) return 3;

    w = read_int(fp);
    h = read_int(fp);
    max = read_int(fp);

    if (max > 255) return 6;
    if (!isPowOfTwo(h) || !isPowOfTwo(w)) return 7;

    img->sizeX = w;
    img->sizeY = h;
    img->raw_data = malloc(w*h*sizeof(rgba_t));

    // TODO: improve and use mmap
    fread(img->raw_data, sizeof(rgba_t), w * h, fp);

    return 0;
}
*/


void scale_image(img_t * img)
{
    int w, h;
    const int width = img->sizeX;
    const int height = img->sizeY;

    /*
     * This code code be nicely optimized with sse.
     * However, according to callgrind this function
     * has < 1% cpu time
     */

    switch (img->channels)
    {
        case 4:
            for (h = 0; h < height / 2; h++)
            {
                for (w = 0; w < width / 2; w++)
                {
                    unsigned char * v0 = &img->raw_data[(w*2 + h*2 * width)*4]; // x
                    unsigned char * v1 = &img->raw_data[(w*2 + 1 + h*2 * width)*4]; // >
                    unsigned char * v2 = &img->raw_data[(w*2 + (h*2 + 1) * width)*4]; // v
                    unsigned char * v3 = &img->raw_data[(w*2 + 1 + (h*2 + 1) * width)*4]; // >v

                    int x = (w + h * (width / 2)) * 4;

                    img->raw_data[x] = (*v0 + *v1 + *v2 + *v3) / 4;
                    img->raw_data[x+1] = (*(v0+1) + *(v1+1) + *(v2+1) + *(v3+1)) / 4;
                    img->raw_data[x+2] = (*(v0+2) + *(v1+2) + *(v2+2) + *(v3+2)) / 4;
                    img->raw_data[x+3] = (*(v0+3) + *(v1+3) + *(v2+3) + *(v3+3)) / 4;
                }
            }
        break;

        case 3:
            for (h = 0; h < height / 2; h++)
            {
                for (w = 0; w < width / 2; w++)
                {
                    unsigned char * v0 = &img->raw_data[(w*2 + h*2 * width)*3]; // x
                    unsigned char * v1 = &img->raw_data[(w*2 + 1 + h*2 * width)*3]; // >
                    unsigned char * v2 = &img->raw_data[(w*2 + (h*2 + 1) * width)*3]; // v
                    unsigned char * v3 = &img->raw_data[(w*2 + 1 + (h*2 + 1) * width)*3]; // >v

                    int x = (w + h * (width / 2)) * 3;

                    img->raw_data[x] = (*v0 + *v1 + *v2 + *v3) / 4;
                    img->raw_data[x+1] = (*(v0+1) + *(v1+1) + *(v2+1) + *(v3+1)) / 4;
                    img->raw_data[x+2] = (*(v0+2) + *(v1+2) + *(v2+2) + *(v3+2)) / 4;
                }
            }
        break;

        case 2:
            for (h = 0; h < height / 2; h++)
            {
                for (w = 0; w < width / 2; w++)
                {
                    unsigned char * v0 = &img->raw_data[(w*2 + h*2 * width)*2]; // x
                    unsigned char * v1 = &img->raw_data[(w*2 + 1 + h*2 * width)*2]; // >
                    unsigned char * v2 = &img->raw_data[(w*2 + (h*2 + 1) * width)*2]; // v
                    unsigned char * v3 = &img->raw_data[(w*2 + 1 + (h*2 + 1) * width)*2]; // >v

                    int x = (w + h * (width / 2)) * 2;

                    img->raw_data[x] = (*v0 + *v1 + *v2 + *v3) / 2;
                    img->raw_data[x+1] = (*(v0+1) + *(v1+1) + *(v2+1) + *(v3+1)) / 4;
                }
            }
        break;

        case 1:
            for (h = 0; h < height / 2; h++)
            {
                for (w = 0; w < width / 2; w++)
                {
                    unsigned char * v0 = &img->raw_data[w*2 + h*2 * width]; // x
                    unsigned char * v1 = &img->raw_data[w*2 + 1 + h*2 * width]; // >
                    unsigned char * v2 = &img->raw_data[w*2 + (h*2 + 1) * width]; // v
                    unsigned char * v3 = &img->raw_data[w*2 + 1 + (h*2 + 1) * width]; // >v

                    img->raw_data[w + h * (width / 2)] = (*v0 + *v1 + *v2 + *v3) / 4;
                }
            }
        break;
    }

    img->sizeX /= 2;
    img->sizeY /= 2;
}


void save_image(img_t *img, int output_format, char *filename)
{
    int ret, v;
    png_t png_writer;
    jpg_t jpg_writer;
    png_bytep png_row_ptr;
    JSAMPROW jpg_row_ptr[1];

    char out_file_name[strlen(filename) + 4 + 1];
    strcpy(out_file_name, filename);

    switch(output_format)
    {
        case PNG:
            ret = open_png_writer(filename, &png_writer, img->channels, img->sizeX, img->sizeY);

            if (ret) {
                fprintf(stderr, "Could not open file \"%s\" for writing! %d\n", filename, ret);
                return;
            }

            for (v = 0; v < (int)(img->sizeY); v++) {
                png_row_ptr = (png_bytep) &img->raw_data[img->sizeX * v * img->channels];
                png_write_row(png_writer.png_ptr, png_row_ptr);
            }

            close_png_writer(&png_writer);
        break;

        case JPG:
            ret = open_jpg_writer(filename, &jpg_writer, img->channels, img->sizeX, img->sizeY);

            if (ret) {
                fprintf(stderr, "Could not open file \"%s\" for writing! %d\n", filename, ret);
                return;
            }

            for (v = 0; v < (int)(img->sizeY); v++) {
                jpg_row_ptr[0] = (JSAMPROW) &img->raw_data[img->sizeX * v * img->channels];
                jpeg_write_scanlines(&jpg_writer.cinfo, jpg_row_ptr, 1);
            }

            close_jpg_writer(&jpg_writer);
        break;
    }
}


int thumb_create(char *input_file_path, unsigned int size, char *out_file_name)
{
    int output_format = -1;
    int ret = 0;
    img_t img;
    img.tSize = size;

    if ( ret != -2 && (ret = read_png(input_file_path, &img)) == 0 )
    {
        strcat(out_file_name, ".png");
        output_format = PNG;
    }
    else if ( ret != -2 && (ret = read_jpg(input_file_path, &img)) == 0 )
    {
        strcat(out_file_name, ".jpg");
        output_format = JPG;
    }
//     else if ( read_ppm(input_file_path, &img) == 0 )
//     {
//         output_format = PNG;
//     }

    if (ret) {
        return ret;
    }

    while (img.sizeX > size || img.sizeY > size)
        scale_image(&img);

    save_image(&img, output_format, out_file_name);

    return 0;
}
