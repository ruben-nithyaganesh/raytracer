#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bitmap.h"
#include "vector.h"
#include "hittable.h"
#include "camera.h"

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
    Material diffuse;
    diffuse.type = DIFFUSE;
    diffuse.albedo = 0.2;

    Material lighter_diffuse;
    lighter_diffuse.type = DIFFUSE;
    lighter_diffuse.albedo = 0.5;

    Material darker_diffuse;
    darker_diffuse.type = DIFFUSE;
    darker_diffuse.albedo = 0.1;

    Material metal;
    metal.type = METAL;
    metal.albedo = 0.9;

    Material darker_metal;
    darker_metal.type = METAL;
    darker_metal.albedo = 0.5;

    Material normal;
    normal.type = COLOR_NORMAL;

    add_hittable(&world, h_sphere((Point){0.,       0.8,     -2.5},    metal,            1.));
    add_hittable(&world, h_sphere((Point){0.5,      -0.25,    -.8},    normal,          .15));
    add_hittable(&world, h_sphere((Point){-0.5,     -0.25,    -.8},    lighter_diffuse, .15));
    add_hittable(&world, h_sphere((Point){0,        -0.25,    -.8},    metal,           .15));
    add_hittable(&world, h_sphere((Point){0.17,     -0.35,   -.57},    darker_diffuse,  .04));
    add_hittable(&world, h_sphere((Point){-0.17,    -0.35,   -.57},    normal,          .04));
    add_hittable(&world, h_sphere((Point){0.,       -100.5,   -2.},    diffuse,        100.));

    int size = camera.image_width * camera.image_height;

    Pixel *pixels = (Pixel *)malloc(size * sizeof(Pixel));

    render(camera, &world, pixels);

    save_ppm("out/image.ppm", pixels, camera.image_width, camera.image_height);
}