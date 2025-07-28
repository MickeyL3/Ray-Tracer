/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>
#include <iostream>
/**
* Cylinder's intersection method.  The input is a ray. 
*/
float Cylinder::intersect(glm::vec3 p0, glm::vec3 dir) {

    
    float a = std::pow(dir.x, 2) + std::pow(dir.z, 2); // a = dx^2 + dz^2
    float b = 2 * (((p0.x - center.x) * dir.x) + ((p0.z - center.z) * dir.z)); // b = 2.((x0 - xC).dx + (z0 -zC).dz
    float c = std::pow(p0.x - center.x,2) + std::pow(p0.z - center.z,2) - std::pow(radius,2) ; //c = (x0 - xC)^2 + (z0 -zC)^2 - r^2

    float delta = b*b - 4 * (a*c);

    if(delta < 0.001 || delta < 0.0) return -1.0;    

    float t1 = (-b - sqrt(delta)) / (2*a);
    float t2 = (-b + sqrt(delta)) / (2*a);
    float t;
    if (t1 > 0 && t2 > 0) {
        if (t1 > t2) {
            t = t1;
            t1 = t2;
            t2 = t;
        }
    }

    //intersection with curve
    float p1 = p0.y + t1 * dir.y;
    if (t1 > 0 && p1 >= center.y && p1 <= center.y + height) {
        t = t1;
    } else {
        float p2 = p0.y + t2 * dir.y;
        if (t2 > 0 && p2 >= center.y && p2 <= center.y + height) {
            t = t2;
        }
    }

    // intersection with caps
    if (dir.y != 0.0f) {
        float bottomCap = (center.y - p0.y) / dir.y;
        glm::vec3 pBottom = p0 + dir * bottomCap;
        if (bottomCap > 0 && (std::pow(pBottom.x - center.x, 2) + std::pow(pBottom.z - center.z, 2)) <= std::pow(radius, 2)) {
            if (t < 0 || bottomCap < t) {
                t = bottomCap;
            }
        }
        float topCap = (center.y + height - p0.y) / dir.y;
        glm::vec3 pTop = p0 + dir * topCap;
        if (topCap > 0 && (std::pow(pTop.x - center.x, 2) + std::pow(pTop.z - center.z, 2)) <= std::pow(radius, 2)) {
            if (t < 0 || topCap < t) {
                t = topCap;
            }
        }
    }

return t;

}


/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cylinder::normal(glm::vec3 p) {

    //point on bottom cap
    if (fabs(p.y - center.y) < 1e-4)
        return glm::vec3(0.0, -1.0, 0.0);

    //point on top cap
    if (fabs(p.y - (center.y + height)) < 1e-4)
        return glm::vec3(0.0, 1.0, 0.0);

    glm::vec3 n = glm::vec3(p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
	return n;
}