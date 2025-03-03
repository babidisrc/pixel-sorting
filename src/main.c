#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int low_threshold = 10;
const int high_threshold = 100;

int main (int argc, char *argv[]) {
    int width, height, channels;
    unsigned char *img = stbi_load("../data/duck.jpg", &width, &height, &channels, 0);
     if (img == NULL) {
        printf("Error in loading the image.\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    size_t img_size = width * height * channels;
    unsigned char *mask = malloc(img_size);
    if (mask == NULL) {
        printf("Unable to allocate memory for the mask layer.\n");
        exit(1);
    }
    
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            unsigned char *p = img + (channels * (j * width + i));
            unsigned char *mask_p = mask + (channels * (j * width + i));

            unsigned char r = p[0]; // red
            unsigned char g = p[1]; // green 
            unsigned char b = p[2]; // blue

            unsigned char brightness = (r + g + b) / 3;

            if (brightness > low_threshold && brightness < high_threshold) {
                mask_p[0] = 0;
                mask_p[1] = 0;
                mask_p[2] = 0;
            }
            else {
                mask_p[0] = 255;
                mask_p[1] = 255;
                mask_p[2] = 255;
            }
        }
    }
    stbi_write_jpg("../data/duck2.jpg", width, height, channels, mask, 100);
}