#pragma once
#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace RayTracer
{
	namespace AABB
	{
		struct AABB_Box {
			Vec3 min;
			Vec3 max;
		};
		AABB_Box GetAABB(const Triangle& t);
		AABB_Box GetAABB(const Sphere& s);
		AABB_Box GetAABB(const AreaLight& a);
		AABB_Box SurroundingBox(const AABB_Box& box0, const AABB_Box& box1);
	}
}

#endif // __AABB_HPP__