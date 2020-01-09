#pragma once
#include "geometry.h"

// Adapted from https://github.com/ssloy/tinyraytracer

struct Ray
{
	Ray(vec3 origin, vec3 direction) : origin(origin), direction(direction) {}

	vec3 origin;
	vec3 direction;
	//vec3 energy;
};

struct Light {
	Light(const vec3 &p, const float &i) : position(p), intensity(i) {}
	vec3 position;
	float intensity;
};

struct Material {
	Material(const vec2 &a, const vec3 &color, const float &spec) : albedo(a), diffuse_color(color), specular_exponent(spec) {}
	Material() : albedo(1, 0), diffuse_color(), specular_exponent() {}
	vec2 albedo;
	vec3 diffuse_color;
	float specular_exponent;
};

struct Sphere {
	vec3 center;
	float radius;

	Material material;

	Sphere(vec3 c, const float &r, const Material &m) : center(c), radius(r), material(m) {}

	bool ray_intersect(const vec3 &orig, const vec3 &dir, float &t0) const {
		vec3 L = center - orig;
		float tca = L * dir;
		float d2 = L * L - tca * tca;
		if (d2 > radius*radius) return false;
		float thc = sqrtf(radius*radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		return true;
	}

	bool ray_intersect(Ray ray, float &outDistance) const {
		vec3 L = center - ray.origin;
		float tca = L * ray.direction;
		float d2 = L * L - tca * tca;
		if (d2 > radius * radius) return false;
		float thc = sqrtf(radius * radius - d2);
		outDistance = tca - thc;
		float t1 = tca + thc;
		if (outDistance < 0) outDistance = t1;

		if (outDistance < 0) return false;
		return true;
	}
};