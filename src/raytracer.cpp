#include "raytracer.h"

/* printf function that is only run if DEBUG is defined */
void RayTracer::dbg_printf(const char* fmt, ...)
{
    #ifdef DEBUG
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    #endif
}

Vec3f RayTracer::getEyeRayDirection(int x, int y) {
    float w = (float)get_width();
    float h = (float)get_height();

    Vec3f look(0.0f,0.0f,-1.0f);
    Vec3f up(0.0f,1.0f,0.0f);

    Vec3f u = look.cross(up);
    u = u / u.len();

    Vec3f v = u.cross(look);
    v = v / v.len();

    /* http://courses.csusm.edu/cs535xz/Lectures/raytracing.ppt */
    Vec3f dx = u * ((2.0f * camera.tan_fovX) / w);
    Vec3f dy = v * ((2.0f * camera.tan_fovY) / h);

    Vec3f target;
    target = look / look.len() + (dx * ((2.0f * x + 1.0f - w) / 2.0f)) + (dy * ((2.0f * y + 1.0f - h) / 2.0f));

    return target.getNormalized();
}

// only call after image->width and image->height has been set.
void RayTracer::set_camera(float x, float y, float z, float fovX, float zDir)
{
    camera.position.x = x;
    camera.position.y = y;
    camera.position.z = z;
    camera.fovX = (float)degrees_to_radians(fovX);
    // derive a suitable fovY from fovX
    camera.fovY = ((float)get_height() / (float)get_width()) * camera.fovX;
    camera.look = Vec3f(x, y, zDir);
    // needed for deriving eye rays, stupid to calc many times.
    camera.tan_fovX = tan(camera.fovX / 2.0f);
    camera.tan_fovY = tan(camera.fovY / 2.0f);
    camera.zDir = zDir;
}

void RayTracer::print_test_count()
{
    printf("%llu intersection tests were performed\n", test_counter);
}

RayTracer::RayTracer(Scene * scene, Image * image, int _recursion_depth) {
    this->scene = scene;
    this->image = image;
    recursion_depth = _recursion_depth;
    test_counter = 0;
}

void RayTracer::searchClosestHit(const Ray & ray, HitRec & hitRec) {
    float tdist = FLT_MAX;
    float t = FLT_MAX;
    
    for (int i=0; i<(int)scene->spheres.size(); ++i) {
        test_counter++;
        if (scene->spheres[i].hit(ray, t)) {
            hitRec.primIndex = i;
            hitRec.anyHit = true;
        }
    }

    if (hitRec.anyHit) {
        hitRec.tHit = t;
        hitRec.color = scene->spheres[hitRec.primIndex].color;
        scene->spheres[hitRec.primIndex].computeSurfaceHitFields(ray, hitRec);
    }
}

