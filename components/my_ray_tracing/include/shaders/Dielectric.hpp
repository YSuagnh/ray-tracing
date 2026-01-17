#pragma once
#ifndef __DIELECTRIC_HPP__
#define __DIELECTRIC_HPP__

#include "Shader.hpp"

namespace RayTracer
{
    /**
     * Simple dielectric (refraction) shader.
     *
     * Uses Fresnel (Schlick approximation) to probabilistically choose
     * reflection vs refraction.
     *
     * Material parameters:
     * - `ior` (float): index of refraction (e.g. 1.5)
     * - `absorbed` (RGB): per-bounce absorption/tint applied to transmitted rays
     */
    class Dielectric : public Shader
    {
    private:
        float ior = 1.5f;
        Vec3 absorbed = Vec3(1.0f);

        static float fresnelSchlick(float cosTheta, float etaI, float etaT);

    public:
        Dielectric(Material& material, vector<Texture>& textures);

        Scattered shade(const Ray& ray, const Vec3& hitPoint, const Vec3& normal) const override;
    };
}

#endif
