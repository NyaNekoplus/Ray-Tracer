#ifndef AABB_H_
#define AABB_H_

#include "hitable.h"
//inline double ffmin(double a, double b) { return a < b ? a : b; }
//inline double ffmax(double a, double b) { return a > b ? a : b; }

class aabb {
public:
	aabb(){}
	aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

	vec3 min()const { return _min; }
	vec3 max()const { return _max; }

	bool hit(const ray& r, double tmin, double tmax)const {	//interval(padding) tmin/tmax,
		for (int a = 0; a < 3; ++a) {						//prevent NaN,which meaning is giving t0/t1 a meaningful value
			double invD = 1.0 / r.direction()[a];
			double t0 = (min()[a] - r.origin()[a]) * invD;
			double t1 = (max()[a] - r.origin()[a]) * invD;
			if (invD < 0.0) {
				std::swap(t0, t1);
			}
			tmin = t0 < tmin ? tmin : t0;
			tmax = t1 > tmax ? tmax : t1;
			if (tmax <= tmin)
				return false;
		}
		return true;
	}
	vec3 _min, _max;
};

aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(fmin(box0.min().x(), box1.min().x()),
		fmin(box0.min().y(), box1.min().y()),
		fmin(box0.min().z(), box1.min().z()));
	vec3 big(fmax(box0.max().x(), box1.max().x()),
		fmax(box0.max().y(), box1.max().y()),
		fmax(box0.max().z(), box1.max().z()));
	return aabb(small, big);
}

#endif