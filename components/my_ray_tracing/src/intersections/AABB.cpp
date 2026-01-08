#include "intersections/AABB.hpp"
#include "glm.hpp"

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
}