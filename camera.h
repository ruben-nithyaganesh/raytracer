#ifndef CAMERA
#define CAMERA
#include "vector.h"
#include "hittable.h"
#include "bitmap.h"
#include "random_util.h"

#define CAMERA_ANTI_ALIASING 0x00000001

typedef struct {
    Point pos;

    double aspect_ratio;

    int image_width;
    int image_height;

    double focal_length;

    double viewport_height;
    double viewport_width;

    int samples_per_pixel;
    double pixel_sample_scale;

    struct {
        Color start;
        Color end;
    } bg_linear_gradient;

    Vector3 viewport_u;
    Vector3 viewport_v;
    Vector3 viewport_upper_left;

    Vector3 pixel_delta_u;
    Vector3 pixel_delta_v;
    Vector3 pixel_delta;
    Vector3 half_pixel_delta;

    int max_depth;
    int flags;
} Camera;

Camera init_camera(Point position, double aspect_ratio, double image_height, double focal_length, double viewport_height, int flags) {
    Camera camera;
    camera.aspect_ratio = aspect_ratio;
    camera.image_height = image_height;
    camera.image_width = camera.image_height * camera.aspect_ratio;
    camera.image_height = (camera.image_height > 1) ? camera.image_height : 1;

    camera.focal_length = focal_length;
    camera.viewport_height = viewport_height;
    camera.viewport_width = camera.viewport_height * ((double)camera.image_width / camera.image_height);
    camera.pos = position;

    camera.samples_per_pixel = 100;
    camera.pixel_sample_scale = 1.0 / camera.samples_per_pixel;

    camera.bg_linear_gradient.start = (Color){.5, .7, 1.};
    camera.bg_linear_gradient.end   = (Color){1.0, 1.0, 1.0};

    camera.viewport_u       = (Vector3){camera.viewport_width, 0.0, 0.0};
    camera.viewport_v       = (Vector3){0.0, -camera.viewport_height, 0.0};
    camera.pixel_delta_u    = vector3_div(camera.viewport_u, camera.image_width);
    camera.pixel_delta_v    = vector3_div(camera.viewport_v, camera.image_height);
    camera.pixel_delta      = vector3_add(camera.pixel_delta_u, camera.pixel_delta_v);
    camera.half_pixel_delta = vector3_mul(camera.pixel_delta, 0.5);

    printf("pixel delta");
    vector3_print(camera.pixel_delta);
    // upper left pixel of viewport
    camera.viewport_upper_left = vector3_add(
        camera.pos,
        (Vector3) {-(camera.viewport_width / 2.0),  // move x,y to top left corner
        (camera.viewport_height / 2.0),
        -camera.focal_length}                       // move 'backwards' into z
    );

    camera.max_depth = 10;
    camera.flags = flags;
    return camera;
}

Ray sample_ray(Camera camera, Vector3 current_pixel_point, Ray ray) {
    Ray sampled_ray;
    // ray currently points to the correct pixel for a point sample,
    // to generate an anti aliased sample we sample around this point
    Point offset = (Vector3){current_pixel_point.x, current_pixel_point.y, current_pixel_point.z};

    // perturb the current point by a random small amount
    Vector3 sample_square = (Vector3){my_random_double() - 0.5, my_random_double() - 0.5, 0};
    offset = vector3_add(offset, (Vector3){sample_square.x * camera.pixel_delta.x, sample_square.y * camera.pixel_delta.y, 0});

    // sample this offset ray
    sampled_ray.origin = camera.pos;
    sampled_ray.direction = vector3_sub(offset, ray.origin);
    return sampled_ray;
}   

