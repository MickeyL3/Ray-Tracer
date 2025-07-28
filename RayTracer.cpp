
#include "TextureBMP.h"
#include "Plane.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

const float EDIST = 40.0;
const int NUMDIV = 1500;
const int MAX_STEPS = 5;
const float XMIN = -10.0;
const float XMAX = 10.0;
const float YMIN = -10.0;
const float YMAX = 10.0;
const float MAX_WIDTH = 40.0;
const float MAX_HEIGHT = 20.0;
const float Z_1 = 150.0;
const float Z_2 = 200.0;
const float TABLE_WIDTH = 22.0;
const float TABLE_HEIGHT = 12.0;
const float TABLE_Z1 = 90.0;
const float TABLE_Z2 = 180.0;
bool increaseLight = false;
vector<SceneObject*> sceneObjects;
TextureBMP texture;

//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------

glm::vec3 trace(Ray ray, int step) {
	glm::vec3 backgroundCol(0.5, 0.5, 0.5);						
	std::vector<glm::vec3> lightPos = {
		glm::vec3 (-10, 18, -5),
    	glm::vec3 (15, 18, -50),
		glm::vec3 (-4, 12, -110)
	};
	SceneObject* obj;
	
	ray.closestPt(sceneObjects);
	if(ray.index == -1) return backgroundCol;
	
	obj = sceneObjects[ray.index];	//object on which the closest point of intersection is found
	glm::vec3 color = 0.2f * obj->getColor();					
	glm::vec3 floorColor;
	glm::vec3 normalVec = obj->normal(ray.hit);
	glm::vec3 viewVec = -ray.dir;  // View vector (eye - intersection)
	
	//Chequered pattern for floor
	if (ray.index == 7) {
		int checkWidth = 5;
		int ix = int(ray.hit.x/ checkWidth);
		int iz = int(ray.hit.z/ checkWidth);
		
		int k = (ix + iz) % 2;
		if (k == 0) 
			floorColor = glm::vec3(0.0f, 0.0f, 0.0f);
		else
			floorColor = glm::vec3(0.6f, 0.6f, 0.6f);

		obj->setColor(floorColor);
    }

	if (ray.index == 5) {

		// Texture mapping for the cylinder
		glm::vec3 base = glm::vec3(15.0, -12, -155.0);
		float height = 8.0f;
		float theta = atan2((ray.hit - base).z, (ray.hit - base).x);
		float texcoords = (theta + M_PI) / (2.0f * M_PI); // normalize to 0 1
		float texcoordt = (ray.hit - base).y / height; 

		if (texcoords >= 0 && texcoords <= 1 &&
			texcoordt >= 0 && texcoordt <= 1)
		{
			color = texture.getColorAt(texcoords, texcoordt);
			obj->setColor(color);
		}
	}

	//Computing shadow multiple light source, lighter shadow, and spotlight
	for ( const glm::vec3& light : lightPos) 
	{
		glm::vec3 lightVec = light - ray.hit;
		Ray shadowRay(ray.hit, lightVec);
		float lightDist = glm::length(lightVec);
		shadowRay.closestPt(sceneObjects);
		//in shadow
		if (shadowRay.index > -1 && shadowRay.dist < lightDist) {
			SceneObject* shadowObj = sceneObjects[shadowRay.index];
			if (shadowObj->isTransparent() || shadowObj->isRefractive()) {
				color += 0.2f *  obj->lighting(light, viewVec, ray.hit);  // Soft shadow
			}
		//not in shadow
		} else {
			color += obj->lighting(light, viewVec, ray.hit)*0.35f;
		}
		//spotlight
		bool islightUp = true;
		if (shadowRay.index > -1 && shadowRay.dist < lightDist) {
			islightUp = false;
		}
		if (islightUp) {
			if (light == lightPos[2]) {
				glm::vec3 spotlightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
				float spotlightAngle = glm::radians(20.0f);
				float cosTheta = glm::dot(glm::normalize(-lightVec), spotlightDir);
				float theta = glm::acos(cosTheta);
				// outside spotlight angle 20 radian (in shadow)
				if (theta > spotlightAngle) {
				// In shadow
					islightUp = false;
				}
			}	
		}
		// Not in shadow, check for spotlight cone
		if (islightUp) {
			if (light == lightPos[2]) 
			{
				glm::vec3 spotlightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
				float spotlightAngle = glm::radians(20.0f);
				float cosTheta = glm::dot(glm::normalize(-lightVec), spotlightDir);
				float theta = glm::acos(cosTheta);
				if (theta < spotlightAngle) 
				{
					// In spotlight angle, increase light
					color += obj->lighting(light, viewVec, ray.hit) * 0.5f; 
					// color = glm::vec3(1, 0, 0);
				}
			} else {
					color += obj->lighting(light, viewVec, ray.hit)*0.35f;
			}
		}

	}
	
    // Reflection
    if (obj->isReflective() && step < MAX_STEPS) {
        float rho = obj->getReflectionCoeff();
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
        Ray reflectedRay(ray.hit, reflectedDir);
        glm::vec3 reflectedColor = trace(reflectedRay, step + 1);
        color += rho * reflectedColor;
    }

	//Refraction
	if (obj->isRefractive() && step < MAX_STEPS) 
	{
		float eta = 1.0f / obj->getRefractiveIndex();  
		glm::vec3 g = glm::refract(ray.dir, normalVec, eta); 

		if (glm::length(g) > 0.0f) {
			Ray refrRay(ray.hit, g);
			refrRay.closestPt(sceneObjects);

			glm::vec3 m = obj->normal(refrRay.hit); 
			glm::vec3 h = glm::refract(g, -m, 1.0f / eta); 

			if (glm::length(h) > 0.0f) {
				return trace(Ray(refrRay.hit, h), step + 1);
			}
		}
    }

	//Transparent 
	if (obj->isTransparent() && step < MAX_STEPS) 
	{
		float eta = obj->getTransparencyCoeff();
		Ray transRay(ray.hit + ray.dir * 0.001f, ray.dir);
		glm::vec3 transColor = trace(transRay, step + 1);
		glm::vec3 surfaceLight(0.0f);
		for (const glm::vec3& light : lightPos) {
			surfaceLight += obj->lighting(light, -ray.dir, ray.hit);
		}
		// Combine surface and transmit light
		color = (1.0f - eta) * surfaceLight + eta * transColor;
	}

	//Fog

	// float lambda = (glm::length(ray.hit -  glm::vec3(0.0f, 0.0f, 0.0f)) - Z_1) / (Z_2 - Z_1);
	// lambda = glm::clamp(lambda, 0.0f, 1.0f);
	// glm::vec3 fogColor(1.0f); //white
	// color = (1.0f - lambda) * color + lambda * fogColor;

	return color;
}