Vec3f RayTracer::trace_ray(Ray &ray, HitRec & hitRec, int rcounter) {
    searchClosestHit(ray, hitRec);
    Vec3f final_color;
    if (hitRec.anyHit) {
        Sphere *hit_sphere = NULL;
        hit_sphere = &scene->spheres[hitRec.primIndex];
        final_color = hit_sphere->color;

        float shadow = 0.0f;
        Vec3f N;
        Vec3f L;
        Vec3f R;
        Vec3f V;

        N = hitRec.n;
        L =  scene->light.pos - hitRec.p;
        L.normalize();
        V = camera.position - hitRec.p;
        V.flip();
        V.normalize();
        R = L - N * (2.0 * L.dot(N));

        /* shadow test */
        /* for each light... */
        for (int i=0; i<1; ++i) {
            Ray shadow_ray;
            shadow_ray.o = hitRec.p;
            float light_projection = L.dot(N);

            if (light_projection <= 0.0f) {
                continue;
            }

            float light_dist = L.dot(L);
            float temp = light_dist;
            if (temp == 0.0f) {
                continue;
            }

            temp = invsqrt(temp);
            shadow_ray.d = L * temp;
            shadow_ray.epsMoveStartAlongDir();

            float t = light_dist;
            for (int j=0; j<(int)scene->spheres.size(); ++j) {
                test_counter++;
                if (scene->spheres[j].hit(shadow_ray, t)) {
                    shadow = 1.0f;
                    break;
                }
            }

            /* phong shading */
            /* shade with ambient even if point is in shadow */
            final_color += hitRec.color.multCoordwise(hit_sphere->ambient_abs) +
                (scene->light.ambient.multCoordwise(hit_sphere->ambient_abs));

            float lambert_term = N.dot(L);

            /* shade with diffuse and specular lighting if the point is not in shadow */
            if (shadow != 1.0f && lambert_term > 0.0f)
            {
                final_color += (scene->light.diffuse.multCoordwise(
                    hit_sphere->diffuse_abs) *
                    lambert_term);

                float specular = pow((float)max_val(R.dot(V), 0.0),
                    hit_sphere->shininess);

                final_color += (scene->light.specular.multCoordwise(
                    hit_sphere->specular_abs) *
                    specular);
            }
        }

        /* if reflective, reflect ray */
        /* don't exceed recursion limit */
        if (hit_sphere->is_reflective() && rcounter < this->recursion_depth) {
            Ray reflect_ray;
            Vec3f reflect_color;
            float reflect;
            HitRec reflect_rec;

            reflect_rec.anyHit = false;
            reflect_color.set(0.0f,0.0f,0.0f);
            reflect = ray.d.dot(N) * 2.0f;
            reflect_ray.o = hitRec.p;
            reflect_ray.d = ray.d - (N * reflect);

            /* move start along dir to avoid hitting own sphere */
            reflect_ray.epsMoveStartAlongDir();

            reflect_color = trace_ray(reflect_ray, reflect_rec, rcounter+1);
            final_color *= (1 - hit_sphere->reflection);
            if (reflect_rec.anyHit) {
                final_color += reflect_color * hit_sphere->reflection;
            }
        }
    } else {
        final_color.set(0.0f,0.0f,0.0f);
    }

    return final_color;
}

Vec3f RayTracer::fire_ray(int x, int y)
{
    Ray ray;
    HitRec hitRec;
    
    Vec3f final_color;
    ray.o = camera.position;
    ray.d = getEyeRayDirection(x, y);
    hitRec.anyHit = false;
    int rcounter = 0;

    final_color = trace_ray(ray, hitRec, rcounter);

    if (!hitRec.anyHit) {
        final_color.set(0.0f,0.0f,0.0f);
    }

    /* TODO: use clamp where it seems ok */
    final_color.r = clamp(final_color.r);
    final_color.g = clamp(final_color.g);
    final_color.b = clamp(final_color.b);

    return final_color;
}