Color ray_color(Camera *camera, Ray ray, World *world, int depth) {
    if(depth <= 0) {
        return (Color){0., 0., 0.,};
    }
    Hit_Record hit_record;
    // iterate through objects in world looking for a hit
    for(int i = 0; i < world->count; i++) {
        Hittable h = world->hittables[i];
        if(hit_object(h, ray, &hit_record, 0.001, 100000.0)) {
            
            Vector3 direction = vector3_add(hit_record.normal, vector3_random_unit_vector());
            Ray bounced = (Ray){hit_record.point, direction};

            // double random_number = my_random_double();
            // if(random_number <= 0.5) {
            //     return color_scale(ray_color(camera, bounced, world, 1), 0.5);
            // }
            return color_scale(ray_color(camera, bounced, world, depth-1), 0.5);

            // Vector3 surface_normal = hit_record.normal;
            // Color col = {
            //     0.5*(surface_normal.x+1),
            //     0.5*(surface_normal.y+1),
            //     0.5*(surface_normal.z+1)
            // };
            // return col;
        }
    }

    // if no objects were hit, render the default background
    Vector3 unit_dir = vector3_unit_vector(ray.direction);
    double a = 0.5 * (unit_dir.y + 1.0);
    Color lerped = lerp_color(
        camera->bg_linear_gradient.end,
        camera->bg_linear_gradient.start,
        a
    );
    return lerped;
}

Pixel pixel_at(Pixel pixels[], int x, int y, int image_width, int image_height) {
    if(x < 0 || x >= image_width || y < 0 || y >= image_height) {
        return (Pixel){0, 0, 0};
    }
    else {
        return pixels[y * image_width + x];
    }
}

Pixel convolved(Pixel point_samples[], int x, int y, int image_width, int image_height, int kernel) {
    Pixel pixel;
    pixel.r = 0;
    pixel.g = 0;
    pixel.b = 0;
    int r, g, b;
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x-1, y-1, image_width, image_height), (1.0/        18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x, y-1, image_width, image_height), (1.0/          18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x+1, y-1, image_width, image_height), (1.0/        18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x-1, y, image_width, image_height), (1.0/          18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x, y, image_width, image_height), (1.0/            3.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x+1, y, image_width, image_height), (1.0/          18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x-1, y+1, image_width, image_height), (1.0/        18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x, y+1, image_width, image_height), (1.0/          18.)));
    pixel = pixel_add(pixel, pixel_scale(pixel_at(point_samples, x+1, y+1, image_width, image_height), (1.0/        18.)));
    return pixel;
}

void render(Camera camera, World *world, Pixel point_samples[], Pixel pixels[]) {

    Point pixel00 = vector3_add(camera.viewport_upper_left, camera.half_pixel_delta);
    Point current_pixel_point = pixel00;

    vector3_print(camera.pixel_delta_u);
    vector3_print(camera.pixel_delta_v);
    // calculate point samples
    Pixel *p = pixels;
    for (int y = 0; y < camera.image_height; y++) {
        for (int x = 0; x < camera.image_width; x++) {

            Vector3 ray_direction = vector3_sub(current_pixel_point, camera.pos);   
            Ray ray   = (Ray){camera.pos, ray_direction};
            Color col = (Color){0., 0., 0.};
            
            if(camera.flags && CAMERA_ANTI_ALIASING) {
                for(int i= 0; i < camera.samples_per_pixel; i++) {
                    Ray sampled_ray = sample_ray(camera, current_pixel_point, ray);
                    col = color_add(col, ray_color(&camera, sampled_ray, world, camera.max_depth));
                }
                col = color_scale(col, camera.pixel_sample_scale);
            } else {
                col = ray_color(&camera, ray, world, camera.max_depth);
            }
            *p = pixel_from_color(col);
            p++;
            current_pixel_point = vector3_add(current_pixel_point, camera.pixel_delta_u);
        }
        current_pixel_point.x = pixel00.x;
        current_pixel_point = vector3_add(current_pixel_point, camera.pixel_delta_v);
    }
    // now fill in actual rendered image
    // p = pixels;
    // for (int y = 0; y < camera.image_height; y++) {
    //     for (int x = 0; x < camera.image_width; x++) {
    //         *p = convolved(point_samples, x, y, camera.image_width, camera.image_height, 3);
    //         p++;
    //     }
    // }
}
#endif