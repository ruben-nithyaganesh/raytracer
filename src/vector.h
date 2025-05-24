#ifndef VECTOR
#define VECTOR
#include "math.h"
#include "random_util.h"

typedef struct {
    double x, y, z;
} Vec3;

typedef Vec3 Point;

Vec3 init_vec3(double x, double y, double z) {
    Vec3 vec3;
    vec3.x = x;
    vec3.y = y;
    vec3.z = z;
    return vec3;
}

Vec3 vec3_mul(Vec3 vec3, double s) {
    Vec3 a;
    a.x = vec3.x * s;
    a.y = vec3.y * s;
    a.z = vec3.z * s;
    return a;
}

Vec3 vec3_div(Vec3 vec3, double s) {
    Vec3 a;
    a.x = vec3.x / s;
    a.y = vec3.y / s;
    a.z = vec3.z / s;
    return a;
}

double vec3_dot(Vec3 vec1, Vec3 vec2) {
    double dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    return dot;
}

Vec3 vec3_add(Vec3 vec3, Vec3 to_add) {
    Vec3 a;
    a.x = vec3.x + to_add.x;
    a.y = vec3.y + to_add.y;
    a.z = vec3.z + to_add.z;
    return a;
}

Vec3 vec3_sub(Vec3 vec3, Vec3 to_sub) {
    Vec3 a;
    a.x = vec3.x - to_sub.x;
    a.y = vec3.y - to_sub.y;
    a.z = vec3.z - to_sub.z;
    return a;
}

Vec3 vec3_invert(Vec3 vec3) {
    Vec3 a;
    a.x = -vec3.x;
    a.y = -vec3.y;
    a.z = -vec3.z;
    return a;
}

double vec3_mag_squared(Vec3 vec) {
    double sum = 0;
    sum += vec.x*vec.x;
    sum += vec.y*vec.y;
    sum += vec.z*vec.z;
    return sum;
}

double vec3_mag(Vec3 vec) {
    return sqrt(vec3_mag_squared(vec));
}

Vec3 vec3_unit_vector(Vec3 vec) {
    double mag = vec3_mag(vec);
    Vec3 unit_vector = vec3_mul(vec, (1.0 / mag));
    return unit_vector;
}

void vec3_print(Vec3 vec) {
    printf("vec3(%.5f, %.5f, %.5f)\n", vec.x, vec.y, vec.z);
}

Vec3 vec3_random() {
    return (Vec3) {my_random_double(), my_random_double(), my_random_double()};
}

Vec3 vec3_random_between(double min, double max) {
    return (Vec3) {my_random_double_between(min, max), my_random_double_between(min, max), my_random_double_between(min, max)};
}

Vec3 vec3_random_unit_vector() {
    while(1) {
        Point p = vec3_random_between(-1, 1);
        double length_squared = vec3_mag_squared(p);

        // lesson has this condition, but don't think we care about the 1 here since
        // we normalise the vector to unit length anyway ?
        // if(1e-160 < length_squared && length_squared <= 1) { 
        if(1e-160 < length_squared) {
            Vec3 normalised = vec3_unit_vector(p);
            return normalised;
        }
    }
}

Vec3 vec3_random_on_hemisphere(Vec3 normal) {
    Vec3 on_unit_sphere = vec3_random_unit_vector();
    if(vec3_dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    } else {
        return vec3_invert(on_unit_sphere);
    }
}

int vec3_near_zero(Vec3 v) {
    double s = 1e-8;
    return (v.x <= s && v.y <= s && v.z <= s);
}

Vec3 vec3_reflect(Vec3 v, Vec3 normal) {
    double proj = vec3_dot(v, normal);
    return vec3_sub(v, vec3_mul(normal, 2*proj));
}

typedef struct {
    Point origin;
    Vec3 direction;
} Ray;

Ray ray_init(Point origin, Vec3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    return ray;
}

Point ray_at(Ray ray, double t) {
    Vec3 scaled_dir = vec3_mul(vec3_unit_vector(ray.direction), t);
    Point position = vec3_add(ray.origin, scaled_dir);
    return position;
}

#endif