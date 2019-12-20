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
	rec.u = (x - x1) / (x2 - x1);
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

class box :public hitable {
public:
	box() {}
	box(const vec3& p0, const vec3& p1, material* ptr);
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec)const;		//纯虚函数，抽象类
	virtual bool bounding_box(double t0, double t1, aabb& box)const {	//time0,time1
		box = aabb(pmin, pmax);
		return true;
	}
	vec3 pmin, pmax;
	hitable* list_ptr;
};

box::box(const vec3& p0, const vec3& p1, material* ptr) {
	pmin = p0;
	pmax = p1;
	hitable** list = new hitable * [6];
	list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
	list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
	list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
	list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	return list_ptr->hit(r, t_min, t_max, rec);
}

#endif