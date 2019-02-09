#include <cmath>
#include <iostream>

#include "light.h"
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE*:
	// You should implement shadow-handling code here.
	auto w = glm::dvec3(0, 0, 0);
	ray pl(p, -orientation, w, ray::SHADOW);
  	isect intersection;

	if (!scene->intersect(pl, intersection))
	{
		return color;
	} 

	auto point_of_intersect = pl.at(intersection.getT());
	auto kt = intersection.getMaterial().kt(intersection);
	auto Ia = shadowAttenuation(r, point_of_intersect);

	return Ia * kt;

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
	if(I_att > 1.0) return 1;
	else if(I_att < 0.0) return 0;
	return I_att;
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
	// YOUR CODE HERE*:
	// You should implement shadow-handling code here.
	// Lighting model equation: http://www.cs.utexas.edu/~fussell/courses/cs354/assignments/raytracing/equations.pdf
	auto w = glm::dvec3(0, 0, 0);
	ray v_PL(p, getDirection(p), w, ray::SHADOW);
	isect intersection;

	bool check_intersect = scene->intersect(v_PL, intersection);
	auto point_of_intersect = v_PL.at(intersection.getT());

	double m_PI = glm::length((point_of_intersect - p));
	double m_PL = glm::length(position - p);
	bool valid_intersect = (m_PI < m_PL);
	if (!check_intersect || !valid_intersect)
	{
		return color;
	}
  	auto kt = intersection.getMaterial().kt(intersection);
  	auto Ia = shadowAttenuation(r, point_of_intersect);

  	return Ia * kt;
}

#define VERBOSE 0

