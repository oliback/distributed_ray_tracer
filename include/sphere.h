#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "vec3.h"
#include "ray.h"

class Sphere {
public:
	Vec3f c;
	float r;
	Vec3f color;
	Vec3f ambient_abs;
	Vec3f diffuse_abs;
	Vec3f specular_abs;
	float shininess;
	float specular_intensity; // TODO: needed?
	float reflection;

    Sphere(const Sphere& other);
    Sphere() { c = Vec3f(0.0f, 0.0f, 0.0f); r = 0.0f, color = Vec3f(0.0f, 0.0f, 0.0f); reflection = 0.0f; shininess = 0.0f;}
	Sphere(const Vec3f & cen, float rad) : c(cen), r(rad) { reflection = 0.0f; shininess = 0.0f; }

	bool is_reflective() { return (reflection > 0); }
	bool hit(const Ray &r, float &t);
	void computeSurfaceHitFields(const Ray & r, HitRec & rec) const;
	void set_color(float r, float g, float b) { color.r = r; color.g = g; color.b = b; }
	void set_shininess(float s) { shininess = s; }
	void set_reflection(float r) { reflection = r; }
	void set_specular(float x, float y, float z) { specular_abs.x = x; specular_abs.y = y; specular_abs.z = z; }
	void set_diffuse(float x, float y, float z) { diffuse_abs.x = x; diffuse_abs.y = y; diffuse_abs.z = z; }
	void set_ambient(float x, float y, float z) { ambient_abs.x = x; ambient_abs.y = y; ambient_abs.z = z; }
};

#endif
