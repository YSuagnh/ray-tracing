#pragma once
#ifndef __PHOTONKDTREE_HPP__
#define __PHOTONKDTREE_HPP__

#include "Photon.hpp"
#include "common/macros.hpp"

#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

namespace RayTracer {


    struct PhotonKDTreeNode {
        Vec3 min{};
        Vec3 max{};
        int axis = 0;
        int photonIndex = -1;               // index into PhotonKDTree::photons
        std::unique_ptr<PhotonKDTreeNode> left;
        std::unique_ptr<PhotonKDTreeNode> right;

        bool isLeaf() const { return !left && !right; }
    };

    class PhotonKDTree {
    private:
        std::vector<PhotonBase> photons;
        std::unique_ptr<PhotonKDTreeNode> root;

        static inline Vec3 photonPos(const PhotonBase& p) {
            return p.ray.origin;
        }

        static inline void expandBounds(Vec3& mn, Vec3& mx, const Vec3& p) {
            mn = glm::min(mn, p);
            mx = glm::max(mx, p);
        }

        std::unique_ptr<PhotonKDTreeNode> buildRecursive(std::vector<int>& indices, int begin, int end) {
            if (begin >= end) return nullptr;

            // compute bounds and choose split axis
            Vec3 mn(std::numeric_limits<float>::infinity());
            Vec3 mx(-std::numeric_limits<float>::infinity());
            for (int i = begin; i < end; ++i) {
                const Vec3 p = photonPos(photons[indices[i]]);
                expandBounds(mn, mx, p);
            }
            Vec3 ext = mx - mn;
            int axis = 0;
            if (ext.y > ext.x) axis = 1;
            if (ext.z > ext[axis]) axis = 2;

            const int mid = (begin + end) / 2;
            std::nth_element(indices.begin() + begin, indices.begin() + mid, indices.begin() + end,
                [&](int a, int b) {
                    return photonPos(photons[a])[axis] < photonPos(photons[b])[axis];
                });

            auto node = std::make_unique<PhotonKDTreeNode>();
            node->min = mn;
            node->max = mx;
            node->axis = axis;
            node->photonIndex = indices[mid];

            node->left = buildRecursive(indices, begin, mid);
            node->right = buildRecursive(indices, mid + 1, end);
            return node;
        }

        struct Candidate {
            float dist2;
            int index;
        };

        static inline float dist2(const Vec3& a, const Vec3& b) {
            Vec3 d = a - b;
            return glm::dot(d, d);
        }

        static inline float aabbDist2(const Vec3& p, const Vec3& mn, const Vec3& mx) {
            // squared distance from point to AABB (0 if inside)
            float dx = (p.x < mn.x) ? (mn.x - p.x) : (p.x > mx.x ? (p.x - mx.x) : 0.0f);
            float dy = (p.y < mn.y) ? (mn.y - p.y) : (p.y > mx.y ? (p.y - mx.y) : 0.0f);
            float dz = (p.z < mn.z) ? (mn.z - p.z) : (p.z > mx.z ? (p.z - mx.z) : 0.0f);
            return dx * dx + dy * dy + dz * dz;
        }

        void knnSearch(const PhotonKDTreeNode* node, const Vec3& pos, int k, std::vector<Candidate>& best) const {
            if (!node) return;

            // prune if this node's bbox is farther than current worst
            float worst = std::numeric_limits<float>::infinity();
            if ((int)best.size() >= k) worst = best.back().dist2;
            const float boxD2 = aabbDist2(pos, node->min, node->max);
            if (boxD2 > worst) return;

            // evaluate current photon
            if (node->photonIndex >= 0) {
                const Vec3 p = photonPos(photons[node->photonIndex]);
                const float d2 = dist2(pos, p);

                if ((int)best.size() < k) {
                    best.push_back({ d2, node->photonIndex });
                    std::sort(best.begin(), best.end(), [](auto& a, auto& b) { return a.dist2 < b.dist2; });
                }
                else if (d2 < best.back().dist2) {
                    best.back() = { d2, node->photonIndex };
                    std::sort(best.begin(), best.end(), [](auto& a, auto& b) { return a.dist2 < b.dist2; });
                }
            }

            // traverse near->far
            const float split = photonPos(photons[node->photonIndex])[node->axis];
            const float v = pos[node->axis];
            const PhotonKDTreeNode* first = (v < split) ? node->left.get() : node->right.get();
            const PhotonKDTreeNode* second = (v < split) ? node->right.get() : node->left.get();

            knnSearch(first, pos, k, best);
            knnSearch(second, pos, k, best);
        }

        void rangeSearch(const PhotonKDTreeNode* node, const Vec3& pos, float radius2, std::vector<int>& out) const {
            if (!node) return;

            // prune by aabb distance
            const float boxD2 = aabbDist2(pos, node->min, node->max);
            if (boxD2 > radius2) return;

            if (node->photonIndex >= 0) {
                const Vec3 p = photonPos(photons[node->photonIndex]);
                const float d2 = dist2(pos, p);
                if (d2 <= radius2) out.push_back(node->photonIndex);
            }

            rangeSearch(node->left.get(), pos, radius2, out);
            rangeSearch(node->right.get(), pos, radius2, out);
        }

    public:
        PhotonKDTree() = default;

        void build(const std::vector<Photon>& vec) {
            photons.clear();
            photons.reserve(vec.size());
            for (auto& ph : vec) {
                if (ph) photons.push_back(*ph);
            }

            std::vector<int> indices(photons.size());
            for (int i = 0; i < (int)indices.size(); ++i) indices[i] = i;
            root = buildRecursive(indices, 0, (int)indices.size());
        }

        void find(std::vector<Photon>& ret, glm::vec3 pos, int k = 50) const {
            ret.clear();
            if (!root || photons.empty() || k <= 0) return;

            std::vector<Candidate> best;
            best.reserve((size_t)k);
            knnSearch(root.get(), pos, k, best);

            ret.reserve(best.size());
            for (auto& c : best) {
                ret.push_back(std::make_optional<PhotonBase>(photons[c.index]));
            }
        }

        void findInRadius(std::vector<Photon>& ret, glm::vec3 pos, float radius) const {
            ret.clear();
            if (!root || photons.empty() || radius <= 0.0f) return;

            const float r2 = radius * radius;
            std::vector<int> idx;
            idx.reserve(64);
            rangeSearch(root.get(), pos, r2, idx);

            ret.reserve(idx.size());
            for (int i : idx) {
                ret.push_back(std::make_optional<PhotonBase>(photons[i]));
            }
        }
    };

    SHARE(PhotonKDTree);
};

#endif // !__PHOTONKDTREE_HPP__
