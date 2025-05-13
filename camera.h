#ifndef CAMERA
#define CAMERA
#include "vector.h"
#include "hittable.h"
#include "bitmap.h"

typedef struct {
    Point pos;

    double aspect_ratio;

    int image_width;
    int image_height;

    double focal_length;

    double viewport_height;
    double viewport_width;

} Camera;

Camera init_camera(Point position, double aspect_ratio, double image_height, double focal_length, double viewport_height) {
    Camera camera;
    camera.aspect_ratio = aspect_ratio;
    camera.image_height = image_height;
    camera.image_width = camera.image_height * camera.aspect_ratio;
    camera.image_height = (camera.image_height > 1) ? camera.image_height : 1;

    camera.focal_length = focal_length;
    camera.viewport_height = viewport_height;
    camera.viewport_width = camera.viewport_height * ((double)camera.image_width / camera.image_height);
    camera.pos = position;

    return camera;
}

Pixel ray_color(Ray ray, Hittable_List *hl) {
    Hit_Record hit_record;
    for(int i = 0; i < hl->count; i++) {
        Hittable h = hl->hittables[i];
        if(hit_object(h, ray, &hit_record, 1.0, 5.0)) {
            Vector3 surface_normal = hit_record.normal;
            return pixel_from_floats(0.5*(surface_normal.x+1), 0.5*(surface_normal.y+1), 0.5*(surface_normal.z+1));
        }
    }

    Pixel p;
    Vector3 unit_dir = vector3_unit_vector(ray.direction);
    double a = 0.5 * (unit_dir.y + 1.0);
    Pixel start = pixel_from_floats(.5, .7, 1.);
    Pixel end   = pixel_from_floats(1.0, 1.0, 1.0);
    Pixel lerped = lerp_pixel(end, start, a);
    return lerped;
}

void render(Camera camera, Hittable_List *hl, Pixel pixels[]) {
    // setup
    Vector3 viewport_u       = (Vector3){camera.viewport_width, 0.0, 0.0};
    Vector3 viewport_v       = (Vector3){0.0, -camera.viewport_height, 0.0};
    Vector3 pixel_delta_u    = vector3_div(viewport_u, camera.image_width);
    Vector3 pixel_delta_v    = vector3_div(viewport_v, camera.image_height);
    Vector3 pixel_delta      = vector3_add(pixel_delta_u, pixel_delta_v);
    Vector3 half_pixel_delta = vector3_mul(pixel_delta, 0.5);

    // upper left pixel
    // push back to viewport z, and move to top left of the view port
    Vector3 viewport_upper_left;
    viewport_upper_left = vector3_add(camera.pos, (Vector3){-(camera.viewport_width / 2.0), (camera.viewport_height / 2.0), -camera.focal_length});

    Point pixel00 = vector3_add(viewport_upper_left, half_pixel_delta);
    Pixel *p = pixels;

    Point current_pixel_point = pixel00;
    for (int y = 0; y < camera.image_height; y++) {
        for (int x = 0; x < camera.image_width; x++) {
            Vector3 ray_direction = vector3_sub(current_pixel_point, camera.pos);   
            Ray ray = (Ray){camera.pos, ray_direction};
            *p = ray_color(ray, hl);
            p++;
            current_pixel_point = vector3_add(current_pixel_point, pixel_delta_u);
        }
        current_pixel_point.x = pixel00.x;
        current_pixel_point = vector3_add(current_pixel_point, pixel_delta_v);
        //printf("Row %d/%d done.\n", (y+1), image_height);
    }

    // return pixels;
}
#endif