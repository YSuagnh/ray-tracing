#include "PhotonMapping/PhotonMapping.hpp"

#include "samplers/SamplerInstance.hpp"
#include "Onb.hpp"
#include "intersections/intersections.hpp"
#include "intersections/KDT.hpp"
#include "shaders/ShaderCreator.hpp"

#include <thread>

namespace RayTracer
{
    static constexpr float kEps = 1e-5f;

    static inline Vec3 samplePointOnAreaLight(const AreaLight& a) {
        const float u1 = defaultSamplerInstance<UniformSampler>().sample1d();
        const float u2 = defaultSamplerInstance<UniformSampler>().sample1d();
        return a.position + a.u * u1 + a.v * u2;
    }

    struct PhotonBuildContext {
        Scene& scene;
        std::vector<AreaLight>& lights;
        KDT::SharedKDTree kdtree;
        int maxBounce;
        const std::vector<SharedShader>* shaders; // sized as scene.materials
    };

    static HitRecord closestHitObjectKD(const PhotonBuildContext& ctx, const Ray& r, float& closest) {
        auto hitRecordKDT = ctx.kdtree->intersect(r, kEps, closest);
        if (hitRecordKDT) return hitRecordKDT;

        HitRecord closestHit = nullopt;
        for (auto& p : ctx.scene.planeBuffer) {
            auto hitRecord = Intersection::xPlane(r, p, kEps, closest);
            if (hitRecord && hitRecord->t < closest) {
                closest = hitRecord->t;
                closestHit = hitRecord;
            }
        }
        return closestHit;
    }

    static void photonBuildTask(
        const PhotonBuildContext& ctx,
        std::vector<Photon>& out,
        int photonCount,
        int off,
        int step)
    {
        out.clear();
        out.reserve((size_t)photonCount / (size_t)step);
        if (ctx.lights.empty() || !ctx.shaders) return;

        for (int i = off; i < photonCount; i += step) {
            const AreaLight& a = ctx.lights[0];

            const Vec3 pos = samplePointOnAreaLight(a);
            const Vec3 ln = glm::normalize(glm::cross(a.u, a.v));

            // Emit direction: cosine hemisphere around light normal
            Vec3 local = defaultSamplerInstance<CosPowerHemisphere>().sample3d();
            Onb onb{ ln };
            Vec3 dir = glm::normalize(onb.local(local));

            // Initial photon power (naive): proportional to radiance
            Vec3 throughput = a.radiance;
            Ray ray{ pos + ln * kEps, dir };

            for (int bounce = 0; bounce < ctx.maxBounce; ++bounce) {
                float closest = FLOAT_INF;
                HitRecord hit = closestHitObjectKD(ctx, ray, closest);
                if (!hit) break;

                const size_t midx = hit->material.index();
                if (midx >= ctx.shaders->size()) break;

                // Store only on diffuse hits.
                bool isDiffuse = true;
                {
                    int shaderType = 1;
                    auto st = ctx.scene.materials[midx].getProperty<Property::Wrapper::IntType>("ShaderType");
                    if (st) shaderType = (*st).value;
                    if (shaderType == 3) isDiffuse = false; // Dielectric
                }

                if (isDiffuse) {
                    PhotonBase pb;
                    pb.ray = Ray(hit->hitPoint, -ray.direction); // incoming direction at hit
                    pb.power = throughput;
                    out.push_back(std::make_optional<PhotonBase>(pb));
                }

                const auto& shader = (*ctx.shaders)[midx];
                if (!shader) break;

                Scattered scattered = shader->shade(ray, hit->hitPoint, hit->normal);

                const float cosTheta = glm::dot(hit->normal, scattered.ray.direction);
                const float pdf = glm::max(scattered.pdf, 1e-8f);
                throughput *= scattered.attenuation * glm::max(cosTheta, 0.0f) / pdf;

                if (glm::length(throughput) < 1e-6f) break;

                ray = Ray(scattered.ray.origin + glm::normalize(hit->normal) * kEps, scattered.ray.direction);
            }
        }
    }

    void PhotonMapping::build(int k)
    {
        if (!Tree) Tree = std::make_shared<PhotonKDTree>();
        if (!Light) {
            std::vector<Photon> empty;
            Tree->build(empty);
            return;
        }

        std::vector<AreaLight>& lights = *Light;
        if (lights.empty() || k <= 0) {
            std::vector<Photon> empty;
            Tree->build(empty);
            return;
        }

        // Build KDTree once for fast photon tracing intersections.
        auto kdtree = std::make_shared<KDT::KDTree>(scene);

        // Build shaders once for throughput update
        std::vector<SharedShader> shaders;
        shaders.reserve(scene.materials.size());
        ShaderCreator shaderCreator{};
        for (auto& m : scene.materials) {
            shaders.push_back(shaderCreator.create(m, scene.textures));
        }

        PhotonBuildContext ctx{ scene, lights, kdtree, (int)glm::max(1u, scene.renderOption.depth), &shaders };

        const int taskNums = 8;
        std::thread threads[taskNums];
        std::vector<Photon> localPhotons[taskNums];

        for (int t = 0; t < taskNums; ++t) {
            threads[t] = std::thread(photonBuildTask, std::cref(ctx), std::ref(localPhotons[t]), k, t, taskNums);
        }
        for (int t = 0; t < taskNums; ++t) {
            threads[t].join();
        }

        std::vector<Photon> photons;
        size_t total = 0;
        for (int t = 0; t < taskNums; ++t) total += localPhotons[t].size();
        photons.reserve(total);
        for (int t = 0; t < taskNums; ++t) {
            photons.insert(photons.end(), localPhotons[t].begin(), localPhotons[t].end());
        }

        Tree->build(photons);
    }
}
