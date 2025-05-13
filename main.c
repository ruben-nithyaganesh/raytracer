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

    Camera camera = init_camera(
        (Point){0.0, 0.0, 0.0}, // position
        (16.0 / 9.0),           // aspect ratio
        400.0,                  // image height
        1.0,                    // focal length
        2.0                     // viewport height
    );

    Hittable_List hl = empty_hittable_list();
    add_hittable(&hl, h_sphere((Point){0., 0., -2.0}, 1.0));

    Pixel pixels[camera.image_width * camera.image_height];
    render(camera, &hl, pixels);
    save_ppm("image.ppm", pixels, camera.image_width, camera.image_height);
}