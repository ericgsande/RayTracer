#include "Vector3.h"
#include "Ray.h"

Ray::Ray() {
	origin = Vector3() ;
	direction = Vector3() ;
}

Ray::Ray(Vector3 o_, Vector3 d_) {
	origin = o_ ;
	direction = d_ ;
}

void Ray::print() {
	origin.print() ;
	direction.print() ;
}
