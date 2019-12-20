#include <iostream>
#include "sence_function.h"

#define MAXdouble 3.40282e+038
vec3 color(const ray& r, hitable* world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, DBL_MAX, rec)) {
		vec3 attenuation;
		ray scattered;
		vec3 emit = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emit+attenuation * color(scattered, world, depth + 1);
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

#define NX 600
#define NY 600
#define NS 1000
unsigned char img[NX * NY * 3];

int main() {
	unsigned char* p = img;
	//long count = ny;
	
	/*hitable* list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5,new lambertian(vec3(0.1,0.2,0.5)));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2),1.0));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
	hitable* world = new hitable_list(list, 5);*/
#if 0
	hitable* world = random_secne();
#else
	hitable* world = cornellbox();
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
	for (int j = NY - 1; j >= 0; --j) {
		for (int i = 0; i < NX; ++i) {
			vec3 colum(0, 0, 0);
			for (int s = 0; s < NS; ++s) {											//s为取样次数
				double u = double(i + randow1) / double(NX);
				double v = double(j + randow1) / double(NY);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_t(2.0);
				colum += color(r, world,0);											//初始深度为0
			}
			colum /= double(NS);
			colum = vec3(sqrt(colum[0]), sqrt(colum[1]), sqrt(colum[2]));
			
			*p++ = int(255.99 * colum[0]);
			*p++ = int(255.99 * colum[1]);
			*p++ = int(255.99 * colum[2]);
			
		}
		std::cout << j<< "\n";
	}
#if 0
	toybmp(fopen("cornellbox2withbox2.bmp", "wb"), NX, NY, p, 0);
#else
	svpng(fopen("cornellbox2withbox.png", "wb"), NX, NY, img, 0);
#endif
} 