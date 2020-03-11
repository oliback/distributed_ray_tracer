#include "sphere.h"

/* copy constructor */
Sphere::Sphere(const Sphere& other)
{
    this->c.x = other.c.x;
    this->c.y = other.c.y;
    this->c.z = other.c.z;
    
    this->r = other.r;

    this->color.r = other.color.r;
    this->color.g = other.color.g;
    this->color.b = other.color.b;

    this->ambient_abs.x = other.ambient_abs.x;
    this->ambient_abs.y = other.ambient_abs.y;
    this->ambient_abs.z = other.ambient_abs.z;

    this->diffuse_abs.x = other.diffuse_abs.x;
    this->diffuse_abs.y = other.diffuse_abs.y;
    this->diffuse_abs.z = other.diffuse_abs.z;

    this->specular_abs.x = other.specular_abs.x;
    this->specular_abs.y = other.specular_abs.y;
    this->specular_abs.z = other.specular_abs.z;

    this->shininess = other.shininess;
    
    this->specular_intensity = other.specular_intensity;

    this->reflection = other.reflection;
}

/* checking with &t to make sure we get the lower t (closest hit) */
bool Sphere::hit(const Ray &r, float &t) {
	/* http://buddharaytracer.googlecode.com/svn-history/r5/trunk/Raytrace.cpp */
    Vec3f dist = this->c - r.o;
	float B = r.d.dot(dist);
	float D = B*B - dist.dot(dist) + this->r * this->r;
    if (D < 0.0f)
        return false;
    float t0 = B - sqrtf(D);
    float t1 = B + sqrtf(D);
    bool retvalue = false;

    if ((t0 > 0.0f) && (t0 < t)) {
        t = t0;
        retvalue = true;
    }

    if ((t1 > 0.0f) && (t1 < t)) {
        t = t1;
        retvalue = true;
    }
    return retvalue;
}

void Sphere::computeSurfaceHitFields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.tHit;
	rec.n = (rec.p - c).normalize();
}
