#ifndef PNG_HELPERS_H
#define PNG_HELPERS_H

#include<png.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

/* A coloured pixel. */

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pngh_pixel_t;

/* A picture. */
    
typedef struct  {
    pngh_pixel_t *pixels;
    size_t nx;
    size_t ny;
} pngh_bitmap_t;

/* A colour. */

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pngh_colour_t;

/* A colour map. */

typedef struct {
    double minval;
    double maxval;
    pngh_colour_t mincol;
    pngh_colour_t maxcol;
} pngh_cmap_t;

/* Constructor for a pngh_bitmap_t object. */

pngh_bitmap_t* pngh_bitmap_con(size_t nx, size_t ny);

/* Destructor for a pngh_bitmap_t object. */

void pngh_bitmap_des(pngh_bitmap_t *bitmap);
    
/* Given "bitmap", this returns the pixel of bitmap at the point 
   ("x", "y"). */

pngh_pixel_t * pngh_pixel_at (pngh_bitmap_t * bitmap, int x, int y);

/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */

int pngh_write (pngh_bitmap_t *bitmap, const char *path);

/* Given "value" and "max", the maximum value which we expect "value" to take,
 * this returns an integer between 0 and 255 proportional to "value" divided by
 * "max". */

int pngh_pix (double value, double max);

/* Given a pixel and a colour scheme, get the value that is mapped to the colour
 * at the pixel. */

double pngh_val (const pngh_pixel_t *pixel, const pngh_cmap_t cmap);

/* Set a pixel to a certain colour. */

void pngh_pixel_setcol(pngh_pixel_t *pixel, const pngh_colour_t colour);

/* Set a pixel's colour according to a value relative to a colourmap. */

void pngh_pixel_setcmap(pngh_pixel_t *pixel, double val, const pngh_cmap_t cmap);

/* Use a 2d array of doubles to fill in a bitmap, according to a colourmap. */

void pngh_table_to_bitmap(pngh_bitmap_t *bitmap, double** table, const pngh_cmap_t cmap);


#endif
