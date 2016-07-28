#include<pnghelpers.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
    
int main ()
{
    /* Create an image. */

    pngh_bitmap_t *fruit = pngh_bitmap_con(100, 100);

    for (int y = 0; y < fruit->height; y++) {
        for (int x = 0; x < fruit->width; x++) {
            pngh_pixel_t * pixel = pngh_pixel_at (fruit, x, y);
            pixel->red = pngh_pix (x, fruit->width);
            pixel->blue = pngh_pix (y, fruit->height);
        }
    }

    /* Write the image to a file 'fruit.png'. */

    pngh_write (fruit, "fruit.png");

    pngh_bitmap_des(fruit);

    return 0;
}
