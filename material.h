#ifndef MATERIAL_H_
#define MATERIAL_H_
struct hit_record;
#include "ray.h"
#include "hitable.h"
#include "texture.h"


vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

bool refract(const vec3& v, const vec3& n, double ni_over_nt, vec3& refracted) {
	vec3 uv = unit_vector(v);
	double dt = dot(uv, n);
	double discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0) {
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else return false;
}

double schlick(double cos, double ref_idx) {
	double r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cos), 5);
}

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(double u, double v, const vec3& p)const {
		return vec3(0, 0, 0);
	}
};

class dielectric :public material {
public:
	dielectric(double refractive_index) :ref_idx(refractive_index){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.n);
		double ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		double reflect_prob;					//�������
		double cosine;
		if (dot(r_in.direction(), rec.n) > 0) {	//����ǲ�Ϊ90��
			outward_normal = -rec.n;			//���߷�����ת
			ni_over_nt = ref_idx;				//��ǰ����ϵ����Ϊ�����ڲ���ϵ��
			cosine = ref_idx * dot(r_in.direction(), rec.n) / r_in.direction().length();//�����ڲ���*cos(�����)
		}
		else {											//�����Ϊ�۽ǻ�ֱ��
			outward_normal = rec.n;						//����������
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.n) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {	//�������
			reflect_prob = schlick(cosine, ref_idx);							//����������ȣ����������Ĺ�ǿ����
		}
		else {
			reflect_prob = 1.0;
		}
		if (randow1 < reflect_prob) {											//�����С�ڷ����������
			scattered = ray(rec.p, reflected, r_in.time());									//ɢ��������Ϊ��������
		}
		else {																	//��������ڵ��ڷ����������
			scattered = ray(rec.p, refracted, r_in.time());									//ɢ��������Ϊ��������
		}
		return true;
	}
	double ref_idx;
};
class lambertian :public material {
public:
	lambertian(texture* a) :albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.n + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p,r_in.time());
		attenuation = albedo->value(rec.u, rec.v, rec.p);
		return true;
	}
	texture* albedo;								//albedo��������ϵ��
};

class metal :public material {
public:
	metal(const vec3& a, double f) :albedo(a) { if (f < 1)  fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.n);
		scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere(), r_in.time());		//ƫ��ģ��ϵ��*�����λԲ�ڵ㣬ϵ��Խ��ƫ������
		attenuation = albedo;
		return (dot(scattered.direction(), rec.n) > 0);
	}
	vec3 albedo;
	double fuzz;
};

class diffuse_light :public material {
public:
	diffuse_light(texture* a):emit(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		return false;
	}
	virtual vec3 emitted(double u, double v, const vec3& p)const {
		return emit->value(u, v, p);
	}
	texture* emit;
};
#endif