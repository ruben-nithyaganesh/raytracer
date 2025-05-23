#ifndef HITTABLE_OBJECT
#define HITTABLE_OBJECT

#include "vector.h"

#define MAX_HITTABLES 5

typedef enum {
    DIFFUSE,
    COLOR_NORMAL,
    n_material_types
} Material_Type;

typedef struct {
    Material_Type type;
    union {
        struct {
            float absorption;
        } diffuse;
    };
} Material;

typedef enum {
    SPHERE,
    n_hittable_types
} Hittable_Type;

typedef struct {
    double radius;
} Sphere;

typedef struct {
    Hittable_Type type;
    Material      material;
    Point         pos;
    union {
        Sphere sphere;
    };
} Hittable;

typedef struct {
    Hittable hittables[MAX_HITTABLES];
    int      count;
} World;

typedef struct {
    Point   point;
    Vector3 normal;
    double  t;
    int     front_face;
} Hit_Record;

Hittable h_sphere(Point p, Material material, double radius) {
    Sphere s;
    s.radius = radius;

    Hittable h;
    h.type = SPHERE;
    h.pos = p;
    h.sphere = s;

    h.material = material;
    return h;
}

World init_world() {
    World world;
    memset(world.hittables, 0, MAX_HITTABLES * sizeof(world.hittables[0]));
    world.count = 0;
    return world;
}

void add_hittable(World *world, Hittable h) {
    if(world->count >= MAX_HITTABLES){
        printf("Max hittables reached.\n");
        return;
    }

    world->hittables[world->count++] = h;
}

void set_face_normal(Hit_Record *hit_record, Ray ray) {
    // This assumes hit_record->normal is already normalised
    hit_record->front_face = (vector3_dot(ray.direction, hit_record->normal) < 0) ? 1 : 0;
    if(!hit_record->front_face) {
       hit_record->normal = vector3_invert(hit_record->normal);
    }
}

int hit_sphere(Hittable hittable_sphere, Ray ray, Hit_Record *hit_record, double t_min, double t_max) {
    // given a ray, solve for time t for
    // when the ray intersects with this sphere
    // done by solving for t with the quadratic equation
    // t^2d.d - 2td.(C - Q) + (C - Q).(C - Q)
    // where C := center, Q := origin of r
    Point center = hittable_sphere.pos;
    double radius = hittable_sphere.sphere.radius;
    Vector3 oc = vector3_sub(center, ray.origin);
    double a = vector3_mag_squared(ray.direction);
    double h = vector3_dot(ray.direction, oc);
    double c = vector3_mag_squared(oc) - radius*radius;
    double discriminant = h*h - a*c;

    // < 0 means no solutions, i.e no intersections
    if(discriminant < 0) {
        return 0;
    }

    // >= 0 means one or two solutions
    // return out the closer one if it is range
    double sqrtd = sqrt(discriminant);
    double root = (h - sqrtd) / a;
    if(root <= t_min || t_max <= root) {
        double root = (h + sqrtd) / a;
        if(root <= t_min || t_max <= root) {
            return 0;
        }
    }

    hit_record->t = root;

    // in the case of a sphere, we can get the surface normal by
    // substracting the sphere centre from the point of intersection
    hit_record->normal = vector3_sub(ray_at(ray, hit_record->t), center);
    hit_record->normal = vector3_div(hit_record->normal, radius);
    
    set_face_normal(hit_record, ray);

    hit_record->point = ray_at(ray, hit_record->t);
    return 1;
}

int hit_object(Hittable hittable, Ray ray, Hit_Record *hit_record, double t_min, double t_max) {
    switch (hittable.type)
    {
        case SPHERE:
            return hit_sphere(hittable, ray, hit_record, t_min, t_max);
        default:
            return 0;
    }
}

int find_hit(World *world, Ray ray, Hit_Record *hit_record) {
    // iterate through objects in world looking for a hit
    Hit_Record closest;
    closest.t = 100000.0;
    int hit_index = -1;
    for(int i = 0; i < world->count; i++) {
        Hittable h = world->hittables[i];
        Hit_Record test_hit;
        if(hit_object(h, ray, &test_hit, 0.001, 100000.0)) {
            if(closest.t > test_hit.t) {
                hit_index = i;
                closest = test_hit;
            }
        }
    }
    *hit_record = closest;
    return hit_index;
}
#endif
