#ifndef RAY_H
#define RAY_H

#include "Vector3.h"

class Ray {
	
	private:
		Vector3 origin ;
		Vector3 direction ;
		
	public:
		Ray() ;
		Ray(Vector3 o_, Vector3 d_) ;
		
		Vector3 getOrigin() { return origin ; } ;
		Vector3 getDirection() { return direction ; } ;
		float getX() { return origin.getX() ; } ;
		float getY() { return origin.getY() ; } ;
		float getZ() { return origin.getZ() ; } ;
		float getDX() { return direction.getX() ; } ;
		float getDY() { return direction.getY() ; } ;
		float getDZ() { return direction.getZ() ; } ;
		
		void setOrigin(Vector3 o_) { origin = o_ ; } ;
		void setDirection(Vector3 d_) { direction = d_ ; } ;
		
		void print() ;
	
} ;
#endif
