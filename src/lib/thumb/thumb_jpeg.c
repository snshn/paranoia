#include "thumb.h"
#include "thumb_jpeg.h"


static void my_error_exit (j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message) (cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}


int read_jpg(const char * file, img_t * img)
{
    /* these are standard libjpeg structures for reading(decompression) */
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
    /* libjpeg data structure for storing one row, that is, scanline of an image */
    JSAMPROW scanline_buffer[1];

    FILE *fp = fopen(file, "rb");

    /* here we set up our own custom error handler */
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return -2;
    }
    /* setup decompression process and source, then read JPEG header */
    jpeg_create_decompress(&cinfo);
    /* this makes the library read from fp */
    jpeg_stdio_src(&cinfo, fp);

    /* reading the image header which contains image information */
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return -1;
    }

    if (cinfo.image_width <= img->tSize && cinfo.image_height <= img->tSize) {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return -2;
    }

    /*
    fprintf(stderr, "JPEG File Information: \n");
    fprintf(stderr, "Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height);
    fprintf(stderr, "Color components per pixel: %d.\n", cinfo.num_components);
    fprintf(stderr, "Color space: %d.\n", cinfo.jpeg_color_space);
    //*/

    /* Start decompression jpeg here */
    //cinfo.dct_method = JDCT_FASTEST;
    jpeg_start_decompress(&cinfo);

    img->channels = cinfo.num_components;
    img->sizeX = cinfo.output_width;
    img->sizeY = cinfo.output_height;

    /* allocate memory to hold the uncompressed image */
    img->raw_data = malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    /* now actually read the jpeg into the raw buffer */
    scanline_buffer[0] = malloc(cinfo.output_width * cinfo.num_components);
    /* read one scan line at a time */
    //fprintf(stderr, "presize: %d %d\n", cinfo.output_width, cinfo.output_height);

    //unsigned char * jdata;        // data for the image
    //jdata = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
    while (cinfo.output_scanline < cinfo.output_height)
    {
        scanline_buffer[0] = (unsigned char*)img->raw_data + cinfo.num_components * cinfo.output_width * cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, scanline_buffer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return 0;
}


int open_jpg_writer(const char * file, jpg_t * jpg, int channels, int sizeX, int sizeY)
{
    jpg->fp = fopen(file, "wb");
    if (!jpg->fp) return 2;

    jpg->cinfo.err = jpeg_std_error(&jpg->jerr);
    jpeg_create_compress(&jpg->cinfo);
    jpeg_stdio_dest(&jpg->cinfo, jpg->fp);

    jpg->cinfo.image_width = sizeX;
    jpg->cinfo.image_height = sizeY;
    jpg->cinfo.input_components = channels;
    jpg->cinfo.in_color_space = channels == 3 ? JCS_RGB : JCS_GRAYSCALE;

    jpeg_set_defaults(&jpg->cinfo);
    jpeg_set_quality(&jpg->cinfo, JPEG_QUALITY, TRUE);
    jpeg_start_compress(&jpg->cinfo, TRUE);

    return 0;
}


void close_jpg_writer(jpg_t * jpg)
{
    jpeg_finish_compress(&jpg->cinfo);
    jpeg_destroy_compress(&jpg->cinfo);

    fclose(jpg->fp);
}
