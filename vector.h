#include <cmath>

#ifndef __RW_VECTOR_H__
#define __RW_VECTOR_H__

template <class T>
struct vec3d_generic {
	T x;
	T y;
	T z;

	vec3d_generic() : x(0), y(0), z(0) {}
	vec3d_generic(T _x, T _y) : x(_x), y(_y), z(1) {}
	vec3d_generic(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	vec3d_generic(const vec3d_generic& v) : x(v.x), y(v.y), z(v.z) {}

	T mag() {
		return sqrt(x*x + y*y + z*z);
	}

	vec3d_generic norm() {
		T r = 1 / mag();
		return vec3d_generic(x * r, y * r, z * r);
	}

	vec3d_generic operator + (const vec3d_generic& rhs) {
		return vec3d_generic(this->x + rhs.x, this->y + rhs.y , this->z + rhs.z);
	}

	vec3d_generic operator - (const vec3d_generic& rhs) {
		return vec3d_generic(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}

	vec3d_generic operator * (const T& rhs) {
		return vec3d_generic(this->x * rhs, this->y * rhs, this->z * rhs);
	}

	vec3d_generic& operator += (const vec3d_generic& rhs) {
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	vec3d_generic& operator -= (const vec3d_generic& rhs) {
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	vec3d_generic& operator *= (const T& scaler) {
		this->x *= scaler;
		this->y *= scaler;
		this->z *= scaler;
		return *this;
	}

	T dotunits(const vec3d_generic& rhs) {
		return (this->x * rhs.x + this->y * rhs.y + this->z * rhs.z);
	}

	T dot(const vec3d_generic& rhs) {

		T m2 = sqrt(rhs.x * rhs.x + rhs.y * rhs.y + rhs.z * rhs.z);

		return (this->x * rhs.x + this->y * rhs.y + this->z * rhs.z)/(this->mag() * m2);
	}

	vec3d_generic cross(const vec3d_generic& rhs) {
		return vec3d_generic(
			this->y * rhs.z - this->z * rhs.y,
			this->z * rhs.x - this->x * rhs.z,
			this->x * rhs.y - this->y * rhs.x
			);
	}


	vec3d_generic rotateZ(const T& radians) {
		T cs = cos(radians);
		T sn = sin(radians);

		return vec3d_generic(x,y*cs - z*sn, y*sn + z *cs);
	}

	vec3d_generic rotateY(const T& radians) {
		T cs = cos(radians);
		T sn = sin(radians);

		return vec3d_generic(x * cs + z * sn, y, -x * sn + z * cs);
	}

	vec3d_generic project(const T& d) {
		T _x = x * d / z;
		T _y = y * d / z;
		T _z = d;

		return vec3d_generic(_x, _y, _z);
	}

};

typedef vec3d_generic<float> vecf3d;
typedef vec3d_generic<double> vecd3d;

#endif