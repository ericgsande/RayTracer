/* Eric Sande, CSCI 5607 Assigment 1D
 * 
 * starting over from scratch to build a high-quality ray-tracer
 * 
 * TO COMPILE THIS PROGRAM USING CMAKE:
 * 	cmake ..
 * 	make
 * 	./main 'scene_file.txt'
 * 
 * PLEASE READ!
 * NOTES:
 * 	- i worked pretty hard to improve my crappy program from 1c, and used
 * 	  all 7 of my grace days, but there are still some areas where it
 * 	  falls short (i am pretty happy with the improvement i made however)
 * 	- program segfaults on image sizes at about 550x550
 * 		- this is being handled in the parsing which will clamp each one
 * 		  to 512
 * 	- triangle/shadows/texturing/triangle lighting were still giving me
 * 	  trouble, and thus were left out
 * 	- transparency is weird and doesn't function 100%
 * 		- for opacity > 0.7, it looks pretty nice and you can see objects
 * 		  behind it clearly
 * 	- when creating my scene files for submission i forgot how to nicely
 * 	  make them, so i exported them as .png files which messed up coloring
 * 	  slightly
 */
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <cmath>
#include "Vector3.h"
#include "Sphere.h"
#include "Ray.h"
#include "Color.h"
#include "Face.h"

using namespace std ;

const float pi = 4 * atan(1.0) ;

typedef struct {
	Color oD ;
	Color oS ;
	float ka, kd, ks ;
	float n ;
	float op ;
	float rIn ;
} MtlType ;

typedef struct {
	float x, y, z, w ;
	Color color ;
} LightType ;

typedef struct {
	int height ;
	int width ;
	int colorRange ;
	Vector3 eye ;
	Vector3 viewDir ;
	Vector3 upDir ;
	int fovv ;
	Color bkgColor ;
	vector <MtlType> materials ;
	vector <Sphere> spheres ;
	vector <LightType> lights ;
	vector <Vector3> verts ;
	vector <Face> faces ;
} Scene ;

Color traceRay(Ray, Scene, int) ;
Color shadeRay(Ray, Scene, Vector3, int, int, int) ;
void parsingFail(int) ;
Vector3 normalize(Vector3) ;
Vector3 cross(Vector3, Vector3) ;
float dot(Vector3, Vector3) ;
Ray makeRay(Vector3, Vector3) ;
float clamp(float) ;

