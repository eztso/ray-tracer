#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

glm::dvec3 DirectionalLight::dsaHelper(ray &r, const glm::dvec3& p, glm::dvec3 currHelper) const{
	isect i;
	if(scene->intersect(r,i)) {
		glm::dvec3 iPoint = r.at(i.getT());
		glm::dvec3 N = i.getN();
		bool going_in = glm::dot(N, r.getDirection() ) < 0;
		ray next (iPoint, r.getDirection(), glm::dvec3(1,1,1), ray::SHADOW);
		if(going_in) {
			if(glm::length(i.getMaterial().kt(i)) == 0)
			{
				return glm::dvec3(0, 0, 0);
			}
			return dsaHelper(next, iPoint, currHelper);

		}
		else {
			currHelper = currHelper * (glm::pow(i.getMaterial().kt(i), glm::dvec3(1,1,1) * glm::length(iPoint - r.getPosition())));
			return dsaHelper(next, iPoint, currHelper);
		}
	}
	else
		return color * currHelper;
} 

glm::dvec3 PointLight::psaHelper(ray &r, const glm::dvec3& p, glm::dvec3 currHelper) const{
	isect i;

	if(scene->intersect(r,i)) {
		glm::dvec3 iPoint = r.at(i.getT());
		bool behindLight = glm::length(p - this->position) <= glm::length(p - iPoint);
		if(behindLight) {
			return color * currHelper;
		}

		glm::dvec3 N = i.getN();
		bool going_in = glm::dot(N, r.getDirection() ) < 0;
		ray next (iPoint, r.getDirection(), glm::dvec3(1,1,1), ray::SHADOW);
		if(going_in) {
			if(glm::length(i.getMaterial().kt(i)) == 0)
			{
				return glm::dvec3(0, 0, 0);
			}
			return psaHelper(next, iPoint, currHelper);

		}
		else {
			currHelper = currHelper * (glm::pow(i.getMaterial().kt(i), glm::dvec3(1,1,1) * glm::length(iPoint - r.getPosition())));
			return psaHelper(next, iPoint, currHelper);
		}
	}
	else
		return color * currHelper;
} 


double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE*:
	// You should implement shadow-handling code here.
	// still don't know what this does but ray constructor requires a 3rd vector
	ray rayToLight (p, -orientation, glm::dvec3(1,1,1), ray::SHADOW);
	return dsaHelper(rayToLight, p, glm::dvec3(1.0,1.0,1.0));

}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE*

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0
	double d = glm::length(P - position);
	double I_att = 1.0 / (constantTerm + linearTerm * d + quadraticTerm * d * d);

	// Light intensity should only get weaker or stay the same
	// return attenuation;
	return glm::clamp(I_att, 0.0, 1.0);
	// if(I_att > 1.0) return 1;
	// else if(I_att < 0.0) return 0;
	// return I_att;
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	 ray tmp (p,this->getDirection(p), glm::dvec3(1.0,1.0,1.0), ray::SHADOW);

	 return psaHelper(tmp, p, glm::dvec3(1.0,1.0,1.0));

	// YOUR CODE HERE*:
	// You should implement shadow-handling code here.
	// Lighting model equation: http://www.cs.utexas.edu/~fussell/courses/cs354/assignments/raytracing/equations.pdf
	


}


#define VERBOSE 0

