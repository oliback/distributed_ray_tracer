#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include <stdio.h>
#include <SDL/SDL.h>
#include <vector>
#include <float.h>
#include <mpi.h>

#include "vec3.h"
#include "algebra.h"
#include "pointlight.h"
#include "sphere.h"
#include "image.h"
#include "ray.h"
#include "timer.h"

#define invsqrt(x) (1.0f / sqrt(x))
#define MASTER 1
#define WORKER 2

using namespace std;

typedef struct Camera {
    Vec3<float> position;
    Vec3f look;
    Vec3f up;
    float fovX;
    float fovY;
    float tan_fovX;
    float tan_fovY;
    float zDir;
} Camera;

class Scene {
public:
    vector<Sphere> spheres;
    PointLight light;

    Scene(void) {
         
    }

    Vec3f get_light_ray_direction(const Vec3f &point) {
        return Vec3<float>();
    }

    void set_light_pos(float x, float y, float z) {
        light.pos.x = x;
        light.pos.y = y;
        light.pos.z = z;
    }

    void add(Sphere s) {
        
        spheres.push_back(s);
        //cout << "Sphere added: " << "r = " << spheres[spheres.size()-1].r << endl;
    }
};

class RayTracer {
private: 
	Scene * scene;
	Image * image;
	unsigned long long test_counter;
    Camera camera;
    int width;
    int height;
    int recursion_depth;
    
    Vec3f getEyeRayDirection(int, int);

public:
    void dbg_printf(const char*, ...);
	void set_camera(float, float, float, float, float);
	void print_test_count();
	RayTracer(Scene*, Image*, int);
	void searchClosestHit(const Ray&, HitRec&);
    Vec3f trace_ray(Ray&, HitRec&, int);
    Vec3f fire_ray(int, int);
    void parallell_fire_rays(int, int);
    Vec3f get_pixel_color(const int, const int);
    void set_image(Image*);
    void set_dimension(int, int);
    int get_width();
    int get_height();
};

#endif
