#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int low_threshold = 127;
const int high_threshold = 223;

unsigned char getBrightness(unsigned char *pixel, int channels) {
    if (channels == 1) return pixel[0];
    unsigned char brightness = (pixel[0] + pixel[1] + pixel[2]) / 3;
    return brightness;
}

int comparePixels(const void *a, const void *b) {
    unsigned char *pixelA = (unsigned char *)a;
    unsigned char *pixelB = (unsigned char *)b;
    unsigned char brightnessA = getBrightness(pixelA, 3);
    unsigned char brightnessB = getBrightness(pixelB, 3);
    return brightnessA - brightnessB;
}

void sortInterval(unsigned char *img, int start, int end, int channels) {
    int num_pixels = (end - start) + 1;
    unsigned char *pixels = malloc(num_pixels * channels);
    if (pixels == NULL) {
        printf("Unable to allocate memory for the sorting algorithm.\n");
        exit(1);
    }

    for (int i = 0; i < num_pixels; i++) {
        for (int c = 0; c < channels; c++) {
            pixels[i * channels + c] = img[(start + i) * channels + c];
        }
    }

    qsort(pixels, num_pixels, channels, comparePixels);

    for (int i = 0; i < num_pixels; i++) {
        for (int c = 0; c < channels; c++) {
            img[(start + i) * channels + c] = pixels[i * channels + c];
        }
    }

    free(pixels);
}

void prepareMask(unsigned char *img, unsigned char *mask, int width, int height, int channels, int mode) {
     if (mode == 0) {
        for (int x = 0; x < width; x++) {
        // interval start index
        int start = -1;
            for (int y = 0; y < height; y++) {
                unsigned char *p = img + (channels * (y + x * height));
                unsigned char *mask_p = mask + (y + x * height);

                if (mask_p[0] == 255 && start == -1) {
                    start = y + x * height;
                }

                if ((mask_p[0] != 255 || x == width - 1) && start != -1) {
                    int end = mask_p[0] == 255 ? y + x * height : (y + x * height) - 1;
                    sortInterval(img, start, end, channels);
                    start = -1;
                }
            }
        }
    }

    if (mode == 1) {
        for (int y = 0; y < height; y++) {
        // interval start index
        int start = -1;
            for (int x = 0; x < width; x++) {
                unsigned char *p = img + (channels * (y * width + x));
                unsigned char *mask_p = mask + (y * width + x);

                if (mask_p[0] == 255 && start == -1) {
                    start = y * width + x;
                }

                if ((mask_p[0] != 255 || x == width - 1) && start != -1) {
                    int end = mask_p[0] == 255 ? y * width + x : (y * width + x) - 1;
                    sortInterval(img, start, end, channels);
                    start = -1;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./pixel-sorting <image.jpg>");
        exit(1);
    }

    char *filename = argv[1];

    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 0);
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
            unsigned char *mask_p = mask + (j * width + i);

            unsigned char brightness = getBrightness(p, channels);

            if (brightness > low_threshold && brightness < high_threshold) {
                mask_p[0] = 255;
                mask_p[1] = 255;
                mask_p[2] = 255;
            }
            else {
                mask_p[0] = 0;
                mask_p[1] = 0;
                mask_p[2] = 0;
            }
        }
    }

    prepareMask(img, mask, width, height, channels, 0);
    prepareMask(img, mask, width, height, channels, 1);

    stbi_write_jpg("../data/sorted-image.jpg", width, height, channels, img, 100);
}