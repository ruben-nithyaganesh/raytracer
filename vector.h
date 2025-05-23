#ifndef VECTOR
#define VECTOR
#include "math.h"
#include "random_util.h"

typedef struct {
    double x, y, z;
} Vector3;

typedef Vector3 Point;

Vector3 init_vector3(double x, double y, double z) {
    Vector3 vec3;
    vec3.x = x;
    vec3.y = y;
    vec3.z = z;
    return vec3;
}

Vector3 vector3_mul(Vector3 vec3, double s) {
    Vector3 a;
    a.x = vec3.x * s;
    a.y = vec3.y * s;
    a.z = vec3.z * s;
    return a;
}

Vector3 vector3_div(Vector3 vec3, double s) {
    Vector3 a;
    a.x = vec3.x / s;
    a.y = vec3.y / s;
    a.z = vec3.z / s;
    return a;
}

double vector3_dot(Vector3 vec1, Vector3 vec2) {
    double dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    return dot;
}

Vector3 vector3_add(Vector3 vec3, Vector3 to_add) {
    Vector3 a;
    a.x = vec3.x + to_add.x;
    a.y = vec3.y + to_add.y;
    a.z = vec3.z + to_add.z;
    return a;
}

Vector3 vector3_sub(Vector3 vec3, Vector3 to_sub) {
    Vector3 a;
    a.x = vec3.x - to_sub.x;
    a.y = vec3.y - to_sub.y;
    a.z = vec3.z - to_sub.z;
    return a;
}

Vector3 vector3_invert(Vector3 vec3) {
    Vector3 a;
    a.x = -vec3.x;
    a.y = -vec3.y;
    a.z = -vec3.z;
    return a;
}

double vector3_mag_squared(Vector3 vec) {
    double sum = 0;
    sum += vec.x*vec.x;
    sum += vec.y*vec.y;
    sum += vec.z*vec.z;
    return sum;
}

double vector3_mag(Vector3 vec) {
    return sqrt(vector3_mag_squared(vec));
}

Vector3 vector3_unit_vector(Vector3 vec) {
    double mag = vector3_mag(vec);
    Vector3 unit_vector = vector3_mul(vec, (1.0 / mag));
    return unit_vector;
}

void vector3_print(Vector3 vec) {
    printf("vec3(%.5f, %.5f, %.5f)\n", vec.x, vec.y, vec.z);
}

Vector3 vector3_random() {
    return (Vector3) {my_random_double(), my_random_double(), my_random_double()};
}

Vector3 vector3_random_between(double min, double max) {
    return (Vector3) {my_random_double_between(min, max), my_random_double_between(min, max), my_random_double_between(min, max)};
}

Vector3 vector3_random_unit_vector() {
    while(1) {
        Point p = vector3_random_between(-1, 1);
        double length_squared = vector3_mag_squared(p);

        // lesson has this condition, but don't think we care about the 1 here since
        // we normalise the vector to unit length anyway ?
        // if(1e-160 < length_squared && length_squared <= 1) { 
        if(1e-160 < length_squared) {
            Vector3 normalised = vector3_unit_vector(p);
            return normalised;
        }
    }
}

Vector3 vector3_random_on_hemisphere(Vector3 normal) {
    Vector3 on_unit_sphere = vector3_random_unit_vector();
    if(vector3_dot(on_unit_sphere, normal) > 0.0) {
        return on_unit_sphere;
    } else {
        return vector3_invert(on_unit_sphere);
    }
}

int vector3_near_zero(Vector3 v) {
    double s = 1e-8;
    return (v.x <= s && v.y <= s && v.z <= s);
}

typedef struct {
    Point origin;
    Vector3 direction;
} Ray;

Ray ray_init(Point origin, Vector3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    return ray;
}

Point ray_at(Ray ray, double t) {
    Vector3 scaled_dir = vector3_mul(vector3_unit_vector(ray.direction), t);
    Point position = vector3_add(ray.origin, scaled_dir);
    return position;
}

#endif