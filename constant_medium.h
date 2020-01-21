#ifndef CONSTANT_MEDIUM
#define CONSTANT_MEDIUM

#include "material.h"

class constant_medium :public hitable {
public:
	constant_medium(hitable* b, double d, texture* a) :boundary(b), density(d)
	{
		phase_function = new isotropic(a);
	}
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		return boundary->bounding_box(t0, t1, box);
	}
	hitable* boundary;
	double density;
	material* phase_function;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	hit_record rec1, rec2;
	if (boundary->hit(r, -DBL_MAX, DBL_MAX, rec1)) {
		//-inf ~inf 可能会在物体内部
		if (boundary->hit(r, rec1.t + 0.0001, DBL_MAX, rec2)) {//确保rec2在rec1之后
			if (rec1.t < t_min)		//确保在范围内
				rec1.t = t_min;
			if (rec2.t > t_max)
				rec2.t = t_max;
			if (rec1.t > rec2.t)	//不能发生
				return false;
			double distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
			double hit_distance = -(1 / density) * log(drand48());
			//求解散射距离
			//http://psgraphics.blogspot.com/2013/11/scattering-in-constant-medium.html
			if (hit_distance < distance_inside_boundary) {
				rec.t = rec1.t + hit_distance / r.direction().length();
				rec.p = r.point_at_t(rec.t);
				rec.n = vec3(1, 0, 0);	//arbitrary
				rec.mat_ptr = phase_function;
				return true;
			}
		}
	}
	return false;
}
#endif