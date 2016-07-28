#include<png.h>
#include"pnghelpers.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

/* Constructor for a pngh_bitmap_t object. */

pngh_bitmap_t* pngh_bitmap_con(size_t nx, size_t ny) 
{
    pngh_bitmap_t* bitmap_ptr = malloc(sizeof(pngh_bitmap_t));
    bitmap_ptr->nx = nx;
    bitmap_ptr->ny = ny;
    bitmap_ptr->pixels = calloc(sizeof(pngh_pixel_t), nx*ny);
    return bitmap_ptr;
}

/* Destructor for a pngh_bitmap_t object. */

void pngh_bitmap_des(pngh_bitmap_t *bitmap)
{
    free(bitmap->pixels);
    free(bitmap);
}

/* Given "bitmap", this returns the pixel of bitmap at the point 
   ("x", "y"). */

pngh_pixel_t * pngh_pixel_at (pngh_bitmap_t * bitmap, int x, int y)
{
    x = (x % bitmap->nx);
    y = (y % bitmap->ny);
    return bitmap->pixels + bitmap->nx * y + x;
}
    
/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */

int pngh_write (pngh_bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->nx,
                  bitmap->ny,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->ny * sizeof (png_byte *));
    for (y = 0; y < bitmap->ny; ++y) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->nx * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->nx; ++x) {
            pngh_pixel_t * pixel = pngh_pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->ny; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

/* Given "value" and "max", the maximum value which we expect "value" to take,
 * this returns an integer between 0 and 255 proportional to "value" divided by
 * "max". */

int pngh_pix (double value, double max)
{
    if (value <= 0)
        return 0;
    if (value >= max)
        return 255;
    return (int) (256.0 *((double) (value)/(double) max));
}

/* Given a pixel and a colour scheme, get the value that is mapped to the colour
 * at the pixel. */

double pngh_val (const pngh_pixel_t *pixel, const pngh_cmap_t cmap)
{
    double rred, rgreen, rblue, x;
    rred = (double)(pixel->red - cmap.mincol.red) / (double)(cmap.maxcol.red - cmap.mincol.red);
    rgreen = (double)(pixel->green - cmap.mincol.green) / (double)(cmap.maxcol.green - cmap.mincol.green);
    rblue = (double)(pixel->blue - cmap.mincol.blue) / (double)(cmap.maxcol.blue - cmap.mincol.blue);
    x = cmap.minval + (cmap.maxval-cmap.minval)*(1./3.)*(rred+rgreen+rblue);
    return x;
}

/* Set a pixel to a certain colour. */

void pngh_pixel_setcol(pngh_pixel_t *pixel, const pngh_colour_t colour)
{
    pixel->red = colour.red;
    pixel->green = colour.green;
    pixel->blue = colour.blue;
}

/* Set a pixel's colour according to a value relative to a colourmap. */

void pngh_pixel_setcmap(pngh_pixel_t *pixel, double val, const pngh_cmap_t cmap)
{
    double r;
    r = (val - cmap.minval) / (cmap.maxval - cmap.minval);
    if (r > 1) 
        r = 1;
    if (r < 0)
        r = 0;

    pixel->red = floor ( (1-r) * (cmap.mincol.red) + r * (cmap.maxcol.red) );
    pixel->green = floor ( (1-r) * (cmap.mincol.green) + r * (cmap.maxcol.green) );
    pixel->blue = floor ( (1-r) * (cmap.mincol.blue) + r * (cmap.maxcol.blue) );
}

/* Example colours and colour maps */
/*
pngh_colour_t pngh_black = {.red = 0, .green = 0, .blue = 0}; 
pngh_colour_t pngh_grey = {.red = 127, .green = 127, .blue = 127}; 
pngh_colour_t pngh_white = {.red = 255, .green = 255, .blue = 255}; 
pngh_colour_t pngh_red = {.red = 255, .green = 0, .blue = 0}; 
pngh_colour_t pngh_cyan = {.red = 0, .green = 255, .blue = 255}; 

pngh_cmap_t pngh_greyscale = {
    .mincol = pngh_black, .maxcol = pngh_white, .minval = 0, .maxval = 1
};

pngh_cmap_t pngh_dennis_the_menace = {
    .mincol = pngh_black, .maxcol = pngh_red, .minval = 0, .maxval = 1
};
*/

/* Use a 2d array of doubles to fill in a bitmap, according to a colourmap. */

void pngh_table_to_bitmap(pngh_bitmap_t *bitmap, double** table, const pngh_cmap_t cmap)
{
    for (int i = 0; i < bitmap->nx; i++)
    {
        for (int j = 0; j < bitmap->ny; j++)
        {
            // fprintf(stderr, "i = %d, j = %d, val = %f\n", i, j, table[i][j]);
            pngh_pixel_t *pixel = pngh_pixel_at(bitmap, i, j);
            pngh_pixel_setcmap(pixel, table[i][j], cmap);
        }
    }
}


