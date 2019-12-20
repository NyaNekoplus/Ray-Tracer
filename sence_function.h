#ifndef SENCE_FUNC_H_
#define SENCE_FUNC_H_

#include "sphere.h"
#include "hitable_list.h"
#include "bvh_node.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "svpng.inc"
#include "toybmp.h"
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma warning(disable:4996)


hitable* random_secne() {
	int n = 500;
	texture* checker = new check_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	hitable** list = new hitable * [n + 1];
#if 1
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
#else
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
#endif
	int i = 1;
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			double choose_mat = randow1;
			vec3 center(a + 0.9 * randow1, 0.2, b + 0.9 * randow1);
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					list[i++] = new sphere(center, 0.2, new lambertian(new constant_texture(vec3(randow1 * randow1, randow1 * randow1, randow1 * randow1))));
				}
				else if (choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + randow1), 0.5 * (1 + randow1), 0.5 * (1 + randow1)), 0.5 * randow1));
				}
				else {
					list[i++] = new sphere(center, 0.2, new dielectric(2.4));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.4, 0.2, 0.1), 0.0));
#if 0
	return new hitable_list(list, i);
#else
	return new bvh_node(list, i, 0, 0);
#endif
}
hitable* random_secne_jump() {
	int n = 50000;
	hitable** list = new hitable * [n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
	int i = 1;
	for (int a = -11; a < 11; ++a) {
		for (int b = -11; b < 11; ++b) {
			double choose_mat = randow1;
			vec3 center(a + 0.9 * randow1, 0.2, b + 0.9 * randow1);
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * randow1, 0), 0.0, 1.0, 0.2, new lambertian(new constant_texture(vec3(randow1 * randow1, randow1 * randow1, randow1 * randow1))));
				}
				else if (choose_mat < 0.95) {
					list[i++] = new sphere(center, 0.2, new metal(vec3(0.5 * (1 + randow1), 0.5 * (1 + randow1), 0.5 * (1 + randow1)), 0.5 * randow1));
				}
				else {
					list[i++] = new sphere(center, 0.2, new dielectric(2.4));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.4, 0.2, 0.1), 0.0));
	return new hitable_list(list, i);
}
hitable* perlin_noise() {
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("sagawa2.jpg", &nx, &ny, &nn, 0);
	material* mat = new lambertian(new image_texture(tex_data, nx, ny));
	texture* pertext = new noise_texture(0.7);
	texture* checker = new check_texture(new constant_texture(vec3(0.999, 0.275, 0.41)), new constant_texture(vec3(0.52, 0.73, 0.66)));
	hitable** list = new hitable * [4];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	list[1] = new sphere(vec3(0, 1, -2), 1.0, mat);
	list[2] = new sphere(vec3(-2, 1, -1), 1.0, new metal(vec3(0.8, 0.8, 0.9), 0.0));
	list[3] = new sphere(vec3(2, 2, -5), 2.0, new lambertian(pertext));
	return new bvh_node(list, 4, 0, 0);
}
hitable* texture_test() {
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("sagawa2.jpg", &nx, &ny, &nn, 0);
	material* mat = new lambertian(new image_texture(tex_data, nx, ny));

	texture* blutext = new noise_texture(4, vec3(1, 0.22, 0));
	texture* pertext = new noise_texture(4);
	hitable** list = new hitable * [4];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(blutext));
	list[1] = new sphere(vec3(0, 2, 0), 2.0, new lambertian(pertext));
	list[2] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new sphere(vec3(0, 540, -500), 1080, mat);
	return new hitable_list(list, 4);
}

hitable* simple_light() {
	int nx, ny, nn;
	unsigned char* tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material* mat = new lambertian(new image_texture(tex_data, nx, ny));
	return new sphere(vec3(0, 0, 0), 2, mat);
}

hitable* cornellbox() {
	hitable** list = new hitable * [6];
	int i = 0;
	material* red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material* white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material* green = new lambertian(new constant_texture(vec3(0.05, 0.65, 0.05)));
	material* light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new box(vec3(130, 0, 65), vec3(295, 165, 230), white);
	list[i++] = new box(vec3(265, 0, 295), vec3(430, 330, 460), white);
	return new hitable_list(list, i);
}

#endif // !SENCE_FUNC_H_