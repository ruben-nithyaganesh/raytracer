#ifndef CAMERA
#define CAMERA
#include "vector.h"
#include "hittable.h"
#include "bitmap.h"
#include "random_util.h"

#define CAMERA_ANTI_ALIASING 0x00000001

typedef struct {
    Point           pos;
    double          aspect_ratio;
    int             image_width;
    int             image_height;
    double          focal_length;
    double          viewport_height;
    double          viewport_width;
    int             samples_per_pixel;
    double          pixel_sample_scale;
    struct {
        Color start;
        Color end;
    }               bg_linear_gradient;
    Vector3         viewport_u;
    Vector3         viewport_v;
    Vector3         viewport_upper_left;
    Vector3         pixel_delta_u;
    Vector3         pixel_delta_v;
    Vector3         pixel_delta;
    Vector3         half_pixel_delta;
    int             max_depth;
    int             flags;
} Camera;

Camera init_camera(
    Point position,
    double aspect_ratio,
    double image_height,
    double focal_length,
    double viewport_height,
    int samples_per_pixel,
    int flags
) {
    Camera camera;
    camera.pos              = position;
    camera.aspect_ratio     = aspect_ratio;
    camera.image_height     = image_height;
    camera.image_width      = camera.image_height * camera.aspect_ratio;
    camera.image_height     = (camera.image_height > 1) ? camera.image_height : 1;

    camera.focal_length     = focal_length;
    camera.viewport_height  = viewport_height;
    camera.viewport_width   = camera.viewport_height * ((double)camera.image_width / camera.image_height);

    camera.samples_per_pixel    = samples_per_pixel;
    camera.pixel_sample_scale   = 1.0 / camera.samples_per_pixel;

    camera.bg_linear_gradient.start = (Color){0.5, 0.7, 1.0};
    camera.bg_linear_gradient.end   = (Color){1.0, 1.0, 1.0};

    camera.viewport_u       = (Vector3)  {camera.viewport_width, 0.0,                     0.0};
    camera.viewport_v       = (Vector3)  {0.0,                   -camera.viewport_height, 0.0};

    camera.pixel_delta_u    = vector3_div(camera.viewport_u,     camera.image_width  );
    camera.pixel_delta_v    = vector3_div(camera.viewport_v,     camera.image_height );
    camera.pixel_delta      = vector3_add(camera.pixel_delta_u,  camera.pixel_delta_v);
    camera.half_pixel_delta = vector3_mul(camera.pixel_delta,    0.5                 );

    camera.viewport_upper_left = vector3_add(
        camera.pos,
        (Vector3)
        {-(camera.viewport_width / 2.0),
        (camera.viewport_height / 2.0),
        -camera.focal_length}
    );

    camera.max_depth = 10;
    camera.flags = flags;
    return camera;
}

Ray sample_ray(Camera camera, Point current_pixel, Ray ray) {

    Vector3 sample_square = (Vector3){
        (my_random_double() - 0.5) * camera.pixel_delta.x,
        (my_random_double() - 0.5) * camera.pixel_delta.y,
        0
    };
    Point perturbed = vector3_add(current_pixel, sample_square);

    Ray sampled_ray;
    sampled_ray.origin = camera.pos;
    sampled_ray.direction = vector3_sub(perturbed, ray.origin);
    return sampled_ray;
}   

Color ray_color(Camera *camera, Ray ray, World *world, int depth) {
    if(depth <= 0) {
        return (Color){0., 0., 0.,};
    }
    Hit_Record hit_record;

    int hit_index = find_hit(world, ray, &hit_record);
    if(hit_index != -1) {
        Hittable hit = world->hittables[hit_index];
        switch(hit.material.type) {
            case DIFFUSE:
            {
                Vector3 direction = vector3_add(hit_record.normal, vector3_random_unit_vector());
                if(vector3_near_zero(direction))
                    direction = hit_record.normal;
                Ray bounced = (Ray){hit_record.point, direction};
                Color col   = ray_color(camera, bounced, world, depth-1);
                return color_scale(col, hit.material.albedo);
            }break;
            case METAL:
            {
                Vector3 reflected = vector3_reflect(ray.direction, hit_record.normal);
                Ray bounced = (Ray){hit_record.point, reflected};
                Color col   = ray_color(camera, bounced, world, depth-1);
                return color_scale(col, hit.material.albedo);
            }break;
            case COLOR_NORMAL:
            {
                Vector3 surface_normal = hit_record.normal;
                return (Color) {
                    0.5*(surface_normal.x+1),
                    0.5*(surface_normal.y+1),
                    0.5*(surface_normal.z+1)
                };
            }break;
            default:
                return (Color){0., 0., 0.,};
        }
    }

    // render default background if no objects hit
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

void render(Camera camera, World *world, Pixel pixels[]) {

    Point pixel00 = vector3_add(camera.viewport_upper_left, camera.half_pixel_delta);
    Point current_pixel_point = pixel00;

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
        current_pixel_point.x   = pixel00.x;
        current_pixel_point.y   += camera.pixel_delta.y;
        printf("Row %d/%d\n", y+1, camera.image_height);
    }
}
#endif