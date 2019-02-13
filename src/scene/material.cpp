#include "material.h"
#include "../ui/TraceUI.h"
#include "light.h"
#include "ray.h"
extern TraceUI* traceUI;

#include <glm/gtx/io.hpp>
#include <iostream>
#include "../fileio/images.h"

using namespace std;
extern bool debugMode;

Material::~Material()
{
}

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
glm::dvec3 Material::shade(Scene* scene, const ray& r, const isect& i) const
{
	// YOUR CODE HERE*

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and
	// shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	//	if( debugMode )
	//		std::cout << "Debugging Phong code..." << std::endl;

	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//
	// for ( const auto& pLight : scene->getAllLights() )
	// {
	//              // pLight has type unique_ptr<Light>
	// 		.
	// 		.
	// 		.
	// }

	// get point of intersction
	auto p = r.at(i.getT());

	// from slides, base color
	glm::dvec3 color = ke(i) + ka(i) * scene->ambient();
	
	for(auto& light: scene->getAllLights()) {
		// Direction from intersection to light
		glm::dvec3 L = glm::normalize(light->getDirection(p));
		glm::dvec3 light_color = light->shadowAttenuation(r, p);
		// The normal of intersection surface 
		glm::dvec3 N = glm::normalize(i.getN());

		bool going_in = glm::dot(r.getDirection(), N) < 0;
		if (!going_in) { N = -N; }

		// Direction towards the camera
		glm::dvec3 V = glm::normalize(scene->getCamera().getEye() - p);
		// Direction that a reflection would take
		// glm::dvec3 R = 2.0 * glm::dot(L, N) * N - L;
		glm::dvec3 R = -glm::reflect(L, N);
 		auto tmp = glm::dot(N, L);
 		if (this->Trans()) { tmp = abs(tmp); }
 		else tmp = max(tmp, 0.0);

		glm::dvec3 diffuse = kd(i)* tmp;

		// check if object is transparent?
		glm::dvec3 spec = !going_in ? glm::dvec3(0, 0, 0) : ks(i) * pow(max(glm::dot(V,R),0.0), shininess(i));
		color += light_color * (diffuse + spec) * light->distanceAttenuation(p);
	}

	return color;
}


TextureMap::TextureMap(string filename)
{
	data = readImage(filename.c_str(), width, height);
	if (data.empty()) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("'.");
		throw TextureMapException(error);
	}
}

glm::dvec3 TextureMap::getMappedValue(const glm::dvec2& coord) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 TextureMap::getPixelAt(int x, int y) const
{
	// YOUR CODE HERE
	//
	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.

	return glm::dvec3(1, 1, 1);
}

glm::dvec3 MaterialParameter::value(const isect& is) const
{
	if (0 != _textureMap)
		return _textureMap->getMappedValue(is.getUVCoordinates());
	else
		return _value;
}

double MaterialParameter::intensityValue(const isect& is) const
{
	if (0 != _textureMap) {
		glm::dvec3 value(
		        _textureMap->getMappedValue(is.getUVCoordinates()));
		return (0.299 * value[0]) + (0.587 * value[1]) +
		       (0.114 * value[2]);
	} else
		return (0.299 * _value[0]) + (0.587 * _value[1]) +
		       (0.114 * _value[2]);
}
