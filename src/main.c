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
    if (channels == 1) return pixel[0]; // Grayscale
    if (channels == 3 || channels == 4) {
        return (pixel[0] + pixel[1] + pixel[2]) / 3; // RGB(A)
    }
    return 0;
}

int comparePixels(const void *a, const void *b) {
    unsigned char *pixelA = (unsigned char *)a;
    unsigned char *pixelB = (unsigned char *)b;
    
    unsigned char brightnessA = get_brightness(pixelA, 3);
    unsigned char brightnessB = get_brightness(pixelB, 3);
    
    return brightnessA - brightnessB;
}

void sortInterval(unsigned char *img, int start_idx, int end_idx, int channels) {
    int num_pixels = (end_idx - start_idx) + 1;

    unsigned char *pixels = malloc(num_pixels * channels);

    for (int i = 0; i < num_pixels; i++) {
        for (int c = 0; c < channels; c++) {
            pixels[i * channels + c] = img[(start_idx + i) * channels + c];
        }
    }

    qsort(pixels, num_pixels, channels * sizeof(unsigned char), comparePixels);

    for (int i = 0; i < num_pixels; i++) {
        for (int c = 0; c < channels; c++) {
            img[(start_idx + i) * channels + c] = pixels[i * channels + c];
        }
    }
    
    free(pixels);
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
            unsigned char *mask_p = mask + (channels * (j * width + i));

            unsigned char r = p[0]; // red
            unsigned char g = p[1]; // green 
            unsigned char b = p[2]; // blue

            unsigned char brightness = (r + g + b) / 3;

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

    int i = 0;
    int j = 0;
    while (i < width * height) {
        unsigned char *p = img + (channels * (j * width + i));
        unsigned char *mask_p = mask + (channels * (j * width + i));

        unsigned char arr[width * height];
        int aux = 0;

        if (&mask_p == 255) {
            arr[i] = aux++;
            if (arr[i + 1] == 0) {
                aux = 0;
                i++;
            }
        }
        j++;
    }

    stbi_write_jpg("../data/sorted-image.jpg", width, height, channels, mask, 100);
}