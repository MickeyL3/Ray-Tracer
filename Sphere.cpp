/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Sphere.h"
#include <math.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

/**
* Sphere's intersection method.  The input is a ray. 
*/
float Sphere::intersect(glm::vec3 p0, glm::vec3 dir) {
    
    float t_;

    glm::vec3 p = glm::vec3(inverseTransf * glm::vec4(p0, 1.0f));

    glm::vec4 transformation = inverseTransf * glm::vec4(dir, 0.0f);
    t_ = glm::length(glm::vec3(transformation));
    glm::vec3 d = glm::normalize(glm::vec3(transformation)); //transformed dir

    glm::vec3 vdif = p - center;
    float a = glm::dot(d, d);
    float b = 2.0f * glm::dot(vdif, d);
    float c = glm::dot(vdif, vdif) - radius * radius;
    float delta = b * b - 4 * a * c;

    if(delta < 0.001) return -1.0;    //includes zero and negative values

    float t1 = (-b - sqrt(delta)) / (2*a);
    float t2 = (-b + sqrt(delta)) / (2*a);
    float t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
    if (t > 0) {
        return t / t_;
    } else {
        return -1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Sphere::normal(glm::vec3 p) {
	// glm::vec3 n = p - center;
	// n = glm::normalize(n);
	// return n;
    glm::vec3 obj_p = glm::vec3(inverseTransf * glm::vec4(p, 1.0f));
    glm::vec3 n = glm::normalize(obj_p - center);
    return glm::normalize(glm::vec3(normalTransf * glm::vec4(n, 0.0f)));

}

