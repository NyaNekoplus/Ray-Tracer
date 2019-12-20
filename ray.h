#ifndef RAY_H_
#define RAY_H_

#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const vec3& v1, const vec3& v2, double ti = 0.0) { A = v1, B = v2; _time = ti; }

	vec3 origin()const { return A; }
	vec3 direction()const { return B; }
	double time()const { return _time; }
	vec3 point_at_t(const double t)const { return A + t * B; }
	vec3 A, B;

	double _time;
};

#endif