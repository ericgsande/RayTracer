#ifndef COLOR_H
#define COLOR_H

class Color {
	
	private:
		float r ;
		float g ;
		float b ;
		
	public:
		Color() ;
		Color(float r_, float g_, float b_) ;
		
		float getR() { return r ; } ;
		float getG() { return g ; } ;
		float getB() { return b ; } ;
		
		void setR(float r_) { r = r_ ; } ;
		void setG(float g_) { g = g_ ; } ;
		void setB(float b_) { b = b_ ; } ;
		
		void print() ;
		
} ;
#endif