void RayTracer::parallell_fire_rays(int rank, int size)
{
    int x, y, i, yy;
    int height = get_height();
    int width = get_width();
    int leftover = height % size;
    int local_rows = height / size;
    int local_time;
    Vec3f color(0.0f, 0.0f, 0.0f);
    MPI_Status stat;
    float* pixel_buffer = NULL;
    int blocksize = local_rows * width * 3 + 3;// every 3 elements are one vector
    pixel_buffer = new float[blocksize];

    /* defaulting all values to 9.0f */
    for (i=0; i<blocksize; ++i) {
        pixel_buffer[i] = 9.0f;
    }

    if (rank > 0) {
        int sphere_count = 0;
        dbg_printf("slave %d receiving sphere_count var from master\n", rank);
        MPI_Recv(&sphere_count, 1, MPI_INT, 0, WORKER, MPI_COMM_WORLD, &stat);

        Sphere* sphere_array;
        sphere_array = new Sphere[sphere_count];

        dbg_printf("slave %d receiving spheres from master\n", rank);
        MPI_Recv(sphere_array, sphere_count * sizeof(Sphere), MPI_BYTE, 0, WORKER, MPI_COMM_WORLD, &stat);

        dbg_printf("slave %d received %d spheres\n", rank, sphere_count);

        /* copy the spheres to the local scene */
        for (i=0; i < sphere_count; ++i) {
            this->scene->add(sphere_array[i]);
        }

        delete[] sphere_array;

        dbg_printf("slave %d starting to fire rays\n", rank);

        // start the local timer
        local_time = Timer::timer();

        for (y = rank, yy = 0; yy < local_rows; y += size, ++yy) {
            for (x = 0; x < width; ++x) {
                color = fire_ray(x, y);
                pixel_buffer[yy * 3 * width + x * 3] = color.r;
                pixel_buffer[yy * 3 * width + x * 3 + 1] = color.g;
                pixel_buffer[yy * 3 * width + x * 3 + 2] = color.b;
            }
        }

        // stop the local timer
        local_time = Timer::timer() - local_time;

        dbg_printf("slave %d is finished shooting rays\n", rank);
        dbg_printf("slave %d trying to send %d elements to master\n", rank, blocksize);

        MPI_Send(pixel_buffer, blocksize, MPI_FLOAT, 0, WORKER, MPI_COMM_WORLD);

        dbg_printf("slave %d finished sending to master\n", rank);
    }

    if (rank == 0) {
        dbg_printf("size is:%d\n", size);

        if (size > 1) {
            /* send the scene to all slaves */
            int sphere_count = scene->spheres.size();
            Sphere* sphere_array;
            sphere_array = new Sphere[sphere_count];

            /* need to move from a vector to an array for sending */
            for (i=0; i<sphere_count; ++i) {
                sphere_array[i] = scene->spheres[i];
            }

            dbg_printf("master starting to send sphere_count and spheres to slaves\n");
            for (i=1; i<size; ++i) {
                /* inform slave how many objects will be sent */
                MPI_Send(&sphere_count, 1, MPI_INT, i, WORKER, MPI_COMM_WORLD);

                /* send the spheres */
                MPI_Send(sphere_array, sphere_count * sizeof(Sphere), MPI_BYTE, i, WORKER, MPI_COMM_WORLD);
            }
            dbg_printf("master finished sending sphere_count and spheres to slaves\n");

            /* we are done with the sphere array, delete it */
            delete[] sphere_array;
        }

        /* the master can manipulate the image array directly */
        dbg_printf("master is handling his own rays\n");

        // start the master's local timer
        local_time = Timer::timer();

        for (y = rank; y < height; y += size) {
            for (x = 0; x < width; ++x) {
                color = fire_ray(x, y);
                image->setPixel(x, y, color);
            }
        }
        dbg_printf("master is DONE handling his own rays\n");

        /* let master do leftover work */
        if (leftover > 0) {
            dbg_printf("master is handling leftovers\n");
            for (y = height-leftover; y < height; ++y) {
                for (x = 0; x < width; ++x) {
                    color = fire_ray(x, y);
                    image->setPixel(x, y, color);
                }
            }
            dbg_printf("master is done handling leftovers\n");
        }

        // stop the master's local timer
        local_time = Timer::timer() - local_time;

        /* now; receive information from the slaves */
        for (i = 1; i < size; ++i) {
            dbg_printf("master trying to receive %d elements from slave %d\n", blocksize, i);
            MPI_Recv(pixel_buffer, blocksize, MPI_FLOAT, i, WORKER, MPI_COMM_WORLD, &stat);
            dbg_printf("receive done from slave %d\n", i);

            /* paste the pixels into image array */
            for (y = i, yy = 0; yy < local_rows; y += size, ++yy) {
                for (x = 0; x < width; ++x) {
                    color.r = pixel_buffer[yy * 3 * width + x * 3];
                    color.g = pixel_buffer[yy * 3 * width + x * 3 + 1];
                    color.b = pixel_buffer[yy * 3 * width + x * 3 + 2];

                    image->setPixel(x, y, color);
                }
            }
        }
    }

    dbg_printf("|process %d took %f seconds to do local ray tracing|\n", rank, local_time/Timer::SECOND_DIVIDER);
    dbg_printf("|process %d performed %llu intersection tests|\n", rank, this->test_counter);
    
    dbg_printf("slave %d deleting pixel_buffer\n", rank);
    delete[] pixel_buffer;
    dbg_printf("slave %d has finished deleting pixel_buffer\n", rank);
}

Vec3f RayTracer::get_pixel_color(const int x, const int y)
{
    return this->image->getPixel(x, y);
}

void RayTracer::set_image(Image* img)
{
    if (this->image != NULL) {
        delete this->image;
    }

    this->image = img;
}

void RayTracer::set_dimension(int w, int h)
{
    this->width = w;
    this->height = h;
}

int RayTracer::get_width()
{
    return this->width;
}

int RayTracer::get_height()
{
    return this->height;
}