int main(int argc, char * argv[]) {
	//overall scene object
	Scene scene ;
	scene.colorRange = 255 ;
	
	if (argc != 2) {
		cout << "\nUsage: ./main '.txt file'.\nTry again.\n" << endl ;
		exit(1) ;
	}
	string fileName = argv[1] ;
	
	//parse input file
	ifstream input ;
	input.open(fileName.c_str()) ;	
	if (input.is_open()) {
		int lineCount = 1 ;
		
		//counter to keep track of current material color for spheres
		int mtlCount = 0 ;
		int vertCount = 0 ;
		
		string currentLine = "" ;
		while(getline(input, currentLine)) {
			
			if (currentLine == "" || currentLine == "\0") {
				//do nothing
			}
			else {
				stringstream s(currentLine) ;
				string word = "" ;
				s >> word ;
				
				if (word == "imsize") {
					
					int w, h ;
					s >> w >> h ;
					if (s.fail() || w <= 0 || h <= 0) {
						parsingFail(lineCount) ;
					}
					if (w > 512) {
						w = 512 ;
					}
					if (h > 512) {
						h = 512 ;
					}
					scene.width = w ;
					scene.height = h ;
					
				}
				else if (word == "eye") {
					
					float x, y, z ;
					s >> x >> y >> z ;
					if (s.fail()) {
						parsingFail(lineCount) ;
					}
					
					Vector3 e (x, y, z) ;
					scene.eye = e ;
					
				}
				else if (word == "viewdir") {
					
					float x, y, z ;
					s >> x >> y >> z ;
					if (s.fail()) {
						parsingFail(lineCount) ;
					}
					
					Vector3 v (x, y, z) ;
					scene.viewDir = v ;
					
				}
				else if (word == "updir") {
					
					float x, y, z ;
					s >> x >> y >> z ;
					if (s.fail()) {
						parsingFail(lineCount) ;
					}
					
					Vector3 u (x, y, z) ;
					scene.upDir = u ;
					
				}
				else if (word == "fovv") {
					
					int f ;
					s >> f ;
					if (s.fail() || f < 0 || f > 180) {
						parsingFail(lineCount) ;
					}
					
					scene.fovv = f ;
					
				}
				else if (word == "bkgcolor") {
					
					float r, g, b ;
					s >> r >> g >> b ;
					if (s.fail() || r < 0 || b < 0 || g < 0 || r > 1 || b > 1 || g > 1) {
						parsingFail(lineCount) ;
					}
					
					Color c (r, g, b) ;
					scene.bkgColor = c ;
					
				}
				else if (word == "mtlcolor") {
					
					float oDr, oDb, oDg, oSr, oSb, oSg, ka, kd, ks, n, op, rIn ;
					s >> oDr >> oDg >> oDb >> oSr >> oSg >> oSb >> ka >> kd >> ks >> n >> op >> rIn ;
					if (s.fail() || oDr < 0 || oDb < 0 || oDg < 0 || oDr > 1 || oDb > 1 || oDg > 1
								 || oSr < 0 || oSb < 0 || oSg < 0 || oSr > 1 || oSb > 1 || oSg > 1
								 || ka < 0 || kd < 0 || ks < 0 || n < 0 || op < 0 || op > 1 || rIn < 0) {
						parsingFail(lineCount) ;
					}
					
					MtlType m ;
					m.oD = Color(oDr, oDg, oDb) ;
					m.oS = Color(oSr, oSg, oSb) ;
					m.ka = ka ;
					m.kd = kd ;
					m.ks = ks ;
					m.n = n ;
					m.op = op ;
					m.rIn = rIn ;					
					scene.materials.push_back(m) ;
					mtlCount++ ;
					
				}
				else if (word == "sphere") {
					
					float x, y, z, r ;
					s >> x >> y >> z >> r ;
					if (s.fail() || r <= 0) {
						parsingFail(lineCount) ;
					}
					
					Sphere s = Sphere(Vector3(x, y, z), r, mtlCount - 1) ;
					scene.spheres.push_back(s) ;
					
				}
				else if (word == "light") {
					
					float x, y, z, w, r, b, g ;
					s >> x >> y >> z >> w >> r >> b >> g ;
					if (s.fail() || r < 0 || r > 1 || b < 0 || b > 1 || g < 0 || g > 1) {
						parsingFail(lineCount) ;
					}
					if (w != 1.0) {
						if (w != 0.0) {
							cout << "\n\tLight neither positional nor directional." ;
							parsingFail(lineCount) ;
						}
					}
					
					LightType l ;
					l.x = x ;
					l.y = y ;
					l.z = z ;
					l.w = w ;
					l.color = Color(r, g, b) ;
					scene.lights.push_back(l) ;
					
				}
				else if (word == "v") {
					
					float x, y, z ;
					s >> x >> y >> z ;
					if (s.fail()) {
						parsingFail(lineCount) ;
					}
					Vector3 v = Vector3(x, y, z) ;
					scene.verts.push_back(v) ;
					vertCount++ ;
					
				}
				else if (word == "f") {
					
					int v1, v2, v3 ;
					s >> v1 >> v2 >> v3 ;
					if (s.fail() || vertCount == 0 || v1 > vertCount || v2 > vertCount || v3 > vertCount) {
						parsingFail(lineCount) ;
					}
					Face f = Face(scene.verts[v1 - 1], scene.verts[v2 - 1], scene.verts[v3 - 1], mtlCount - 1) ;
					scene.faces.push_back(f) ;					
					
				}
				else if (word == "#") {
					//do nothing
				}
				else {
					parsingFail(lineCount) ;
				}
			}
			lineCount++ ;
		}
	}
	else {
		cout << "\nError: Unable to open input file. Perhaps it is not a text file.\nExiting...\n" << endl ;
		exit(1) ;
	}
	input.close() ;
	
	//various stuff needed to define 4 corners of the viewing window
	Vector3 u = normalize(cross(scene.viewDir, scene.upDir)) ;
	Vector3 v = normalize(cross(u, scene.viewDir)) ;
	
	float aspectRatio = scene.width / scene.height ;
	int d = 5 ;
	Vector3 n = normalize(scene.viewDir) ;
	Vector3 p = scene.eye + d * n ;
	float degVal = scene.fovv / 2 ;
	float h = 2 * d * tan(degVal * pi / 180) ;
	float w = h * aspectRatio ;
	
	//get 4 corners of viewing window
	Vector3 ul = p + (h / 2) * v - (w / 2) * u ;
	Vector3 ur = p + (h / 2) * v + (w / 2) * u ;
	Vector3 ll = p - (h / 2) * v - (w / 2) * u ;
	Vector3 lr = p - (h / 2) * v + (w / 2) * u ;
	
	//get deltas for defining each point location
	Vector3 deltaH = (ur - ul) / (scene.width - 1) ;
	Vector3 deltaV = (ll - ul) / (scene.height - 1) ;
	
	Vector3 points[scene.height][scene.width] ;
	//get each point location
	for (int i = 0; i < scene.height; i++) {
		for (int j = 0; j < scene.width; j++) {
			points[i][j] = ul + (i * deltaV) + (j * deltaH) ;
			
			//cout << "\npoint " << i << " " << j << endl ;
			//points[i][j].print() ;
		}
	}
	
	//this array will hold the color values for each pixel
	Color colorValues[scene.height][scene.width] ;
	for (int i = 0; i < scene.height; i++) {
		for (int j = 0; j < scene.width; j++) {
			//make current ray and trace it
			Ray ray = makeRay(scene.eye, points[i][j]) ;
			colorValues[i][j] = traceRay(ray, scene, 1) ;
		}
	}
	
	//ouput everything to output file
	fileName.erase(fileName.length() - 4) ;
	fileName.append(".ppm") ;
	ofstream output ;
	output.open(fileName.c_str()) ;
	if (output.is_open()) {
		output << "P3 " << scene.width << " " << scene.height << endl ;
		output << "# " << fileName << ": Eric Sande's CSCI 5607 RayTracer!" << endl ;
		output << scene.colorRange << endl ;
		
		for (int i = 0; i < scene.height; i++) {
			for (int j = 0; j < scene.width; j++) {
				output << colorValues[i][j].getR() * scene.colorRange << " " ;
				output << colorValues[i][j].getG() * scene.colorRange << " " ;
				output << colorValues[i][j].getB() * scene.colorRange << endl ;
			}
		}
	}
	else {
		cout << "\nError: Unable to open output file.\n" << endl ;
		exit(1) ;
	}
	output.close() ;
	
	cout << "\n\tRaycasting successful!\n\tOpen \'" << fileName << "\' to see the result!\n" << endl ;
	return 0 ;
}

