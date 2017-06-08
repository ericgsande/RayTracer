#include "Face.h"
#include "Vector3.h"

Face::Face() {
	v1 = Vector3() ;
	v2 = Vector3() ;
	v3 = Vector3() ;
	material = 0 ;
}

Face::Face(Vector3 v1_, Vector3 v2_, Vector3 v3_, int m_) {
	v1 = v1_ ;
	v2 = v2_ ;
	v3 = v3_ ;
	material = m_ ;
}
