/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* Author: Xiaoshi Xie
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "TetrahedronFace.h"
#include "SceneObject.h"
#include "Plane.h"
#include "Ray.h"
#include <GL/glut.h>
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 1000;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

float a1 = -40.0, a2=40.0,b1 = -40.0, b2=-200.0;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene
TextureBMP textureFloor;
TextureBMP textureSphere;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 light2(-10, 60, -90);  //extra light
	glm::vec3 ambientCol(0.2);
	glm::vec3 specularCol(1,1,1);
	float ambientTerm = 0.2;   //Ambient color of light

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);//normal vector on the sphere at the point of intersection

    glm::vec3 lightVector = light - ray.xpt; //light vector(the vector from the point of intersection towards the light source1)
    glm::vec3 lightVector2 = light2 - ray.xpt; //light vector2(the vector from the point of intersection towards the light source2)
    
    float lightDist = glm::length(lightVector);    //Distance between light source1 and intersection point
    float lightDist2 = glm::length(lightVector2);    //Distance between light source2 and intersection point

    lightVector = glm::normalize(lightVector);//normalize lightVector1
    lightVector2 = glm::normalize(lightVector2);//normalize lightVectorw2

    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    glm::vec3 reflVector2 = glm::reflect(-lightVector2, normalVector);

    reflVector = glm::normalize(reflVector);
    reflVector2 = glm::normalize(reflVector2);
       
    float lDotn = glm::dot(lightVector,normalVector);
    float lDotn2 = glm::dot(lightVector2,normalVector);

   
    glm::vec3 v = -ray.dir;
    float rDotv = glm::dot(reflVector,v);
    float rDotv2 = glm::dot(reflVector2,v);
    
    Ray shadow(ray.xpt, lightVector);     //shadow ray
    shadow.closestPt(sceneObjects);
    
    Ray shadow2(ray.xpt, lightVector2);     //shadow ray2
    shadow2.closestPt(sceneObjects);
   
    glm::vec3 colorSum;
    glm::vec3 colFloor;           //texture floor color
    glm::vec3 colSphere;         //texture sphere color
    
	float spec;
	float spec2;

	if(ray.xindex==2){                        //floor texture mapping
		float texcoords = (ray.xpt.x-a1)/(a2-a1);
		float texcoordt=(ray.xpt.z-b1)/(b2-b1);
		colFloor = glm::vec3(0.5)*textureFloor.getColorAt(texcoords, texcoordt);
		materialCol = colFloor;

	}

	if(ray.xindex==1){                         //sphere texture mapping refered to https://en.wikipedia.org/wiki/UV_mapping
		glm::vec3 d = glm::vec3(9-ray.xpt.x,5-ray.xpt.y,-80-ray.xpt.z);
		d = glm::normalize(d);
		float texcoords = 0.5+(atan2(d.z,d.x)/(2*M_PI));
		float texcoordt = 0.5-(asin(d.y)/M_PI);
		colSphere = glm::vec3(0.4)*textureSphere.getColorAt(texcoords,texcoordt);
		materialCol = colSphere;
	}
	
    if((lDotn<=0 ||(shadow.xindex>-1&& shadow.xdist < lightDist))){
		colorSum += ambientTerm*materialCol;
	}else{
		if(rDotv<0){
		    spec = 0;
	    }else{
	        spec = pow(rDotv, 10);
	    }
		colorSum += (ambientTerm*materialCol+lDotn*materialCol+specularCol*spec);
	}
	if((lDotn2<=0 ||(shadow2.xindex>-1&& shadow2.xdist < lightDist2))){
		colorSum += ambientTerm*materialCol;
	}else{
		if(rDotv2<0){
		    spec2 = 0;
	    }else{
	        spec2 = pow(rDotv2, 10);
	    }
		colorSum += (ambientTerm*materialCol+lDotn2*materialCol+specularCol*spec2);
	}

	if((ray.xindex == 0 || ray.xindex==1||ray.xindex==3||ray.xindex==4) && step < MAX_STEPS){         //reflection
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step+1);// Recursion
		colorSum = colorSum + glm::vec3(0.6)*reflectedCol;
	}
	if((ray.xindex == 3) && step < MAX_STEPS)           //refraction
	{
		float eta = 1/1.001;
		glm::vec3 g = glm::refract(ray.dir, normalVector, eta);
		Ray refrRay1(ray.xpt, g);
		refrRay1.closestPt(sceneObjects);
		if(refrRay1.xindex == -1) return backgroundCol;      //If there is no intersection return background colour
		glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);		
		glm::vec3 h = glm::refract(g, -m, 1.0f/eta);	
		Ray refrRay2(refrRay1.xpt, h);
		glm::vec3 refractedCol = trace(refrRay2, step + 1);
		colorSum += glm::vec3(0.8)*refractedCol;
	}
		


	return colorSum;
}