Color traceRay(Ray r, Scene s, int depth) { //depth as parameter
	Vector3 inter ;
	int closestObjIndex = -1 ;
	int smallDist = 99999999 ;
	int objFlag = 0 ;
	
	if (depth >= 5) {
		return Color(0, 0, 0) ;
	}
	
	for (int i = 0; i < s.spheres.size(); i++) {
		float a = pow(r.getDX(), 2) + pow(r.getDY(), 2) + pow(r.getDZ(), 2) ;
		float b = 2 * ((r.getDX() * (r.getX() - s.spheres[i].getCenterX()))
				  + (r.getDY() * (r.getY() - s.spheres[i].getCenterY()))
				  + (r.getDZ() * (r.getZ() - s.spheres[i].getCenterZ()))) ;	
		float c = pow(r.getX() - s.spheres[i].getCenterX(), 2) + pow(r.getY() - s.spheres[i].getCenterY(), 2)
			  + pow(r.getZ() - s.spheres[i].getCenterZ(), 2) - pow(s.spheres[i].getRadius(), 2) ;
		float d = pow(b, 2) - (4 * a * c) ;
		
		if (d > 0) {
			float t1 = (-b - sqrt(d)) / (2 * a) ;
			float t2 = (-b + sqrt(d)) / (2 * a) ;
			
			if (t1 > 0) {
				if (t1 <= smallDist) {
					smallDist = t1 ;
					closestObjIndex = i ;
					inter = Vector3(r.getX() + t1 * r.getDX(), r.getY() + t1 * r.getDY(), r.getZ() + t1 * r.getDZ()) ;
					objFlag = 1 ;
				}
			}
			else if (t2 > 0) {
				if (t2 <= smallDist) {
					smallDist = t2 ;
					closestObjIndex = i ;
					inter = Vector3(r.getX() + t2 * r.getDX(), r.getY() + t2 * r.getDY(), r.getZ() + t2 * r.getDZ()) ;
					objFlag = 1 ;
				}
			}
			else {
				continue ;
			}
		}
		else if (d == 0) {
			float t = (-b) / (2 * a) ;
			if (t <= smallDist) {
				smallDist = t ;
				closestObjIndex = i ;
				inter = Vector3(r.getX() + t * r.getDX(), r.getY() + t * r.getDY(), r.getZ() + t * r.getDZ()) ;
				objFlag = 1 ;
			}
		}
		else {
			continue ;
		}
	}
	for (int i = 0; i < s.faces.size(); i++) {
		Vector3 p0 = s.faces[i].getV1() ;
		Vector3 p1 = s.faces[i].getV2() ;
		Vector3 p2 = s.faces[i].getV3() ;
		Vector3 e1 = p1 - p0 ;
		Vector3 e2 = p2 - p0 ;
		Vector3 n = cross(e1, e2) ;
		
		float Ax = n.getX() * p0.getX() ;
		float By = n.getY() * p0.getY() ;
		float Cz = n.getZ() * p0.getZ() ;
		float D = -Ax - By - Cz ;
		
		float denom = n.getX() * r.getDX() + n.getY() * r.getDY() + n.getZ() * r.getDZ() ;
		if (denom == 0) {
			continue ;
		}
		float numer = -(n.getX() * r.getX() + n.getY() * r.getY() + n.getZ() * r.getZ() + D) ;
		float t = numer / denom ;
		if (t <= 0) {
			continue ;
		}
		Vector3 temp = Vector3(r.getX() + t * r.getDX(), r.getY() + t * r.getDY(), r.getZ() + t * r.getDZ()) ;
		
		Vector3 e3 = temp - p1 ;
		Vector3 e4 = temp - p2 ;
		float bigA = 1.0/2 * cross(e1, e2).getLength() ;
		float littleA = 1.0/2 * cross(e3, e4).getLength() ;
		float littleB = 1.0/2 * cross(e4, e2).getLength() ;
		float littleC = 1.0/2 * cross(e1, e3).getLength() ;
		float alpha = littleA / bigA ;
		float beta = littleB / bigA ;
		float gammaa = littleC / bigA ;
		if (alpha + beta + gammaa - 1 < 0.01) {
			if (t <= smallDist) {
				smallDist = t ;
				closestObjIndex = i ;
				inter = temp ;
				objFlag = 2 ;
			}
		}
		else {
			continue ;
		}				
	}
	if (closestObjIndex == -1) {
		return s.bkgColor ;
	}	
	Color color = shadeRay(r, s, inter, closestObjIndex, objFlag, depth) ;		
	return color ;
}

