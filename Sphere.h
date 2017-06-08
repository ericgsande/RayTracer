#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3.h"

class Sphere {

	private:
		Vector3 center ;
		float radius ;
		int material ;
		
	public:
		Sphere() ;
		Sphere(Vector3 c_, float r_, int m_) ;
		
		Vector3 getCenter() { return center ; } ;
		float getRadius() { return radius ; } ;
		int getMaterial() { return material ; } ;
		float getCenterX() { return center.getX() ; } ;
		float getCenterY() { return center.getY() ; } ;
		float getCenterZ() { return center.getZ() ; } ;
		
		void setCenter(Vector3 c_) { center = c_ ; } ;
		void setRadius(float r_) { radius = r_ ; } ;
		void setMaterial(int m_) { material = m_ ; } ;
		
		void print() ;
		
} ;
#endif
