#ifndef CAMERA_H_
#define CAMERA_H_

#include "ray.h"

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0 * vec3(randow1, randow1, 0) - vec3(1, 1, 0);
	} while (dot(p, p) >= 1.0);		//x^2 + y^2 = r^2
	return p;
}
class camera {
public:
	camera(vec3 lookfrom,vec3 lookat,vec3 viewup,double vertical_fov, double aspect,double aperture,double focus_disk,
		double t0,double t1) {
		time0 = t0;
		time1 = t1;
		lens_radius = aperture /2;//透镜直径
		double theta = vertical_fov * PI / 180;//vertical_fov是视野的广度，1度=pi/180弧度
		double half_height = tan(theta / 2);
		double half_width = aspect * half_height;//宽与高的比例乘高
		origin = lookfrom;			//vec3 (0.0, 0.0, 0.0);
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(viewup, w));
		v = cross(w, u);
		lower_left_corner = origin - half_width * focus_disk*u - half_height * focus_disk*v - focus_disk*w;
		horizontal = 2 * half_width * focus_disk * u;
		vertical = 2 * half_height * focus_disk * v;
		//horizontal=vec3 (2.0*half_width, 0.0, 0.0);//3.5
		//vertical=vec3 (0.0, 2.0*half_height, 0.0);	//2.0
		//lower_left_corner=vec3 (-half_width, -half_height, -1.0);//-1.75,-1.0,-1.0
	}
	ray get_ray(double s, double t) {
		vec3 rd = lens_radius * random_in_unit_disk();//半径乘单位圆盘上的随机点
		vec3 offset = u * rd.x() + v * rd.y();//将点转换至uv坐标系
		double time = time0 + randow1 * (time1 - time0);
		return ray(origin + offset, s * horizontal + t * vertical + lower_left_corner - origin - offset,time);
	}

	vec3 origin;
	vec3 horizontal;
	vec3 vertical;
	vec3 lower_left_corner;
	vec3 u, v, w;
	double lens_radius;		//透镜半径
	double time0, time1;
};
#endif