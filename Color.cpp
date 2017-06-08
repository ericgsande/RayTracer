#include <iostream>
#include "Color.h"

Color::Color() {
	r = 0 ;
	g = 0 ;
	b = 0 ;
}

Color::Color(float r_, float g_, float b_) {
	r = r_ ;
	g = g_ ;
	b = b_ ;
}

void Color::print() {
	std::cout << "(" << r << "," << g << "," << b << ")" << std::endl ;
}
