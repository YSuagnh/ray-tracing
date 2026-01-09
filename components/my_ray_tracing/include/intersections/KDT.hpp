#pragma once
#ifndef __KDT_HPP__
#define __KDT_HPP__

#include "AABB.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"
#include "scene/Scene.hpp"
#include "intersections.hpp"

namespace RayTracer {
	namespace KDT {
		using namespace AABB;
		struct KDTNode {
			AABB_Box box;
			virtual ~KDTNode() = default;
			virtual HitRecord intersect(const Ray& ray, float tMin, float tMax) = 0;
		};
		SHARE(KDTNode);
		struct KDTInternal : public KDTNode {
			int splitAxis;
			float splitPos;
			SharedKDTNode left, right;
			virtual HitRecord intersect(const Ray& ray, float tMin, float tMax) override;
		};
		struct KDTLeaf : public KDTNode {
			std::vector<const Triangle*> triangles;
			std::vector<const Sphere*> spheres;
			virtual HitRecord intersect(const Ray& ray, float tMin, float tMax) override;
		};
		class KDTree {
		private :
			SharedKDTNode root;
		public :
			KDTree(const Scene& scene);
			HitRecord intersect(const Ray& ray, float tMin, float tMax) {
				return root->intersect(ray, tMin, tMax);
			}
		};
		SHARE(KDTree);
	}
}

#endif // __KDT_HPP__