#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "vec3.h"

class Image {
private:
	int width, height;
	Vec3f * pixels;

public:
	Image(const int w, const int h) : width(w), height(h) {
		pixels = new Vec3f[width * height];
	}
	~Image(void) {
		delete [] pixels;
	}
	int getWidth(void) const { return width; }
	int getHeight(void) const { return height; }
	void setPixel(const int x, const int y, const Vec3f & c) { /*pixels[y * width + x] = c;*/
        pixels[y * width + x].r = c.r;
        pixels[y * width + x].g = c.g;
        pixels[y * width + x].b = c.b;
    }
	Vec3f getPixel(const int x, const int y) { return pixels[y * width + x]; }
	Vec3f * getPixelBufferPtr(void) { return pixels; }
};

#endif
