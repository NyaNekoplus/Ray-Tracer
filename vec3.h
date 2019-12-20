#include <math.h>
#include <stdlib.h>
#include <iostream>

#define random(a,b) (randow1%(b-a+1)+a)
#define randow1 (double((rand()%(100)))/100.0)	//获取[0,1]间的随机数
#define PI 3.14159265358979323846
class vec3 {
public:
	vec3() {}
	vec3(double e0, double e1, double e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
	inline double x() const { return e[0]; }
	inline double y() const { return e[1]; }
	inline double z() const { return e[2]; }
	inline double r() const { return e[0]; }
	inline double g() const { return e[1]; }
	inline double b() const { return e[2]; }

	inline const vec3& operator+()const { return *this; }
	inline vec3 operator-()const { return vec3(-e[0], -e[1], -e[2]); }
	inline double operator[](int i)const { return e[i]; }					//用于得到vec3对象的值，故要加const
	inline double& operator[](int i) { return e[i]; }						//用于修改vec3对象的值，所以后面不加const

	inline vec3& operator+=(const vec3& v2);
	inline vec3& operator-=(const vec3& v2);
	inline vec3& operator*=(const vec3& v2);
	inline vec3& operator/=(const vec3& v2);
	inline vec3& operator*=(const double t);
	inline vec3& operator/=(const double t);

	inline double length()const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}
	inline double squared_length()const {
		return (e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}
	inline void make_unit_vector();

	double e[3];
};

inline std::istream& operator>>(std::istream& is, vec3& t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}
inline std::ostream& operator<<(std::ostream& os, const vec3& t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline void vec3::make_unit_vector() {
	double k = 1.0f * sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
}

inline vec3 operator+(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
inline vec3 operator-(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}
inline vec3 operator*(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}
inline vec3 operator/(const vec3& v1, const vec3& v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}
inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline vec3 operator/(const vec3& v,double t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}
inline vec3 operator*(const vec3& v,double t) {								//参数顺序不同
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}
inline double dot(const vec3& v1, const vec3& v2) {							//点乘
	return (v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2]);
}
inline vec3 cross(const vec3& v1, const vec3& v2) {							//叉乘
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
		(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
		(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}
inline vec3& vec3::operator+=(const vec3& v) {
	e[0] += v.e[0];															//默认在前面加 this
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}
inline vec3& vec3::operator-=(const vec3& v) {
	e[0] -= v.e[0];															
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const vec3& v) {
	e[0] *= v.e[0];															
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}
inline vec3& vec3::operator/=(const vec3& v) {
	e[0] /= v.e[0];															
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}
inline vec3& vec3::operator*=(const double t) {
	e[0] *= t;															
	e[1] *= t;
	e[2] *= t;
	return *this;
}inline vec3& vec3::operator/=(const double t) {
	double k = 1.0f / t;
	e[0] *= k;															
	e[1] *= k;
	e[2] *= k;
	return *this;
}
inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(randow1, randow1, randow1) - vec3(1, 1, 1);	//前一个是随机直径，后一个单位圆圆心
	} while (p.squared_length() >= 1.0);							//当随机点在园内时返回
	return p;
}