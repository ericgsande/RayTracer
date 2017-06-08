#include <iostream>
#include <cmath>
#include "Vector3.h"

Vector3::Vector3() {
	x = 0 ;
	y = 0 ;
	z = 0 ;
}

Vector3::Vector3(float x_, float y_, float z_) {
	x = x_ ;
	y = y_ ;
	z = z_ ;
}

float Vector3::getLength() {
	return sqrt(x * x + y * y + z * z) ;
}

Vector3 operator +(const Vector3 &v1, const Vector3 &v2) {
	Vector3 v(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z) ;
	return v ;
}

Vector3 operator -(const Vector3 &v1, const Vector3 &v2) {
	Vector3 v(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z) ;
	return v ;
}

Vector3 operator *(const float &s, const Vector3 &v) {
	Vector3 r(s * v.x, s * v.y, s * v.z) ;
	return r ;
}

Vector3 operator /(const Vector3 &v, const float &s) {
	Vector3 r(v.x / s, v.y / s, v.z / s) ;
	return r ;
}

void Vector3::print() {
	std::cout << "(" << x << "," << y << "," << z << ")" << std::endl ;
}
