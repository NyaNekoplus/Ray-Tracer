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
	sphere(vec3 cen, double r,material *material_p) :center(cen), radius(r),mtr(material_p) {};				//��ʼ��Բ�ĺͰ뾶
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
		if (temp > t_min && temp < t_max) {				//temp�ǹ��߳��ȵı���
			rec.t = temp;								//�˴���¼������hitrecord��ע��recΪ����
			rec.p = r.point_at_t(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);			//rectangle TO sphere !!!!!˳�򣡣�����
			rec.n = (rec.p - center) / radius;			//����:����������Ľ��㵽Բ�ĵľ��� / �뾶
			rec.mat_ptr = mtr;
			return true;
		}
		temp = (-b + sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp�ǹ��߳��ȵı���
			rec.t = temp;
			rec.p = r.point_at_t(rec.t);
			get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);			//rectangle TO sphere !!!!!˳�򣡣�����
			rec.n = (rec.p - center) / radius;			//����:����������Ľ��㵽Բ�ĵľ��� / �뾶
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
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);//�������
}
bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	double a = dot(r.direction(), r.direction());
	double b = dot(r.direction(), oc);
	double c = dot(oc, oc) - radius * radius;
	double delta = b * b - a * c;
	if (delta > 0) {
		double temp = (-b - sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp�ǹ��߳��ȵı���
			rec.t = temp;								//�˴���¼������hitrecord��ע��recΪ����
			rec.p = r.point_at_t(rec.t);
			rec.n = (rec.p - center(r.time())) / radius;			//����:����������Ľ��㵽Բ�ĵľ��� / �뾶
			rec.mat_ptr = mtr;
			return true;
		}
		temp = (-b + sqrt(delta)) / a;
		if (temp > t_min && temp < t_max) {				//temp�ǹ��߳��ȵı���
			rec.t = temp;
			rec.p = r.point_at_t(rec.t);
			rec.n = (rec.p - center(r.time())) / radius;			//����:����������Ľ��㵽Բ�ĵľ��� / �뾶
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
/*ʹ��const�ؼ��ֽ���˵���ĳ�Ա��������Ϊ����Ա������
ֻ�г���Ա���������ʸ���������򳣶���û��ʹ��const�ؼ���˵���ĳ�Ա����������������������*/

/* const����ֻ�ܵ���const��Ա������  
     const�����ֵ���ܱ��޸ģ���const��Ա�������޸�const�������ݳ�Ա��ֵ���﷨����  
     ��const�����е��÷�const��Ա�������﷨����  

���ǰ�������������Ϊconst����˼�ǡ��������ڲ��ܶԳ�Ա�������κθĶ�����
���������������һ��constʵ������ô����ֻ�ܵ�����const���εĺ�����*/