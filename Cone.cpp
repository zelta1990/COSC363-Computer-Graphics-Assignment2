/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir). 
*/

float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float a = pow(dir.x, 2.0)+pow(dir.z,2.0)-pow((radius/h)*dir.y, 2.0);
    float b = 2*(dir.x*(posn.x-center.x)+dir.z*(posn.z-center.z)-pow(radius/h,2.0)*dir.y*(posn.y-center.y-h));
    float c = pow(posn.x-center.x,2.0)+pow(posn.z-center.z,2)-pow(radius/h,2.0)*(pow(posn.y-center.y,2.0)+h*(h-2*posn.y+2*center.y));
    float delta = b*b-4.0*a*c;
    
	if(fabs(delta) < 0.0001||delta<0.0) return -1.0;
	float t1= (-b-sqrt(delta))/(2.0*a);
	float t2= (-b+sqrt(delta))/(2.0*a);
	
	if(fabs(t1) < 0.001)
    {
        if (t2 > 0&&( posn.y+t2*dir.y>=center.y && posn.y+t2*dir.y<=center.y+h)) return t2;
        else t2 = -1.0;
    }
    if(fabs(t2) < 0.001|| posn.y+t1*dir.y<center.y || posn.y+t1*dir.y>center.y+h) 
    {
        if (t1 > 0&&( posn.y+t1*dir.y>=center.y && posn.y+t1*dir.y<=center.y+h)) return t1;
        else t1 = -1.0;
    }
	return (t1 < t2)? t1: t2;
	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 pt)
{
	float theta = radius/h;   //half cone angle
	float alpha = glm::atan((pt.x-center.x)/(pt.z-center.z));
	glm::vec3 n = glm::vec3(glm::sin(alpha)*glm::cos(theta), glm::sin(theta),glm::cos(alpha)*glm::cos(theta));
    n = glm::normalize(n);

    return n;
}



