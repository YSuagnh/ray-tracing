#include "intersections/AABB.hpp"
#include <glm/glm.hpp>

namespace RayTracer::AABB {
	AABB_Box GetAABB(const Triangle& t) {
		AABB_Box box;
		box.min = glm::min(glm::min(t.v1, t.v2), t.v3);
		box.max = glm::max(glm::max(t.v1, t.v2), t.v3);
		return box;
	}
	AABB_Box GetAABB(const Sphere& s) {
		AABB_Box box;
		box.min = s.position - Vec3(s.radius);
		box.max = s.position + Vec3(s.radius);
		return box;
	}
	AABB_Box GetAABB(const AreaLight& a) {
		AABB_Box box;
		box.min = a.position + glm::min(a.u, a.v);
		box.max = a.position + glm::max(a.u, a.v);
		return box;
	}
	AABB_Box SurroundingBox(const AABB_Box& box0, const AABB_Box& box1) {
		AABB_Box box;
		box.min = glm::min(box0.min, box1.min);
		box.max = glm::max(box0.max, box1.max);
		return box;
	}
}