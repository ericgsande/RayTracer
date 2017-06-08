#ifndef FACE_H
#define FACE_H

#include "Vector3.h"

class Face {

	private:
		Vector3 v1 ;
		Vector3 v2 ;
		Vector3 v3 ;
		int material ;
		
	public:
		Face() ;
		Face(Vector3 v1_, Vector3 v2_, Vector3 v3_, int m_) ;
		
		Vector3 getV1() { return v1 ; } ;
		Vector3 getV2() { return v2 ; } ;
		Vector3 getV3() { return v3 ; } ;
		int getMaterial() { return material ; } ;
		
		void setV1(Vector3 v1_) { v1 = v1_ ; } ;
		void setV2(Vector3 v2_) { v2 = v2_ ; } ;
		void setV3(Vector3 v3_) { v3 = v3_ ; } ;
		void setMaterial(int m_) { material = m_ ; } ;
		
		//void print() ;
		
} ;
#endif
