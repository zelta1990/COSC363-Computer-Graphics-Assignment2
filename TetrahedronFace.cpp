/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The TetrahedronFace class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "TetrahedronFace.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c.
*/
bool TetrahedronFace::isInside(glm::vec3 pt)
{
	glm::vec3 cross_A = glm::cross(b-a, pt-a);
	glm::vec3 cross_B = glm::cross(c-b, pt-b);
	glm::vec3 cross_C = glm::cross(a-c, pt-c);
	
	
    glm::vec3 n = normal(pt);
 
    float dot_A = glm::dot(cross_A, n);
    float dot_B = glm::dot(cross_B, n);
    float dot_C = glm::dot(cross_C, n);
    

	if(dot_A>=0 &&dot_B>=0 && dot_C>=0){
		return true;
	}else{
		return false;
	}	

}

/**
* TetrahedronFace's intersection method.  The input is a ray (pos, dir). 
*/
float TetrahedronFace::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 n = normal(posn);
	glm::vec3 vdif = a - posn;
	float vdotn = glm::dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	glm::vec3 q = posn + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the tetrahedronFace's normal vector using 
* member variables a, b, c.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 TetrahedronFace::normal(glm::vec3 pt)
{
	glm::vec3 n = glm::vec3(0);
    n = glm::cross(b-a, c-a);
    n = glm::normalize(n);

    return n;
}