glm::vec3 Avg_Px(const glm::vec3& eye, float pxSize, float x1, float y1) {
    glm::vec3 colorSum(0.0f);
    float halfPx = pxSize / 2.0f;
    float quarterPx = pxSize / 4.0f;
    float samples[2] = {-quarterPx, quarterPx};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float sample_X = x1 + halfPx + samples[i];
            float sample_Y = y1 + halfPx + samples[j];
            glm::vec3 dir(sample_X, sample_Y, -EDIST);
            Ray ray(eye, glm::normalize(dir));
            colorSum += trace(ray, 1);
        }
    }

    return colorSum * 0.25f; 
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display() {
	float xp, yp;  //grid point
	float cellX = (XMAX - XMIN) / NUMDIV;  //cell width
	float cellY = (YMAX - YMIN) / NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for (int i = 0; i < NUMDIV; i++) {	//Scan every cell of the image plane
		xp = XMIN + i * cellX;
		for (int j = 0; j < NUMDIV; j++) {
			yp = YMIN + j * cellY;
			glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);	//direction of the primary ray
			Ray ray = Ray(eye, dir);
			glm::vec3 col = trace(ray, 1); //Trace the primary ray and get the colour value
			//glm::vec3 col = Avg_Px(eye, cellX, xp, yp); // comment out to enable anti-aliasing

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp + cellX, yp);
			glVertex2f(xp + cellX, yp + cellY);
			glVertex2f(xp, yp + cellY);
		}
	}

	glEnd();
	glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL 2D orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize() {
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
	glClearColor(0, 0, 0, 1);

	//Sphere 1 reflective sphere
	Sphere *sphere1 = new Sphere(glm::vec3(5.0, -6, -150.0), 5.0);
	sphere1->setColor(glm::vec3(0.0, 0.0, 0.0));   
	sphere1->setSpecularity(true);            // Enable specularity (default: true)
    sphere1->setShininess(50);                 // Default shininess value is 50
	sphere1->setReflectivity(true, 0.8);
	sceneObjects.push_back(sphere1);

	//Sphere 2 refractive sphere
	Sphere *sphere2 = new Sphere(glm::vec3(-4.5, -7, -110.0), 4.5);
	sphere2->setColor(glm::vec3(0.8, 0.9, 1.0)); //blue tint
	sphere2->setReflectivity(false);
	sphere2->setRefractivity(true, 1.0, 1.5);         
	sceneObjects.push_back(sphere2);

	//Sphere 3 transparent sphere
	Sphere *sphere3 = new Sphere(glm::vec3(-14, -6, -100.0), 5);
	sphere3->setColor(glm::vec3(0.8, 0.9, 1.0)); 
	sphere3->setTransparency(true);
	sceneObjects.push_back(sphere3);

	//Mirror
	Plane* mirror = new Plane(
	glm::vec3 (-20.0,-5.0 , -180.0),
	glm::vec3 ( 20.0,-5.0, -180.0),
	glm::vec3 ( 20.0, 15.0,-178.5),
	glm::vec3 (-20.0, 15.0, -178.5)
	);
	mirror->setColor(glm::vec3(0, 0, 0));
	mirror->setReflectivity(1.0);
	sceneObjects.push_back(mirror);

	//Cone 
	Cone* cone = new Cone(glm::vec3(17, -12, -105), 3.0, 6.0);
	cone->setColor(glm::vec3(0.6, 0.3, 0.5));   
	cone->setShininess(50);
	cone->setSpecularity(true);
	sceneObjects.push_back(cone);

	//Cylinder
	Cylinder *cylinder = new Cylinder(glm::vec3(15.0, -12, -155.0), 3.5, 8);

	cylinder->setColor(glm::vec3(0.0, 1.0, 0.0));   // Green
	cylinder->setShininess(50);
	cylinder->setSpecularity(true);
    sceneObjects.push_back(cylinder);

	//Transformation sphere
	Sphere* transfSphere = new Sphere(glm::vec3(0.0f), 2.0f);
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(4.2f, -5.0f, -48.0f));
	transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0,0,1));
	transform = glm::scale(transform, glm::vec3(1.0f, 0.5f, 1.0f));  // flatten on Y
	transfSphere->setTransform(transform);
	transfSphere->setColor(glm::vec3(0.32, 0.49, 0.46)); 
	transfSphere->setShininess(50);
	transfSphere->setSpecularity(true);
	sceneObjects.push_back(transfSphere);

	//Box: bottom, left wall, right wall, back wall, ceiling and front wall.
	Plane *bottom = new Plane 
	(glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, Z_1), //Point A  A (x1, y, z2) // -z more nev more front eye view
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, Z_1), //Point B B (x2, y, z2)
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, -Z_2), //Point C C (x2, y, z1)
	glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, -Z_2)); //Point D D (x1, y, z1)
	bottom->setColor(glm::vec3(0.6, 0.7, 0.8)); //blue grey
	bottom->setSpecularity(false);
	sceneObjects.push_back(bottom);

	// Left wall
	Plane *left = new Plane(
	glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, Z_1),
	glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, -Z_2),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, -Z_2),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, Z_1)
	);
	left->setColor(glm::vec3(0.858824, 0.439216, 0.858824)); //orchid
	left->setSpecularity(false);
	sceneObjects.push_back(left);

	// Right wall
	Plane *right = new Plane(
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, Z_1),
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, Z_1),
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, -Z_2),
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, -Z_2)
	);
	right->setColor(glm::vec3(0.137255, 0.556863, 0.419608));  //lime green
	right->setSpecularity(false);
	sceneObjects.push_back(right);

	// Back wall 
	Plane *back = new Plane(
	glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, -Z_2),
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, -Z_2),
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, -Z_2),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, -Z_2)
	);
	back->setColor(glm::vec3(0.196078, 0.6, 0.8)); 
	back->setSpecularity(false);
	sceneObjects.push_back(back);

	// Ceiling 
	Plane *top = new Plane(
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, Z_1),
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, -Z_2),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, -Z_2),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, Z_1)
	);
	top->setColor(glm::vec3(0.87, 0.58, 0.98)); //purple
	top->setSpecularity(false);
	sceneObjects.push_back(top);
 
	//Front
	Plane *front = new Plane(
	glm::vec3(-MAX_WIDTH, -MAX_HEIGHT, Z_1),
	glm::vec3(MAX_WIDTH, -MAX_HEIGHT, Z_1),
	glm::vec3(MAX_WIDTH, MAX_HEIGHT, Z_1),
	glm::vec3(-MAX_WIDTH, MAX_HEIGHT, Z_1)
	);
	front->setColor(glm::vec3(1, 0.5, 0)); //red orange
	front->setSpecularity(false);
	sceneObjects.push_back(front);

	// Table plane
	Plane* tableSurface = new Plane(
	glm::vec3(-TABLE_WIDTH, -TABLE_HEIGHT, -TABLE_Z1),
	glm::vec3(TABLE_WIDTH, -TABLE_HEIGHT, -TABLE_Z1),
	glm::vec3(TABLE_WIDTH, -TABLE_HEIGHT, -TABLE_Z2),
	glm::vec3(-TABLE_WIDTH, -TABLE_HEIGHT, -TABLE_Z2)
	);
	tableSurface->setColor(glm::vec3(0.91, 0.76, 0.65)); //Tan
	sceneObjects.push_back(tableSurface);


	// Table leg dimensions (legwidth 1, legheight 9),
	// x1 = x - legwidth / 2
	// x2 = x + legwidth / 2 
	// z1 = z - legwidth / 2
	// z2  = z + legwidth / 2
	// y1 = tableheight
	// y2 = legheight
	auto legSurfaces = [&](float x, float z) 
	{
		// 4 planes per leg
		Plane* front = new Plane(
		glm::vec3 (x - 0.5, -TABLE_HEIGHT, z - 0.5), 
		glm::vec3 (x - 0.5, -21, z - 0.5), 
		glm::vec3 (x + 0.5, -21, z - 0.5), 
		glm::vec3 (x + 0.5, -TABLE_HEIGHT, z - 0.5)
		);
		front->setColor(glm::vec3 (1, 0.43, 0.78));
		sceneObjects.push_back(front);

		Plane* back = new Plane(
		glm::vec3 (x + 0.5, -TABLE_HEIGHT, z + 0.5),
		glm::vec3 (x + 0.5, -21, z + 0.5),
		glm::vec3 (x - 0.5, -21, z + 0.5),
		glm::vec3 (x - 0.5, -TABLE_HEIGHT, z + 0.5)
		);
		back->setColor(glm::vec3 (1, 0.43, 0.78));
		sceneObjects.push_back(back);

		Plane* left = new Plane(
		glm::vec3 (x - 0.5, -TABLE_HEIGHT, z + 0.5), 
		glm::vec3 (x - 0.5, -21, z + 0.5), 
		glm::vec3 (x + 0.5, -21, z - 0.5), 
		glm::vec3 (x - 0.5, -TABLE_HEIGHT, z - 0.5)
		);
		left->setColor(glm::vec3 (1, 0.43, 0.78));
		sceneObjects.push_back(left);

		Plane* right = new Plane(
		glm::vec3 (x + 0.5, -TABLE_HEIGHT,z - 0.5), 
		glm::vec3 (x + 0.5, -21, z - 0.5), 
		glm::vec3 (x + 0.5, -21, z + 0.5),
		glm::vec3 (x + 0.5, -TABLE_HEIGHT, z + 0.5)
		);
		right->setColor(glm::vec3 (1, 0.43, 0.78));
		sceneObjects.push_back(right);

	
	};
	legSurfaces(-14, -91);  // FL
	legSurfaces(14, -91);   // FR
	legSurfaces(-14, -179);  // BL
	legSurfaces(14, -179);   // BR

	//texture
	texture = TextureBMP("../Butterfly.bmp");


}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("Raytracing");
	glutDisplayFunc(display);
	initialize();

	glutMainLoop();
	return 0;
}
