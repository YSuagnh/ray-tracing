#pragma once
#ifndef __KDT_HPP__
#define __KDT_HPP__

#include "AABB.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"

namespace RayTracer {
	namespace KDT {
		using namespace AABB;
		struct KDTNode {
			AABB_Box box;
			SharedKDTNode left, right;
		};
		SHARE(KDTNode);
		class KDTree {

		};
	}
}