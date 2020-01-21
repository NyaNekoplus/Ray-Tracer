#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "hitable.h"

inline double fade(double t) {
	return t * t * t * (t * (6 * t - 15) + 10);
}

inline double trilinear_interpolation(vec3 hash[2][2][2],double u,double v,double w) {
	double uu, vv, ww;
#if 0
	u = fade(uu);
	v = fade(vv);
	w = fade(ww);
#else
	uu= u * u * (3 - 2 * u);	//hermite cubic
	vv = v * v * (3 - 2 * v);
	ww = w * w * (3 - 2 * w);
#endif
	double accum=0;
	for(int di=0;di<2;di++)
		for(int dj=0;dj<2;dj++)
			for (int dk = 0; dk < 2; dk++) {
				vec3 weight_v(u - di, v - dj, w - dk);
				accum += (di * uu + (1-di) * (1 - uu)) *
						 (dj * vv + (1-dj) * (1 - vv)) *
						 (dk * ww + (1-dk) * (1 - ww)) * dot(hash[di][dj][dk],weight_v);
			}
	return accum;
}



class perlin {
public:
	double noise(const vec3 p) const {
		double u = p.x() - floor(p.x());	//求小数部分
		double v = p.y() - floor(p.y());
		double w = p.z() - floor(p.z());
		int i = floor(p.x());		//整数部分 把下标限定在255以内
		int j = floor(p.y());
		int k = floor(p.z());
		vec3 hs[2][2][2];
		for(int di=0;di<2;++di)
			for(int dj=0;dj<2;++dj)
				for (int dk = 0; dk < 2; ++dk) {
					hs[di][dj][dk]= ran_vec3[perm_x[(i+di)&255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];	//哈希取值
				}
		return trilinear_interpolation(hs, u, v, w);
	}

	double turb(const vec3& p, int depth = 7)const {
		double accum = 0;
		vec3 temp_p = p;
		double weight = 1.0;
		for (int i = 0; i < depth; ++i) {
			accum += weight * noise(temp_p);
			weight *= 0.5;
			temp_p *= 2;
		}
		return fabs(accum);
	}
	static vec3* ran_vec3;
	static int* perm_x;
	static int* perm_y;
	static int* perm_z;
};
static vec3* perlin_generate() {
	vec3* d = new vec3[256];
	for (int i = 0; i < 256; ++i)
		d[i] = unit_vector(vec3(-1+2*drand48(), -1 + 2 * drand48(), -1 + 2 * drand48()));
	return d;
}
static int* permute(int* d, int n) {		//打乱哈希值排序
	for (int i = n - 1; i > 0; --i) {
		int target = int(drand48() * (1 + n));
		int temp = d[i];
		d[i] = d[target];
		d[target] = temp;
	}
	return d;
}
static int* perlin_generate_perm() {
	int* p = new int[256];
	for (int i = 0; i < 256; ++i)
		p[i] = i;
	permute(p, 256);
	return p;
}
vec3* perlin::ran_vec3 = perlin_generate();
int* perlin::perm_x=perlin_generate_perm();
int* perlin::perm_z=perlin_generate_perm();
int* perlin::perm_y= perlin_generate_perm();
///////////////////////////////////////////////////////////////////////////////////


class texture {
public:
	virtual vec3 value(double u, double v, const vec3& p)const = 0;
};

class constant_texture :public texture {
public:
	constant_texture(){}
	constant_texture(vec3 c):color(c){}
	virtual vec3 value(double u, double v, const vec3& p)const {
		return color;
	}
	vec3 color;
};

class check_texture :public texture {
public:
	check_texture(){}
	check_texture(texture* t0, texture* t1):even(t0),odd(t1){}
	virtual vec3 value(double u, double v, const vec3& p)const {
		double sines = cos(10 * p.x()) * cos(10 * p.y()) * cos(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
	texture* odd;
	texture* even;
};

class noise_texture :public texture {
public:
	noise_texture(){}
	noise_texture(double s, vec3 color = vec3(1, 1, 1)) :scale(s), rgb(color) {}
	virtual vec3 value(double u, double v, const vec3& p)const {
		//return vec3(1, 1, 1) * noise.noise(p);
		//return vec3(1, 1, 1) * noise.noise(scale*p);
		//return rgb * noise.turb(scale * p);
		return rgb * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
	}
	double scale;
	vec3 rgb;
	perlin noise;
};

class image_texture :public texture {
public:
	image_texture(){}
	image_texture(unsigned char* pixels, int A, int B) :data(pixels), nx(A), ny(B){}
	virtual vec3 value(double u, double v, const vec3& p)const;
	unsigned char* data;
	int nx, ny;
};

vec3 image_texture::value(double u, double v, const vec3& p)const {
	int i = (1 - u) * nx;
	int j = (1 - v) * ny - 0.001;
	if (i < 0)i = 0;
	if (j < 0)j = 0;
	if (i > nx - 1)i = nx - 1;
	if (j > ny - 1)j = ny - 1;
	double r = int(data[3 * i + 3 * nx * j]) / 255.0;
	double g = int(data[3 * i + 3 * nx * j + 1]) / 255.0;
	double b = int(data[3 * i + 3 * nx * j + 2]) / 255.0;
	return vec3(r, g, b);
}

#endif