#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vec3.h"

class PointLight
{
private:

public:
	Vec3f pos;
	Vec3f ambient;
	Vec3f diffuse;
	Vec3f specular;
	/* TODO: implement color intensity in the pointlight */

	PointLight() { }
	PointLight(float x, float y, float z);

	void set_specular(float x, float y, float z) { specular.x = x; specular.y = y; specular.z = z; }
	void set_diffuse(float x, float y, float z) { diffuse.x = x; diffuse.y = y; diffuse.z = z; }
	void set_ambient(float x, float y, float z) { ambient.x = x; ambient.y = y; ambient.z = z; }
};

#endif
