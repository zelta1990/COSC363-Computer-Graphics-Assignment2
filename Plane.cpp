/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c, d.
*/
bool Plane::isInside(glm::vec3 pt)
{
	glm::vec3 cross_A = glm::cross(b-a, pt-a);
	glm::vec3 cross_B = glm::cross(c-b, pt-b);
	glm::vec3 cross_C = glm::cross(d-c, pt-c);
	glm::vec3 cross_D = glm::cross(a-d, pt-d);
	
	
    glm::vec3 n = normal(pt);
 
    float dot_A = glm::dot(cross_A, n);
    float dot_B = glm::dot(cross_B, n);
    float dot_C = glm::dot(cross_C, n);
    float dot_D = glm::dot(cross_D, n);
    

	if(dot_A>=0 &&dot_B>=0 && dot_C>=0&&dot_D>=0){
		return true;
	}else{
		return false;
	}	

}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
float Plane::intersect(glm::vec3 posn, glm::vec3 dir)
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
* Compute the plane's normal vector using 
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Plane::normal(glm::vec3 pt)
{
	glm::vec3 n = glm::vec3(0);
    n = glm::cross(b-a, d-a);
    n = glm::normalize(n);

    return n;
}



