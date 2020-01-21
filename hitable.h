#ifndef HITABLE_H_
#define HITABLE_H_

#include "ray.h"
#include "aabb.h"

class material;

struct hit_record {
	double t;							//在限定范围内的光线长度
	vec3 p;								//point
	vec3 n;								//normal
	double u;
	double v;
	material* mat_ptr;
};


class hitable {
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const = 0;		//纯虚函数，抽象类
	virtual bool bounding_box(double t0, double t1, aabb& box)const = 0;	//time0,time1
	virtual double  pdf_value(const vec3& o, const vec3& v) const { return 0.0; }
	virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0); }
};

class translate :public hitable {
public:
	translate(hitable* p,const vec3& displacement):ptr(p),offset(displacement){}
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
		ray moved_r(r.origin() - offset, r.direction(), r.time());
		if (ptr->hit(moved_r, t_min, t_max, rec)) {
			rec.p += offset;
			return true;
		}
		else
			return false;
	}
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		if (ptr->bounding_box(t0, t1, box)) {
			box = aabb(box.min() + offset, box.max() + offset);
			return true;
		}
		else
			return false;
	}
	hitable* ptr;
	vec3 offset;
};

class rotate_y :public hitable {
public:
	rotate_y(hitable* p, double angle);
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		box = bbox; return hasbox;
	}
	bool hasbox;
	hitable* ptr;
	aabb bbox;
	double sin_theta;
	double cos_theta;
};
rotate_y::rotate_y(hitable* p, double angle):ptr(p) {
	double radians = PI / 180 * angle;
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
	vec3 max(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	hasbox = ptr->bounding_box(0, 1, bbox);
	for(int i=0;i<2;++i)
		for(int j=0;j<2;++j)
			for (int k = 0; k < 2; ++k) {
				double x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				double y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				double z = k * bbox.max().z() + (1 - k) * bbox.min().z();
				double newx = cos_theta * x + sin_theta * z;
				double newz = -sin_theta * x + cos_theta * z;
				vec3 tester(newx, y, newz);
				for (int c = 0; c < 3; ++c) {
					if (tester[c] < min[c])
						min[c] = tester[c];
					if (tester[c] > max[c])
						max[c] = tester[c];
				}
			}
	bbox = aabb(min, max);
}
bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	vec3 origin = r.origin();
	vec3 direction = r.direction();
	origin[0] = r.origin()[0] * cos_theta - sin_theta * r.origin()[2];			//转换光线起点和方向的x,z坐标
	origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];
	direction[0] = r.direction()[0] * cos_theta - sin_theta * r.direction()[2];
	direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];
	ray moved_r(origin, direction,r.time());
	if (ptr->hit(moved_r, t_min, t_max, rec)) {
		vec3 point = rec.p;
		vec3 normal = rec.n;
		point[0] = rec.p[0] * cos_theta + sin_theta * rec.p[2];			//转换击中点和法线
		point[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];
		normal[0] = rec.n[0] * cos_theta + sin_theta * rec.n[2];
		normal[2] = -sin_theta * rec.n[0] + cos_theta * rec.n[2];
		rec.p = point;
		rec.n = normal;
		return true;
	}
	else
		return false;
}

class flip_normals :public hitable {
public:
	flip_normals(hitable* p):ptr(p){}
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
		if (ptr->hit(r, t_min, t_max, rec)) {
			rec.n = -rec.n;
			return true;
		}
		else
			return false;
	}
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		return ptr->bounding_box(t0, t1, box);
	}
	hitable* ptr;
};

class xy_rect :public hitable {
public:
	xy_rect() {}
	xy_rect(double _x1, double _x2, double _y1, double _y2, double _k, material* mtr) :
		x1(_x1), x2(_x2), y1(_y1), y2(_y2), k(_k), mp(mtr) {};
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		box = aabb(vec3(x1, y1, k - 0.0001), vec3(x2, y2, k + 0.0001));
		return true;
	}
	material* mp;
	double x1, x2, y1, y2, k;
};

bool xy_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	double t = (k - r.origin().z()) / r.direction().z();
	if (t<t_min || t>t_max)
		return false;
	double x = r.origin().x() + t * r.direction().x();
	double y = r.origin().y() + t * r.direction().y();
	if (x<x1 || x>x2 || y<y1 || y>y2)
		return false;
	rec.u = (x - x1) / (x2 - x1);//uv coordinate(缩放
	rec.v = (y - y1) / (y2 - y1);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_t(t);
	rec.n = vec3(0, 0, 1);
	return true;
}

class xz_rect :public hitable {
public:
	xz_rect() {}
	xz_rect(double _x1, double _x2, double _z1, double _z2, double _k, material* mtr) :
		x1(_x1), x2(_x2), z1(_z1), z2(_z2), k(_k), mp(mtr) {};
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		box = aabb(vec3(x1, k - 0.0001,z1 ), vec3(x2, k + 0.0001,z2 ));
		return true;
	}
	virtual double  pdf_value(const vec3& o, const vec3& v) const {
		hit_record rec;
		if (this->hit(ray(o, v), 0.001, DBL_MAX, rec)) {
			double area = (x2 - x1) * (z2 - z1);
			double distance_squared = rec.t * rec.t * v.squared_length();
			double cosine = fabs(dot(v, rec.n) / v.length());
			return  distance_squared / (cosine * area);
		}
		else
			return 0;
	}
	virtual vec3 random(const vec3& o) const {
		vec3 random_point = vec3(x1 + drand48() * (x2 - x1), k,
			z1 + drand48() * (z2 - z1));
		return random_point - o;
	}
	material* mp;
	double x1, x2, z1, z2, k;
};

bool xz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	double t = (k - r.origin().y()) / r.direction().y();
	if (t<t_min || t>t_max)
		return false;
	double x = r.origin().x() + t * r.direction().x();
	double z = r.origin().z() + t * r.direction().z();
	if (x<x1 || x>x2 || z<z1 || z>z2)
		return false;
	rec.u = (x - x1) / (x2 - x1);
	rec.v = (z - z1) / (z2 - z1);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_t(t);
	rec.n = vec3(0, 1, 0);
	return true;
}

class yz_rect :public hitable {
public:
	yz_rect() {}
	yz_rect(double _y1, double _y2, double _z1, double _z2, double _k, material* mtr) :
		y1(_y1), y2(_y2), z1(_z1), z2(_z2), k(_k), mp(mtr) {};
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;
	virtual bool bounding_box(double t0, double t1, aabb& box)const {
		box = aabb(vec3(k - 0.0001, y1,z1 ), vec3(k + 0.0001, y2,z2 ));
		return true;
	}
	material* mp;
	double y1, y2, z1, z2, k;
};

bool yz_rect::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	double t = (k - r.origin().x()) / r.direction().x();
	if (t<t_min || t>t_max)
		return false;
	double z = r.origin().z() + t * r.direction().z();
	double y = r.origin().y() + t * r.direction().y();
	if (z<z1 || z>z2 || y<y1 || y>y2)
		return false;
	rec.u = (z - z1) / (z2 - z1);
	rec.v = (y - y1) / (y2 - y1);
	rec.t = t;
	rec.mat_ptr = mp;
	rec.p = r.point_at_t(t);
	rec.n = vec3(1, 0, 0);
	return true;
}



#endif