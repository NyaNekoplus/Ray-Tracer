#ifndef SPHERE_H_
#define SPHERE_H_

#include "hitable.h"

void get_sphere_uv(const vec3& p, double& u, double& v) {
	double phi = atan2(p.z(), p.x());
	double theta = asin(p.y());
	u = 1 - (phi + PI) / (2 * PI);
	v = (theta + PI /2) / PI;
}

class sphere :public hitable {
public:
	sphere(){}
	sphere(vec3 cen, double r,material *material_p) :center(cen), radius(r),mtr(material_p) {};				//初始化圆心和半径
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& box) const;
	vec3 center;
	double radius;
	material* mtr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center;
	double a = dot(r.direction(), r.direction());
	double b = dot(r.direction(), oc);
	double c = dot(oc, oc) - radius * radius;
	double delta = b * b - a * c;
	if (delta > 0) {
		double temp = (-b - sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp是光线长度的比例
			rec.t = temp;								//此处记录参数到hitrecord，注意rec为引用
			rec.p = r.point_at_t(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);			//rectangle TO sphere !!!!!顺序！！！！
			rec.n = (rec.p - center) / radius;			//求法线:光线与球面的交点到圆心的距离 / 半径
			rec.mat_ptr = mtr;
			return true;
		}
		temp = (-b + sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp是光线长度的比例
			rec.t = temp;
			rec.p = r.point_at_t(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);			//rectangle TO sphere !!!!!顺序！！！！
			rec.n = (rec.p - center) / radius;			//求法线:光线与球面的交点到圆心的距离 / 半径
			rec.mat_ptr = mtr;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(double t0, double t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

class moving_sphere :public hitable {
public:
	moving_sphere(){}
	moving_sphere(vec3 cen0, vec3 cen1, double t0, double t1, double r, material* m) :
		center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mtr(m) {};

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
	virtual bool bounding_box(double t0, double t1, aabb& box) const;
	vec3 center(double time)const;
	
	vec3 center0, center1;
	double time0, time1;
	double radius;
	material* mtr;
};
vec3 moving_sphere::center(double time)const {
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);//随机中心
}
bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	double a = dot(r.direction(), r.direction());
	double b = dot(r.direction(), oc);
	double c = dot(oc, oc) - radius * radius;
	double delta = b * b - a * c;
	if (delta > 0) {
		double temp = (-b - sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp是光线长度的比例
			rec.t = temp;								//此处记录参数到hitrecord，注意rec为引用
			rec.p = r.point_at_t(rec.t);
			rec.n = (rec.p - center(r.time())) / radius;			//求法线:光线与球面的交点到圆心的距离 / 半径
			rec.mat_ptr = mtr;
			return true;
		}
		temp = (-b + sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp是光线长度的比例
			rec.t = temp;
			rec.p = r.point_at_t(rec.t);
			rec.n = (rec.p - center(r.time())) / radius;			//求法线:光线与球面的交点到圆心的距离 / 半径
			rec.mat_ptr = mtr;
			return true;
		}
	}
	return false;
}

bool moving_sphere::bounding_box(double t0, double t1, aabb& box) const {
	aabb box_t0, box_t1;
	box_t0 = aabb(center(t0) - vec3(radius, radius, radius), center(t0) + vec3(radius, radius, radius));
	box_t1 = aabb(center(t1) - vec3(radius, radius, radius), center(t1) + vec3(radius, radius, radius));
	box = surrounding_box(box_t0, box_t1);
	return true;
}

#endif
/*使用const关键字进行说明的成员函数，称为常成员函数。
只有常成员函数才有资格操作常量或常对象，没有使用const关键字说明的成员函数不能用来操作常对象*/

/* const对象只能调用const成员函数。  
     const对象的值不能被修改，在const成员函数中修改const对象数据成员的值是语法错误  
     在const函数中调用非const成员函数是语法错误  

这是把整个函数修饰为const，意思是“函数体内不能对成员数据做任何改动”。
如果你声明这个类的一个const实例，那么它就只能调用有const修饰的函数。*/