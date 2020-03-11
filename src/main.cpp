#include <stdio.h>
#include "SDL/SDL.h"
#include <vector>
#include <float.h>
#include <mpi.h>
#include <unistd.h>

#include "vec3.h"
#include "algebra.h"
#include "pointlight.h"
#include "sphere.h"
#include "image.h"
#include "raytracer.h"
#include "timer.h"

#define WIDTH 1920
#define HEIGHT 1080
#define BPP 4
#define DEPTH 32

using namespace std;

RayTracer *ray_tracer;

void add_random_sphere(int xMin, int xMax, int yMin, int yMax, Scene *scene, int zMin, int zMax)
{
	int x = rand() % ((int)xMax - (int)xMin);
	x += xMin;
	int y = rand() % ((int)yMax - (int)yMin);
	y += yMin;
	int z = rand() % ((int)zMax - (int)zMin);
	z += zMin;
	float rad = (float)(rand() % 4);
	Sphere sphere(Vec3f(x, y, z), rad);

	float r = (float)(rand() % 200);
	r /= 255.0f;
	float g = (float)(rand() % 200);
	g /= 255.0f;
	float b = (float)(rand() % 200);
	b /= 255.0f;

	sphere.set_color(r, g, b);
	sphere.set_ambient(0.1f,0.1f,0.1f);
	sphere.set_diffuse(0.5f,0.5f,0.5f);
	float spec = (float)(rand() % 255);
	spec /= 255.0f;
	float shininess = (float)(rand() % 140);
	shininess = shininess < 10.0f ? 10.0f : shininess;
	sphere.set_shininess(shininess);
	sphere.set_specular(spec, spec, spec);
	float reflect = (float)(rand() % 255);
	reflect /= 255.0f;
	sphere.set_reflection(reflect);

	scene->add(sphere);
}

void init()
{
    Scene* scene;
    scene = new Scene();

    // slave processes does not need an image (NULL)
    ray_tracer = new RayTracer(scene, NULL);
    ray_tracer->set_dimension(WIDTH, HEIGHT);

    ray_tracer->set_camera(0.0f, 0.0f, 0.0f, 55.0f/*fovX*/, -1.0f/*dirZ*/);
}

void master_init()
{
    Image* image;
    Scene* scene;
    
    image = new Image(WIDTH, HEIGHT);
    scene = new Scene();

    int i;
    for (i=0; i<400; ++i) {
        add_random_sphere(-25, 25, -18, 18, scene, -80.0f, -50.0);
        add_random_sphere(-40, 40, -15, 15, scene, 10, 50);
    }

    // slave processes does not need an image (NULL)
    ray_tracer = new RayTracer(scene, image);
    ray_tracer->set_dimension(WIDTH, HEIGHT);

    ray_tracer->set_camera(0.0f, 0.0f, 0.0f, 55.0f/*fovX*/, -1.0f/*dirZ*/);
}

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;  
 
    colour = SDL_MapRGB( screen->format, r, g, b );
  
    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}


void DrawScreen(SDL_Surface* screen, int h)
{ 
    int x, y, ytimesw;
    Vec3f current_color(0.0f, 0.0f, 0.0f);
  
    if(SDL_MUSTLOCK(screen)) 
    {
        if(SDL_LockSurface(screen) < 0) return;
    }

    for(y = 0; y < screen->h; ++y)
    {
        ytimesw = y*screen->pitch/BPP;
        for( x = 0; x < screen->w; ++x) 
        {
            current_color = ray_tracer->get_pixel_color(x, y);
            setpixel(screen, x, ytimesw, (int)(current_color.r*255),
                    (int)(current_color.g*255), (int)(current_color.b*255));
        }
    }

    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  
    SDL_Flip(screen); 
}

int main(int argc, char* argv[])
{
    int rank=0, size;
    int tag=1;
    int i, j;
    MPI_Status stat;
    SDL_Surface *screen;
    SDL_Event event;
    int keypress = 0;
    int h=0;
    int time1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    /* master will set up the screen etc. */
    if (rank == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;
       
        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_RESIZABLE|SDL_HWSURFACE)))
        {
            SDL_Quit();
            return 1;
        }

        /* master needs to initialize the image */
        master_init();

        printf("timer started\n");

        /* seed after random spheres generated to get same random spheres each run */
        srand(time(NULL));
        time1 = Timer::timer();
    } else {
        /* slave processes needs to initialize the scene */
        init();
    }

    /* need to have a barrier here.
     * Otherwise, the execution timer can be fooled by other threads running first */
    MPI_Barrier(MPI_COMM_WORLD);

    ray_tracer->parallell_fire_rays(rank, size);

    MPI_Finalize();

    if (rank == 0) {
        time1 = Timer::timer() - time1;
        printf("the total calculation time was %f seconds\n", time1/Timer::SECOND_DIVIDER);

        /* outside loop, only want to render image once */
        DrawScreen(screen, h++);

        while(!keypress) {
             while(SDL_PollEvent(&event)) {      
                  switch (event.type) {
                      case SDL_QUIT:
                      keypress = 1;
                      break;
                      case SDL_KEYDOWN:
                           keypress = 1;
                           break;
                  }
             }
             usleep(10000);
        }

        SDL_Quit();
    }

    return 0;
}
