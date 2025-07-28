/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cone class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray. 
*/
float Cone::intersect(glm::vec3 p0, glm::vec3 dir) {

    float A = p0.x - center.x;              //A = x0 - xC
    float B = p0.z - center.z;              //B = z0 - zC
    float D = height - p0.y + center.y;      //D = h - y0 + yC

    float tan = (radius / height);

    //a.t^2 + b.t + c = 0
    float a = std::pow(dir.x, 2) + std::pow(dir.z, 2) - (std::pow(dir.y,2) * std::pow(tan,2)); //a = dx^2 + dz^2 - tan^2.dy^2
    float b = 2 * ((A * dir.x) + (B * dir.z) + (std::pow(tan,2) * D * dir.y)); // b = 2.((x0 - xC).dx + (z0 -zC).dz + tan^2(y0-yC).dy
    float c = (std::pow(A,2) + std::pow(B,2) - (std::pow(tan,2) * std::pow(D,2))); //c = (x0 - xC)^2 + (z0 -zC)^2 - tan^2.(y0-yC)^2

    float delta = b*b - 4 * (a*c);
    if(delta < 0.001 || delta < 0.0) return -1.0;    //includes zero and negative values

    float t1 = (-b - sqrt(delta)) / (2*a);
    float t2 = (-b + sqrt(delta)) / (2*a);
    float t = -1;

    if (t1 > 0) {
        t = t1;
    } else if (t2 > 0) {
        t = t2;
    } else if (t1 > 0 && t2 > 0) {
        t = std::min(t1, t2);
    } else {
        return -1;
    }

    //intersection within cone height
    float y = p0.y + t * dir.y;
    if (y < center.y + height && y > center.y)
        return t;  
    else
        return -1; 

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p) {

    float r = sqrt(std::pow(p.x - center.x, 2) + std::pow(p.z - center.z, 2));
    glm::vec3 n = glm::vec3(p.x - center.x, r * (radius / height), p.z - center.z);
    n = glm::normalize(n);
	return n;
}