Color shadeRay(Ray ray, Scene s, Vector3 inter, int objectIndex, int objectFlag, int depth) {
	//objectFlag is a flag to denote what object we are shading, 1 = sphere and 2 = triangle		
	Color color = Color() ;
	float r = 0 ;
	float g = 0 ;
	float b = 0 ;

	if (objectFlag == 1) {
		//sphere
		
		//reflected ray
		//R = 2aN - I
		Vector3 Iprime = inter - ray.getOrigin() ;
		Vector3 I = -1 * normalize(Iprime) ;
		Vector3 normal = (inter - s.spheres[objectIndex].getCenter()) / s.spheres[objectIndex].getRadius() ;
		float a = dot(normal, I) ;
		Vector3 R = 2 * a * normal - I ;
		//this is necessary to avoid self-intersections, which gave a nasty, spotty appearance
		Ray refRay = Ray(inter + Vector3(0.01, 0.01, 0.01), R) ;
		Color refColor = traceRay(refRay, s, depth + 1) ;
		
		//Schlick & Fresnel
		//Fr = Fo + (1 - Fo)(1 - I dot N)^5
		//Fr = Ro + (1 - Ro)(1 - I dot N)^5
		float refIn = s.materials[s.spheres[objectIndex].getMaterial()].rIn ;
		float refInPow = (refIn - 1) / (refIn + 1) ;
		//float Fo = pow(refInPow, 2) ;
		float Ro = pow((refIn - 1.0) / (refIn + 1.0), 2) ;
		//float Fr = Fo + (1 - Fo) * pow((1 - dot(normal, I)), 5) ;
		float Fr = Ro + (1 - Ro) * pow((1 - dot(normal, I)), 5) ;
		//Fr should be between [Fo, 1]
		if (Fr > 1) {
			Fr = 1 ;
		}
		if (Fr < Ro) {
			Fr = Ro ;
		}
		Color ref = Color(refColor.getR() * Fr, refColor.getG() * Fr, refColor.getB() * Fr) ;
		
		//transmitted ray
		//(1 - Fr)(1 - alpha)T
		float opac = s.materials[s.spheres[objectIndex].getMaterial()].op ;
		float transStuff = (1 - Fr) * (1 - opac) ;
		Vector3 sqrtStuff = sqrt(1 - (pow(refIn / 1.0, 2) * (1 - pow(dot(normal, I), 2)))) * (-1 * normal) ;
		Vector3 otherStuff = (refIn / 1.0) * (dot(normal, I) * normal - I) ;
		Vector3 T = sqrtStuff + otherStuff ;
		//same thing here? YES - or at least it helps lol
		Ray transRay = Ray(inter + Vector3(0.01, 0.01, 0.01), T) ;
		Color transColor = traceRay(transRay, s, depth + 1) ;
		Color trans = Color((1 - Fr) * (1 - opac) * transColor.getR(),
							(1 - Fr) * (1 - opac) * transColor.getR(),
							(1 - Fr) * (1 - opac) * transColor.getR()) ;
	
		Color oD = s.materials[s.spheres[objectIndex].getMaterial()].oD ;
		Color oS = s.materials[s.spheres[objectIndex].getMaterial()].oS ;
		float kA = s.materials[s.spheres[objectIndex].getMaterial()].ka ;
		float kD = s.materials[s.spheres[objectIndex].getMaterial()].kd ;
		float kS = s.materials[s.spheres[objectIndex].getMaterial()].ks ;
		float n = s.materials[s.spheres[objectIndex].getMaterial()].n ;
		
		Vector3 sphereCenter = s.spheres[objectIndex].getCenter() ;
		Vector3 nV = (inter - sphereCenter) / s.spheres[objectIndex].getRadius() ;
		Vector3 vV = -1 * s.viewDir ;
		for (int i = 0; i < s.lights.size(); i++) {
			Vector3 lV = Vector3() ;
			Vector3 lightDir = Vector3(s.lights[i].x, s.lights[i].y, s.lights[i].z) ;
			if (s.lights[i].w == 0) {
				lV = normalize(Vector3(-1 * lightDir)) ;
			}
			else {
				lV = normalize(lightDir - inter) ;
			}
			Vector3 hV = normalize(lV + vV) ;
			
			r += clamp(s.lights[i].color.getR() * clamp(kD * oD.getR() * clamp(dot(nV, lV)) + kS * oS.getR() * clamp(pow(dot(nV, hV), n)))) ;
			g += clamp(s.lights[i].color.getG() * clamp(kD * oD.getG() * clamp(dot(nV, lV)) + kS * oS.getG() * clamp(pow(dot(nV, hV), n)))) ;
			b += clamp(s.lights[i].color.getB() * clamp(kD * oD.getB() * clamp(dot(nV, lV)) + kS * oS.getB() * clamp(pow(dot(nV, hV), n)))) ;
		}
		color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + refColor.getR() * Fr + (1 - Fr) * (1 - opac) * transColor.getR())) ;
		//color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + ref.getR())) ;
		//color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + trans.getR())) ;
		color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + refColor.getG() * Fr + (1 - Fr) * (1 - opac) * transColor.getG())) ;
		//color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + ref.getG())) ;
		//color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + trans.getG())) ;
		color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + refColor.getB() * Fr + (1 - Fr) * (1 - opac) * transColor.getB())) ;
		//color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + ref.getB())) ;
		//color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + trans.getB())) ;
	}
	else {
		//triangle
		
		Vector3 p0 = s.faces[objectIndex].getV1() ;
		Vector3 p1 = s.faces[objectIndex].getV2() ;
		Vector3 p2 = s.faces[objectIndex].getV3() ;
		Vector3 e1 = p1 - p0 ;
		Vector3 e2 = p2 - p0 ;
		Vector3 nV = cross(e1, e2) ;
		
		//reflected ray
		//R = 2aN - I
		Vector3 Iprime = inter - ray.getOrigin() ;
		Vector3 I = -1 * normalize(Iprime) ;
		float a = dot(nV, I) ;
		Vector3 R = 2 * a * nV - I ;
		Ray refRay = Ray(inter, R) ;
		Color refColor = traceRay(refRay, s, depth + 1) ;
		
		//Schlick & Fresnel
		//Fr = Fo + (1 - Fo)(1 - I dot N)^5
		float refIn = s.materials[s.faces[objectIndex].getMaterial()].rIn ;
		float refInPow = (refIn - 1) / (refIn + 1) ;
		float Fo = pow(refInPow, 2) ;
		float Fr = Fo + (1 - Fo) * pow((1 - dot(nV, I)), 5) ;
		//Fr should be between [Fo, 1]
		if (Fr > 1) {
			Fr = 1 ;
		}
		if (Fr < Fo) {
			Fr = Fo ;
		}
		Color ref = Color(refColor.getR() * Fr, refColor.getG() * Fr, refColor.getB() * Fr) ;
		
		//transmitted ray
		//(1 - Fr)(1 - alpha)T
		float opac = s.materials[s.faces[objectIndex].getMaterial()].op ;
		float transStuff = (1 - Fr) * (1 - opac) ;
		Vector3 sqrtStuff = sqrt(1 - (pow(1.0 / refIn, 2) * (1 - pow(dot(nV, I), 2)))) * (-1 * nV) ;
		Vector3 otherStuff = (1.0 / refIn) * (dot(nV, I) * nV - I) ;
		Vector3 T = sqrtStuff + otherStuff ;
		Ray transRay = Ray(inter, T) ;
		Color transColor = traceRay(transRay, s, depth + 1) ;
		Color trans = Color((1 - Fr) * (1 - opac) * transColor.getR(),
							(1 - Fr) * (1 - opac) * transColor.getR(),
							(1 - Fr) * (1 - opac) * transColor.getR()) ;
							
		Color oD = s.materials[s.faces[objectIndex].getMaterial()].oD ;
		Color oS = s.materials[s.faces[objectIndex].getMaterial()].oS ;
		float kA = s.materials[s.faces[objectIndex].getMaterial()].ka ;
		float kD = s.materials[s.faces[objectIndex].getMaterial()].kd ;
		float kS = s.materials[s.faces[objectIndex].getMaterial()].ks ;
		float n = s.materials[s.faces[objectIndex].getMaterial()].n ;
		
		/*Vector3 p0 = s.faces[objectIndex].getV1() ;
		Vector3 p1 = s.faces[objectIndex].getV2() ;
		Vector3 p2 = s.faces[objectIndex].getV3() ;
		Vector3 e1 = p1 - p0 ;
		Vector3 e2 = p2 - p0 ;
		Vector3 nV = cross(e1, e2) ;*/
		Vector3 vV = -1 * s.viewDir ;
		for (int i = 0; i < s.lights.size(); i++) {
			Vector3 lV = Vector3() ;
			Vector3 lightDir = Vector3(s.lights[i].x, s.lights[i].y, s.lights[i].z) ;
			if (s.lights[i].w == 0) {
				lV = normalize(-1 * lightDir) ;
			}
			else {
				lV = normalize(lightDir - inter) ;
			}
			Vector3 hV = normalize(lV + vV) ;
			
			r += clamp(s.lights[i].color.getR() * clamp(kD * oD.getR() * clamp(dot(nV, lV)) + kS * oS.getR() * clamp(pow(dot(nV, hV), n)))) ;
			g += clamp(s.lights[i].color.getG() * clamp(kD * oD.getG() * clamp(dot(nV, lV)) + kS * oS.getG() * clamp(pow(dot(nV, hV), n)))) ;
			b += clamp(s.lights[i].color.getB() * clamp(kD * oD.getB() * clamp(dot(nV, lV)) + kS * oS.getB() * clamp(pow(dot(nV, hV), n)))) ;
		}
		color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + refColor.getR() * Fr + (1 - Fr) * (1 - opac) * transColor.getR())) ;
		//color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + ref.getR())) ;
		//color.setR(clamp(clamp(clamp(kA * oD.getR()) + r) + trans.getR())) ;
		color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + refColor.getG() * Fr + (1 - Fr) * (1 - opac) * transColor.getG())) ;
		//color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + ref.getG())) ;
		//color.setG(clamp(clamp(clamp(kA * oD.getG()) + g) + trans.getG())) ;
		color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + refColor.getB() * Fr + (1 - Fr) * (1 - opac) * transColor.getB())) ;
		//color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + ref.getB())) ;
		//color.setB(clamp(clamp(clamp(kA * oD.getB()) + b) + trans.getB())) ;
	}
	return color ;
}

