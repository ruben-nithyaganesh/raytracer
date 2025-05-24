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