void drawBox()           //box
{
	float min_x = 6, max_x = 10, min_y = -20, max_y = -16, min_z = -85, max_z = -89;
	glm::vec3 boxCol = glm::vec3(0, 1, 1); 
	Plane *top = new Plane(glm::vec3(min_x, max_y, max_z), 
	                       glm::vec3(min_x, max_y, min_z), 
	                       glm::vec3(max_x, max_y, min_z), 
	                       glm::vec3(max_x, max_y, max_z), boxCol);
	sceneObjects.push_back(top);
	Plane *bottom = new Plane(glm::vec3(min_x, min_y, min_z), 
	                          glm::vec3(min_x, min_y, max_z), 
	                          glm::vec3(max_x, min_y, max_z), 
	                          glm::vec3(max_x, min_y, min_z), boxCol);
	sceneObjects.push_back(bottom);
	Plane *right = new Plane(glm::vec3(max_x, max_y, min_z), 
	                         glm::vec3(max_x, min_y, min_z), 
	                         glm::vec3(max_x, min_y, max_z), 
	                         glm::vec3(max_x, max_y, max_z), boxCol);
	sceneObjects.push_back(right);
	Plane *left = new Plane(glm::vec3(min_x, max_y, max_z), 
	                        glm::vec3(min_x, min_y, max_z), 
	                        glm::vec3(min_x, min_y, min_z), 
	                        glm::vec3(min_x, max_y, min_z), boxCol);
	sceneObjects.push_back(left);
	Plane *front = new Plane(glm::vec3(min_x, min_y, min_z), 
							 glm::vec3(max_x, min_y, min_z), 
							 glm::vec3(max_x, max_y, min_z), 
							 glm::vec3(min_x, max_y, min_z), boxCol);
	sceneObjects.push_back(front);
	Plane *back = new Plane(glm::vec3(min_x, max_y, max_z), 
	                        glm::vec3(max_x, max_y, max_z), 
	                        glm::vec3(max_x, min_y, max_z), 
	                        glm::vec3(min_x, min_y, max_z), boxCol);
	sceneObjects.push_back(back);
}


void drawTetrahedron(){      //Tetrahedron
	float edge = 8.0;
	float bottom = -20;
	float top = (sqrt(6))/3*edge-20;
	float close_z = -90;
	float center = 15;
	glm::vec3 tetraCol = glm::vec3(1, 0, 1); 
	TetrahedronFace *faceRight = new TetrahedronFace(glm::vec3(center, bottom,close_z), 
													 glm::vec3(center+edge/2, bottom, close_z-(sqrt(3)/2)*edge),
	                                                 glm::vec3(center,top,close_z-(sqrt(3)/6)*edge),tetraCol);
    sceneObjects.push_back(faceRight);  	                                                 
	TetrahedronFace *faceBack = new TetrahedronFace(glm::vec3(center+edge/2, bottom, close_z-(sqrt(3)/2)*edge),
	                                                glm::vec3(center-edge/2, bottom, close_z-(sqrt(3)/2)*edge),	                                             
	                                                glm::vec3(center,top,close_z-(sqrt(3)/6)*edge),tetraCol);	
    sceneObjects.push_back(faceBack);  
	TetrahedronFace *faceLeft = new TetrahedronFace(glm::vec3(center-edge/2, bottom, close_z-(sqrt(3)/2)*edge),
	                                                glm::vec3(center, bottom, close_z),   	                                             
	                                                glm::vec3(center,top,close_z-(sqrt(3)/6)*edge),tetraCol);		                                                                                          
    sceneObjects.push_back(faceLeft);   
	TetrahedronFace *faceBottom = new TetrahedronFace(glm::vec3(center, bottom, close_z), 
													  glm::vec3(center+edge/2, bottom, close_z-(sqrt(3)/2)*edge),   	                                             
	                                                  glm::vec3(center-edge/2, bottom, close_z-(sqrt(3)/2)*edge),tetraCol);
    sceneObjects.push_back(faceBottom);  
}



