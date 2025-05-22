#ifndef BITMAP
#define BITMAP
#include <stdio.h>
#include <string.h>

#define CLAMP(n, a, b)  n = (n < a) ? a : (n > b) ? b : n

typedef struct Pixel {
    unsigned char r, g, b;
} Pixel;

typedef struct Color {
    float r, g, b;
} Color;

// void save_bitmap(Pixel data[], const char* file_name, int width, int height, int dpi) {

//     struct bitmap_file_header {
//         unsigned char   bitmap_type[2];     // 2 bytes
//         int             file_size;          // 4 bytes
//         short           reserved1;          // 2 bytes
//         short           reserved2;          // 2 bytes
//         unsigned int    offset_bits;        // 4 bytes
//     } bfh;

//     // bitmap image header (40 bytes)
//     struct bitmap_image_header {
//         unsigned int    size_header;        // 4 bytes
//         unsigned int    width;              // 4 bytes
//         unsigned int    height;             // 4 bytes
//         short int       planes;             // 2 bytes
//         short int       bit_count;          // 2 bytes
//         unsigned int    compression;        // 4 bytes
//         unsigned int    image_size;         // 4 bytes
//         unsigned int    ppm_x;              // 4 bytes
//         unsigned int    ppm_y;              // 4 bytes
//         unsigned int    clr_used;           // 4 bytes
//         unsigned int    clr_important;      // 4 bytes
//     } bih;

//     // create a file object that we will use to write our image
//     FILE *image;
//     // we want to know how many pixels to reserve
//     int image_size = width * height;
//     // a byte is 4 bits but we are creating a 24 bit image so we can represent a pixel with 3
//     // our final file size of our image is the width * height * 4 + size of bitmap header
//     int file_size = 54 + 4 * image_size;
//     // pixels per meter https://www.wikiwand.com/en/Dots_per_inch
//     int ppm = dpi * 39.375;

//     // if you are on Windows you can include <windows.h>
//     // and make use of the BITMAPFILEHEADER and BITMAPINFOHEADER structs

//     memcpy(&bfh.bitmap_type, "BM", 2);
//     bfh.file_size       = file_size;
//     bfh.reserved1       = 0;
//     bfh.reserved2       = 0;
//     bfh.offset_bits     = 0;

//     bih.size_header     = sizeof(bih);
//     bih.width           = width;
//     bih.height          = height;
//     bih.planes          = 1;
//     bih.bit_count       = 24;
//     bih.compression     = 0;
//     bih.image_size      = image_size;
//     bih.ppm_x           = ppm;
//     bih.ppm_y           = ppm;
//     bih.clr_used        = 0;
//     bih.clr_important   = 0;

//     image = fopen(file_name, "wb");

//     // compiler woes so we will just use the constant 14 we know we have
//     fwrite(&bfh, 1, 14, image);
//     fwrite(&bih, 1, sizeof(bih), image);

//     // write out pixel data, one last important this to know is the ordering is backwards
//     // we have to go BGR as opposed to RGB
//     for (int i = 0; i < image_size; i++) {
//         Pixel BGR = data[i];

//         float red   = (BGR.r);
//         float green = (BGR.g);
//         float blue  = (BGR.b);

//         // if you don't follow BGR image will be flipped!
//         unsigned char color[3] = {
//             blue, green, red
//         };

//         fwrite(color, 1, sizeof(color), image);
//     }

//     fclose(image);
// }

void save_ppm(const char* filename, Pixel data[], int width, int height) {
    FILE *file = fopen(filename, "wb");
    if(!file) { printf("Something went wrong.\n"); return; }

    fprintf(file, "P3\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");

    for(int row = 0; row < height; row++) {
        for(int col = 0; col < width; col++) {
            Pixel p = data[width * row + col];
            fprintf(file, "%d %d %d%s", p.r, p.g, p.b, (col == width - 1) ? "" : " ");
        }
        fprintf(file, "\n");
    }

}

Pixel pixel_from_floats(double r, double g, double b) {
    Pixel p;
    CLAMP(r, 0.0, 1.0);
    CLAMP(g, 0.0, 1.0);
    CLAMP(b, 0.0, 1.0);
    p.r = (unsigned char) (255.0 * r);
    p.g = (unsigned char) (255.0 * g);
    p.b = (unsigned char) (255.0 * b);
    return p;
}

Pixel pixel_add(Pixel pixel, Pixel to_add) {
    Pixel p;
    p.r = pixel.r + to_add.r;
    p.g = pixel.g + to_add.g;
    p.b = pixel.b + to_add.b;
    return p;
}

Pixel pixel_scale(Pixel pixel, double scale) {
    Pixel p;
    p.r = pixel.r * scale;
    p.g = pixel.g * scale;
    p.b = pixel.b * scale;
    return p;
}

Pixel lerp_pixel(Pixel p1, Pixel p2, double a) {
    if(a <= 0) return p1;
    if(a >= 1.0) return p2;
    Pixel p_lerped;
    p_lerped.r = (1.0 - a) * p1.r + a * p2.r;
    p_lerped.g = (1.0 - a) * p1.g + a * p2.g;
    p_lerped.b = (1.0 - a) * p1.b + a * p2.b;
    return p_lerped;
}

Color lerp_color(Color p1, Color p2, double a) {
    if(a <= 0) return p1;
    if(a >= 1.0) return p2;
    Color p_lerped;
    p_lerped.r = (1.0 - a) * p1.r + a * p2.r;
    p_lerped.g = (1.0 - a) * p1.g + a * p2.g;
    p_lerped.b = (1.0 - a) * p1.b + a * p2.b;
    return p_lerped;
}

Color color_add(Color pixel, Color to_add) {
    Color p;
    p.r = pixel.r + to_add.r;
    p.g = pixel.g + to_add.g;
    p.b = pixel.b + to_add.b;
    return p;
}

Color color_scale(Color color, double scale) {
    Color scaled;
    scaled.r = color.r * scale;
    scaled.g = color.g * scale;
    scaled.b = color.b * scale;
    return scaled;
}

double inverse_gamma(double linear_component) {
    if(linear_component > 0) {
        return sqrt(linear_component);
    }
    return 0;
}

Color linear_to_gamma(Color c) {
    Color gamma;
    gamma.r = inverse_gamma(c.r);
    gamma.g = inverse_gamma(c.g);
    gamma.b = inverse_gamma(c.b);
    return gamma;
}

Pixel pixel_from_color(Color c) {
    c = linear_to_gamma(c);
    return pixel_from_floats(c.r, c.g, c.b);
}
#endif 