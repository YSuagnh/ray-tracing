#include "shaders/Dielectric.hpp"
#include "samplers/SamplerInstance.hpp"

namespace RayTracer
{
    static inline float saturate(float x) {
        return glm::clamp(x, 0.0f, 1.0f);
    }

    Dielectric::Dielectric(Material& material, vector<Texture>& textures)
        : Shader(material, textures)
    {
        auto iorValue = material.getProperty<Property::Wrapper::FloatType>("ior");
        if (iorValue) ior = (*iorValue).value;

        auto absorbedValue = material.getProperty<Property::Wrapper::RGBType>("absorbed");
        if (absorbedValue) absorbed = (*absorbedValue).value;

        // basic safety
        if (ior < 1.0f) ior = 1.0f;
        absorbed = NRenderer::clamp(absorbed, 1.0f, 0.0f);
    }

    float Dielectric::fresnelSchlick(float cosTheta, float etaI, float etaT)
    {
        cosTheta = saturate(cosTheta);
        const float r0 = (etaI - etaT) / (etaI + etaT);
        const float r02 = r0 * r0;
        const float m = 1.0f - cosTheta;
        const float m5 = m * m * m * m * m;
        return r02 + (1.0f - r02) * m5;
    }

    Scattered Dielectric::shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const
    {
        const Vec3 origin = hitPoint;
        const Vec3 wi = glm::normalize(-ray.direction); // towards surface

        // Determine if we are entering or exiting.
        // We assume `normal` points to the outside.
        Vec3 n = glm::normalize(normal);
        float cosThetaI = glm::dot(wi, n);

        float etaI = 1.0f;
        float etaT = ior;
        bool entering = cosThetaI > 0.0f;
        if (!entering) {
            // exiting: flip normal and swap indices
            n = -n;
            cosThetaI = -cosThetaI;
            etaI = ior;
            etaT = 1.0f;
        }

        cosThetaI = saturate(cosThetaI);

        const float eta = etaI / etaT;

        // Snell's law to detect total internal reflection
        const float sin2ThetaI = glm::max(0.0f, 1.0f - cosThetaI * cosThetaI);
        const float sin2ThetaT = eta * eta * sin2ThetaI;
        const bool totalInternalReflection = sin2ThetaT >= 1.0f;

        // Fresnel reflectance (Schlick)
        float Fr = fresnelSchlick(cosThetaI, etaI, etaT);
        if (totalInternalReflection) Fr = 1.0f;

        const float xi = defaultSamplerInstance<UniformSampler>().sample1d();

        Vec3 wo{};
        Vec3 attenuation(1.0f);
        float pdf = 1.0f;

        if (xi < Fr) {
            // reflect
            wo = glm::normalize(glm::reflect(-wi, n));
            attenuation = Vec3(1.0f);
            pdf = Fr;
        } else {
            // refract
            wo = glm::refract(-wi, n, eta);
            if (glm::length(wo) < 1e-6f) {
                // numerical fallback -> reflect
                wo = glm::normalize(glm::reflect(-wi, n));
                attenuation = Vec3(1.0f);
                pdf = 1.0f;
            } else {
                wo = glm::normalize(wo);

                // apply simple absorption/tint to transmitted rays only
                attenuation = absorbed;

                pdf = 1.0f - Fr;
            }
        }

        pdf = glm::max(pdf, 1e-8f);

        // NOTE: This is a simple delta BSDF; integrator will compute:
        //   emitted + attenuation * next * cos / pdf
        // For delta distributions, this is an approximation, but works as a simple model.
        return {
            Ray{ origin, wo },
            attenuation,
            Vec3(0.0f),
            pdf
        };
    }
}
