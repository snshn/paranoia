#include "thumb.h"
#include "thumb_png.h"

png_uint_32 count_color_channels(png_uint_32 * color_type)
{
    int bytes_per_pixel = 1; /* grayscale by default */

    if (*color_type & PNG_COLOR_MASK_COLOR)
        bytes_per_pixel = 3; /* it's RGB */

    if (*color_type & PNG_COLOR_MASK_ALPHA)
        bytes_per_pixel += 1; /* +1 for alpha */

    return bytes_per_pixel;
}

int read_png(const char *file, img_t *img)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int i;

    fp = fopen(file, "rb");
    if (!fp) return 2;

    unsigned char sig[8];
    i = fread(sig, 1, 8, fp);
    if (!i || !png_check_sig(sig, 8)) return 3;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) return 4;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) return 5;

    // use longjmp for error handling..
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 6;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8); // tell libpng we already read 8 bytes!

    // third argument: transformation parameters...
    png_read_png(png_ptr, info_ptr, 0, NULL);

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    //if (!isPowOfTwo(height) || !isPowOfTwo(width)) return 7;

    if (width <= img->tSize && height <= img->tSize) {
        fclose(fp);
        return -2;
    }

    //png_uint_32 bitdepth = png_get_bit_depth(png_ptr, info_ptr);
    //png_uint_32 channels = png_get_channels(png_ptr, info_ptr);
    //if (channels != 3) return 8;

    png_uint_32 color_type = png_get_color_type(png_ptr, info_ptr);
    //if (color_type != 2) return 9;

    /* DEBUG
    png_uint_32 row_bytes  = png_get_rowbytes(png_ptr, info_ptr);
    printf("depth = %d, channels = %d, color_type = %d, rowbytes = %d\n", bitdepth, channels, color_type, row_bytes);
    */

    img->channels = (int)count_color_channels(&color_type);
    img->sizeX = width;
    img->sizeY = height;

    int bytes_per_scanline = width * img->channels;

    png_bytepp scanline_buffer = png_get_rows(png_ptr, info_ptr);
    // allocate data needed to store the image data
    img->raw_data = malloc(height * bytes_per_scanline);

    for (i = 0; i < (int)(height); i++) {
        //fprintf(stderr, ", %d\n", sizeof(rgba_t));
        memcpy(&img->raw_data[i * bytes_per_scanline], scanline_buffer[i], bytes_per_scanline);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    return 0;
}

int png_type(int * channels)
{
    switch(*channels){
        case 4:
            return PNG_COLOR_TYPE_RGBA;
        break;
        case 3:
            return PNG_COLOR_TYPE_RGB;
        break;
        case 2:
            return PNG_COLOR_TYPE_GRAY_ALPHA;
        break;
        case 1:
            return PNG_COLOR_TYPE_GRAY;
        break;
    }

    return -1;
}

int open_png_writer(const char *file, png_t *png, int channels, int sizeX, int sizeY)
{
    png->fp = fopen(file, "wb");
    if (!png->fp) return 2;

    png->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png->png_ptr) return 3;

    png->info_ptr = png_create_info_struct(png->png_ptr);
    if (!png->info_ptr) return 4;

    if (setjmp(png_jmpbuf(png->png_ptr))) {
        png_destroy_write_struct(&png->png_ptr, &png->info_ptr);
        fclose(png->fp);
        return 5;
    }

    png_init_io(png->png_ptr, png->fp);

    png_set_compression_level(png->png_ptr, Z_BEST_COMPRESSION);
    //png_set_compression_level(png->png_ptr, Z_BEST_SPEED);

    png_set_IHDR(png->png_ptr, png->info_ptr,
                    sizeX, sizeY,
                    8, png_type(&channels), PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png->png_ptr, png->info_ptr);

    return 0;
}

void close_png_writer(png_t *png)
{
    png_write_end(png->png_ptr, png->info_ptr);
    png_destroy_write_struct(&png->png_ptr, &png->info_ptr);

    fclose(png->fp);
}
