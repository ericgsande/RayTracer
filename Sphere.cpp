#include <iostream>
#include "Vector3.h"
#include "Sphere.h" 

Sphere::Sphere() {
	center = Vector3() ;
	radius = 0.0 ;
	material = 0 ;
}

Sphere::Sphere(Vector3 c_, float r_, int m_) {
	center = c_ ;
	radius = r_ ;
	material = m_ ;
}

void Sphere::print() {
	std::cout << "(" << getCenterX() << "," << getCenterY() << "," << getCenterZ() << ") //r: " << radius << " //m: " << material << std::endl ;
}