void parsingFail(int lC) {
	cout << "\n\tError: Parsing failed on line " << lC << ".\n\tCheck values and try again.\n" << endl ;
	exit(1) ;
}

Vector3 normalize(Vector3 v) {
	float mag = sqrt((v.getX() * v.getX()) + (v.getY() * v.getY()) + (v.getZ() * v.getZ())) ;
	if (mag != 0) {
		v.setX(v.getX() / mag) ;
		v.setY(v.getY() / mag) ;
		v.setZ(v.getZ() / mag) ;
	}
	return v ;
}

Vector3 cross(Vector3 v1, Vector3 v2) {
	return Vector3(v1.getY() * v2.getZ() - v1.getZ() * v2.getY(),
				   v1.getZ() * v2.getX() - v1.getX() * v2.getZ(),
				   v1.getX() * v2.getY() - v1.getY() * v2.getX()) ;
}

float dot(Vector3 v1, Vector3 v2) {
	float r = v1.getX() * v2.getX() + v1.getY() * v2.getY() + v1.getZ() * v2.getZ() ;
	return r ;
}

Ray makeRay (Vector3 origin, Vector3 direction) {
	Ray r ;
	Vector3 a = normalize(direction - origin) ;
	r.setOrigin(origin) ;
	r.setDirection(a) ;
	return r ;
}

float clamp(float v) {
	if (v > 1) {
		v = 1 ;
	}
	if (v < 0) {
		v = 0 ;
	}
	return v ;
}
