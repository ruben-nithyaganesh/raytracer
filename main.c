#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "vector.h"
#include "hittable.h"
#include "camera.h"

Pixel fragment(int x, int y, int width, int height) {
    Pixel p;
    p.r = (unsigned char)(255.0 * ((float) (x+1) / width));
    p.g = (unsigned char)0;
    p.b = (unsigned char)(255.0 * ((float) (y+1) / height));
    return p;
}

int main(int argc, char **argv) {
    
    // options
    int anti_aliasing = 0;
    if(argc > 1){
        if(strcmp("--aa", argv[1]) == 0) {
            anti_aliasing = CAMERA_ANTI_ALIASING;
        }
    }

    Camera camera = init_camera(
        (Point){0.0, 0.0, 0.0}, // position
        (16.0 / 9.0),           // aspect ratio
        400,                    // image height
        1.0,                    // focal length
        2.0,                    // viewport height
        100,                    // samples per pixel
        0 | anti_aliasing
    );

    World world = init_world();
    Material diffuse_material;
    diffuse_material.type = DIFFUSE;
    diffuse_material.diffuse.absorption = 0.4;

    Material dark_diffuse_material;
    dark_diffuse_material.type = DIFFUSE;
    dark_diffuse_material.diffuse.absorption = 0.2;

    Material normal_col_material;
    normal_col_material.type = COLOR_NORMAL;
    add_hittable(&world, h_sphere((Point){0., 0.,     -1.}, normal_col_material, .5));
    add_hittable(&world, h_sphere((Point){0., -100.5, -1.}, diffuse_material, 100));

    int size = camera.image_width * camera.image_height;

    Pixel *mem_block = (Pixel *)malloc(2 * size * sizeof(Pixel));
    Pixel *pixels = mem_block;
    Pixel *point_samples = mem_block + size;

    render(camera, &world, point_samples, pixels);

    save_ppm("image.ppm", pixels, camera.image_width, camera.image_height);
}