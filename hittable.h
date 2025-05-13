#ifndef HITTABLE_OBJECT
#define HITTABLE_OBJECT

#include "vector.h"

#define MAX_HITTABLES 10

typedef enum {
    SPHERE,
    n_hittable_types
} Hittable_Type;

typedef struct {
    double radius;
} Sphere;

typedef struct {
    Hittable_Type type;
    Point pos;
    union {
        Sphere sphere;
    };
} Hittable;

typedef struct {
    Hittable hittables[MAX_HITTABLES];
    int count;
} Hittable_List;

typedef struct {
    Point point;
    Vector3 normal;
    double t;
    int front_face;
} Hit_Record;

Hittable h_sphere(Point p, double radius) {
    Sphere s;
    s.radius = radius;

    Hittable h;
    h.type = SPHERE;
    h.pos = p;
    h.sphere = s;
    return h;
}

Hittable_List empty_hittable_list() {
    Hittable_List hl;
    memset(hl.hittables, 0, MAX_HITTABLES * sizeof(hl.hittables[0]));
    hl.count = 0;
    return hl;
}

void add_hittable(Hittable_List *hl, Hittable h) {
    if(hl->count >= MAX_HITTABLES-1){
        printf("Hittable count exceeded.");
        return;
    }

    hl->hittables[hl->count++] = h;
}

// This assumes we have a normalised normal vector already calculated in the given 
// hit record. Will perform a dot product to determine whether the normal is outward or inward 
// facing and encode that information in the 'front_face' variable. Finally, if the normal is found to 
// be back facing we will invert it to be front facing
void set_face_normal(Hit_Record *hit_record, Ray ray) {
    // check if the normal is in the same direction as our ray, if so, it is 'front-facing'
    hit_record->front_face = (vector3_dot(ray.direction, hit_record->normal) < 0) ? 1 : 0;

    // if not front facing, invert our normal
    if(!hit_record->front_face) {
       hit_record->normal = vector3_invert(hit_record->normal);
    }
}

// section 6.2 simplifies this
int hit_sphere(Hittable hittable_sphere, Ray ray, Hit_Record *hit_record, double t_min, double t_max) {
    // given a ray, solve for time t for when the ray intersects with this sphere
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

    // if the discriminant is < 0 we have no solutions, ray never intersects with sphere
    if(discriminant < 0) {
        return 0;
    }
    // if >= 0 we have one or two solutions, so return out the closer one if it is range
    double sqrtd = sqrt(discriminant);
    double root = (h - sqrtd) / a;
    if(root <= t_min || t_max <= root) {
        double root = (h + sqrtd) / a;
        if(root <= t_min || t_max <= root) {
            return 0;
        }
    }

    // at this point we know we have the closest valid t if it exists
    hit_record->t = root;

    // in the case of a sphere, we can get the surface normal by substracting the sphere centre from the point of intersection (i.e, the position of the ray at the hit time t), which we additionally normalise to a unit vector.
    hit_record->normal = vector3_sub(ray_at(ray, hit_record->t), center);
    hit_record->normal = vector3_div(hit_record->normal, radius);
    
    // determine front facingness (?) of normal and invert if back facing
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

#endif
