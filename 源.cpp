#include <iostream>
#include "sence_function.h"

#define MAXdouble 3.40282e+038
vec3 color(const ray& r, hitable* world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, DBL_MAX, rec)) {
		double pdf;
		vec3 attenuation;
		ray scattered;
		vec3 emit = rec.mat_ptr->emitted(r,rec,rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered,pdf)) {
#if 0
			vec3 on_light = vec3(213 + random1 * (343 - 213),
				554,
				227 + random1 * (332 - 227));
			vec3 to_light = on_light - rec.p;
			double distance_squared = to_light.squared_length();
			to_light.make_unit_vector();
			if (dot(to_light, rec.n) < 0)
				return emit;
			double light_area = (343 - 213) * (332 - 227);
			double light_cosine = fabs(to_light.y());
			if (light_cosine < 0.000001)
				return emit;
			pdf = distance_squared / (light_cosine * light_area);
			scattered = ray(rec.p, to_light, r.time());
#elif 1
			hitable* light_shape = new xz_rect(213, 343, 227, 332, 554.99, 0);
			hittable_pdf p0(light_shape, rec.p);
			cosine_pdf p1(rec.n);
			mixture_pdf p(&p0, &p1);
			scattered = ray(rec.p, p.generate(), r.time());
			pdf = p.value(scattered.direction());
			delete light_shape;
#endif
			//rendering equation:http://en.wikipedia.org/wiki/Rendering_equation
			return emit + attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * color(scattered, world, depth + 1) / pdf;
			//return emit+attenuation * color(scattered, world, depth + 1);
		}
		else {
			return emit;
		}
	}
	else {
#if 1
		return vec3(0, 0, 0);
#else
		vec3 unit_direction = unit_vector(r.direction());
		double t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
#endif
	}
}

#define NX 500
#define NY 500
#define NS 1000
unsigned char img[NX * NY * 3];

int main() {
	unsigned char* p = img;

#if 0
	hitable* world = random_secne();
#else
	hitable* world = cornellsence();
#endif

	vec3 lookfrom(278,278 ,-800);
	vec3 lookat(278, 278, 0);
#if 0
	double disk_to_focus = (lookfrom - lookat).length();
#else
	double disk_to_focus = 10.0;
#endif
	double aperture = 0.0;
	double vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, double(NX) / double(NY), aperture, disk_to_focus, 0.0, 1.0);
	ray r;
#pragma omp parallel for schedule(dynamic, 1) private(r)
	for (int j = NY - 1; j >= 0; --j) {
		for (int i = 0; i < NX; ++i) {
			vec3 colum(0, 0, 0);
			for (int s = 0; s < NS; ++s) {											//s为取样次数
				double u = double(i + drand48()) / double(NX);
				double v = double(j + drand48()) / double(NY);
				r = cam.get_ray(u, v);
				vec3 p = r.point_at_t(2.0);
				colum += color(r, world,0);											//初始深度为0
			}
			colum /= double(NS);
			colum = vec3(sqrt(colum[0]), sqrt(colum[1]), sqrt(colum[2]));
			
#if 0
			*p++ = int(255.99 * colum[0]);
			*p++ = int(255.99 * colum[1]);
			*p++ = int(255.99 * colum[2]);
#else
			p[(NY - j-1) * (NX * 3) + 3 * i] = int(255.99 * colum[0]);
			p[(NY - j-1) * (NX * 3) + 3 * i + 1] = int(255.99 * colum[1]);
			p[(NY - j-1) * (NX * 3) + 3 * i + 2] = int(255.99 * colum[2]);
#endif
		}
		std::cout << j<< "\n";
	}
	p = &img[NX * NY * 3];
#if 0
	toybmp(fopen("openmptest10pdf9.bmp", "wb"), NX, NY, p, 0);
#else
	svpng(fopen("InTheRestOfYourLife_sampleLightDirectly15.png", "wb"), NX, NY, img, 0);
#endif
} 