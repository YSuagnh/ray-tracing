#include "intersections/KDT.hpp"

#include <algorithm>
#include <limits>
#include <memory>
#include <numeric>

#include <glm/glm.hpp>

namespace RayTracer::KDT {
namespace {

constexpr int kMaxDepth = 20;
constexpr int kMinLeafPrimitives = 8;
constexpr float kEps = 1e-6f;

inline float centerAxis(const AABB_Box& b, int axis) {
	return 0.5f * (b.min[axis] + b.max[axis]);
}

inline int longestAxis(const AABB_Box& b) {
	Vec3 ext = b.max - b.min;
	if (ext.x >= ext.y && ext.x >= ext.z) return 0;
	if (ext.y >= ext.x && ext.y >= ext.z) return 1;
	return 2;
}

inline AABB_Box computeBounds(const std::vector<const Triangle*>& tris, const std::vector<const Sphere*>& sps) {
	bool hasAny = false;
	AABB_Box out{};

	auto addBox = [&](const AABB_Box& b) {
		if (!hasAny) {
			out = b;
			hasAny = true;
		} else {
			out = SurroundingBox(out, b);
		}
	};

	for (auto* t : tris) addBox(GetAABB(*t));
	for (auto* s : sps) addBox(GetAABB(*s));

	// 极端情况：场景为空
	if (!hasAny) {
		out.min = Vec3(0);
		out.max = Vec3(0);
	}
	return out;
}

inline bool chooseBestSplit(
	const std::vector<const Triangle*>& tris,
	const std::vector<const Sphere*>& sps,
	const AABB_Box& nodeBox,
	int& outAxis,
	float& outPos)
{
	outAxis = longestAxis(nodeBox);
	const float cmin = nodeBox.min[outAxis];
	const float cmax = nodeBox.max[outAxis];
	if (!(cmax - cmin > kEps))
		return false;

	// 取所有 primitive 的 bbox 中心作为候选 split（去重/排序后取中位数附近）
	std::vector<float> centers;
	centers.reserve(tris.size() + sps.size());

	for (auto* t : tris) centers.push_back(centerAxis(GetAABB(*t), outAxis));
	for (auto* s : sps) centers.push_back(centerAxis(GetAABB(*s), outAxis));

	if (centers.empty())
		return false;

	std::nth_element(centers.begin(), centers.begin() + centers.size() / 2, centers.end());
	float split = centers[centers.size() / 2];

	// clamp 到 box 内部一点点，避免退化
	split = std::clamp(split, cmin + kEps, cmax - kEps);

	outPos = split;
	return true;
}

inline void partitionPrimitives(
	const std::vector<const Triangle*>& tris,
	const std::vector<const Sphere*>& sps,
	int axis,
	float splitPos,
	std::vector<const Triangle*>& leftTris,
	std::vector<const Triangle*>& rightTris,
	std::vector<const Sphere*>& leftSps,
	std::vector<const Sphere*>& rightSps)
{
	leftTris.clear();
	rightTris.clear();
	leftSps.clear();
	rightSps.clear();

	// 采用“bbox 与半空间重叠就放入对应子节点”（可重复存放）
	for (auto* t : tris) {
		const auto b = GetAABB(*t);
		const bool inLeft = b.min[axis] <= splitPos;
		const bool inRight = b.max[axis] >= splitPos;
		if (inLeft) leftTris.push_back(t);
		if (inRight) rightTris.push_back(t);
	}
	for (auto* s : sps) {
		const auto b = GetAABB(*s);
		const bool inLeft = b.min[axis] <= splitPos;
		const bool inRight = b.max[axis] >= splitPos;
		if (inLeft) leftSps.push_back(s);
		if (inRight) rightSps.push_back(s);
	}
}

SharedKDTNode buildRecursive(
	const std::vector<const Triangle*>& tris,
	const std::vector<const Sphere*>& sps,
	int depth)
{
	auto nodeBounds = computeBounds(tris, sps);

	const int primCount = static_cast<int>(tris.size() + sps.size());
	if (primCount == 0) {
		auto leaf = std::make_shared<KDTLeaf>();
		leaf->box = nodeBounds;
		return leaf;
	}

	if (depth >= kMaxDepth || primCount <= kMinLeafPrimitives) {
		auto leaf = std::make_shared<KDTLeaf>();
		leaf->box = nodeBounds;
		leaf->triangles = tris;
		leaf->spheres = sps;
		return leaf;
	}

	int axis = 0;
	float splitPos = 0.0f;
	if (!chooseBestSplit(tris, sps, nodeBounds, axis, splitPos)) {
		auto leaf = std::make_shared<KDTLeaf>();
		leaf->box = nodeBounds;
		leaf->triangles = tris;
		leaf->spheres = sps;
		return leaf;
	}

	std::vector<const Triangle*> leftTris, rightTris;
	std::vector<const Sphere*> leftSps, rightSps;
	partitionPrimitives(tris, sps, axis, splitPos, leftTris, rightTris, leftSps, rightSps);

	// 避免无效拆分（完全没减少）
	if ((leftTris.size() == tris.size() && leftSps.size() == sps.size()) ||
		(rightTris.size() == tris.size() && rightSps.size() == sps.size())) {
		auto leaf = std::make_shared<KDTLeaf>();
		leaf->box = nodeBounds;
		leaf->triangles = tris;
		leaf->spheres = sps;
		return leaf;
	}

	auto internal = std::make_shared<KDTInternal>();
	internal->box = nodeBounds;
	internal->splitAxis = axis;
	internal->splitPos = splitPos;
	internal->left = buildRecursive(leftTris, leftSps, depth + 1);
	internal->right = buildRecursive(rightTris, rightSps, depth + 1);
	return internal;
}

inline float axisValue(const Vec3& v, int axis) { return v[axis]; }

} // namespace

HitRecord KDTInternal::intersect(const Ray& ray, float tMin, float tMax) {
	// 先做节点 AABB 裁剪
	if (!Intersection::xAABB(ray, box, tMin, tMax))
		return getMissRecord();

	// 简单近远子树顺序（按入射方向决定）
	const float dir = axisValue(ray.direction, splitAxis);
	const auto first = (dir >= 0.0f) ? left : right;
	const auto second = (dir >= 0.0f) ? right : left;

	HitRecord hit1 = first ? first->intersect(ray, tMin, tMax) : getMissRecord();
	if (hit1.has_value()) {
		// 已经命中更近的点，则收紧 tMax 再查另一个子树
		tMax = hit1->t;
	}
	HitRecord hit2 = second ? second->intersect(ray, tMin, tMax) : getMissRecord();

	if (hit2.has_value()) return hit2;
	return hit1;
}

HitRecord KDTLeaf::intersect(const Ray& ray, float tMin, float tMax) {
	if (!Intersection::xAABB(ray, box, tMin, tMax))
		return getMissRecord();

	HitRecord best = getMissRecord();
	float bestT = tMax;

	for (auto* t : triangles) {
		auto h = Intersection::xTriangle(ray, *t, tMin, bestT);
		if (h.has_value() && h->t < bestT) {
			bestT = h->t;
			best = h;
		}
	}
	for (auto* s : spheres) {
		auto h = Intersection::xSphere(ray, *s, tMin, bestT);
		if (h.has_value() && h->t < bestT) {
			bestT = h->t;
			best = h;
		}
	}
	return best;
}

KDTree::KDTree(const Scene& scene) {
	// 目前仅把 Scene 内的 sphereBuffer / triangleBuffer 纳入 KDTree（和 KDT.hpp 对齐）
	std::vector<const Triangle*> tris;
	std::vector<const Sphere*> sps;

	tris.reserve(scene.triangleBuffer.size());
	for (const auto& t : scene.triangleBuffer) tris.push_back(&t);

	sps.reserve(scene.sphereBuffer.size());
	for (const auto& s : scene.sphereBuffer) sps.push_back(&s);

	root = buildRecursive(tris, sps, 2);
}

} // namespace RayTracer::KDT