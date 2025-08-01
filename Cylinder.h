/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Cylinder class
*  This is a subclass of SceneObject, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CYLINDER
#define H_CYLINDER
#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Defines a simple c located at 'center'
 * with the specified radius and height
 */
class Cylinder : public SceneObject {
private:
	glm::vec3 center = glm::vec3(0);
	float radius;
    float height;

public:
	
    Cylinder() : center(glm::vec3()), radius(1), height(1) {};  

	Cylinder(glm::vec3 c, float r, float h) : center(c), radius(r), height(h) {}

	float intersect(glm::vec3 p0, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);
};

#endif //!H_CYLINDER