glm::vec3 antialiasing(glm::vec3 eye,float xp, float yp,float cellX, float cellY)   //implements anti-aliasing by super sampling
{
	glm::vec3 col;
	glm::vec3 dir1(xp + cellX/4, yp + cellY/4, -EDIST);
	Ray ray1 = Ray(eye, dir1);
	ray1.normalize();
	glm::vec3 col1 = trace(ray1, 1); 
	glm::vec3 dir2(xp + cellX/4, yp + (3/4)* cellY, -EDIST);
	Ray ray2 = Ray(eye, dir2);
	ray2.normalize();
	glm::vec3 col2 = trace(ray2, 1); 
	glm::vec3 dir3(xp + (3/4) * cellX, yp + cellY/4, -EDIST);
	Ray ray3 = Ray(eye, dir3);
	ray3.normalize();
	glm::vec3 col3 = trace(ray3, 1); 
	glm::vec3 dir4(xp + (3/4) * cellX, yp + (3/4) * cellY, -EDIST);
	Ray ray4 = Ray(eye, dir4);
	ray4.normalize();
	glm::vec3 col4 = trace(ray4, 1); 
	col.r = (col1.r + col2.r + col3.r + col4.r) / 4;
	col.g = (col1.g + col2.g + col3.g + col4.g) / 4;
	col.b = (col1.b + col2.b + col3.b + col4.b) / 4;

	return glm::vec3(col.r, col.g, col.b);
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;
         //uncomment below to render without antialising
		    /*glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray
		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value
			glColor3f(col.r, col.g, col.b);*/

			glm::vec3 sampled_col = antialiasing(eye,xp,yp,cellX,cellY); //antialising
			glColor3f(sampled_col.r, sampled_col.g, sampled_col.b);		
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
	textureFloor = TextureBMP((char* const) "Sun.bmp");
	textureSphere = TextureBMP((char* const) "Earth.bmp");
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

	//-- Create a pointer to a sphere object
	Sphere *sphere1 = new Sphere(glm::vec3(0.0, -5.0, -110.0), 10.0, glm::vec3(0, 0, 1));
	Cylinder *cylinder = new Cylinder(glm::vec3(-9.0, -20.0, -90.0), 3.0, 20, glm::vec3(0, 1, 0));
	Cone *cone = new Cone(glm::vec3(0.0, -20.0, -90.0), 3.0, 10, glm::vec3(1, 0, 0));


	Sphere *sphere2 = new Sphere(glm::vec3(9.0, 5.0, -80.0), 3.0, glm::vec3(0, 0, 0));
    
    Plane *plane = new Plane (glm::vec3(a1,-20,b1), //point A
                              glm::vec3(a2, -20,b1 ), //Point B
                              glm::vec3(a2, -20, b2), //point c
                              glm::vec3(a1,-20,b2), //pointD
                              glm::vec3(0.,0.,0.0));     //color
                             

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1); //0
	sceneObjects.push_back(sphere2); //1
	sceneObjects.push_back(plane);    //2
	sceneObjects.push_back(cylinder);  //3
	sceneObjects.push_back(cone);     //4
	drawBox();                        //5-10
	drawTetrahedron();            //11-14

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
