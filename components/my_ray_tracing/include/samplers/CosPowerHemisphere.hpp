#pragma once
#ifndef __COS_POWER_HEMISPHERE_HPP__
#define __COS_POWER_HEMISPHERE_HPP__

#include "Sampler3d.hpp"
#include <ctime>
#include <algorithm>

namespace RayTracer
{
    using namespace std;

    /**
     * Cosine-power hemisphere sampler.
     *
     * Samples directions over the +Z hemisphere with PDF:
     *   p(¦Ø) = (n + 1) / (2¦Ð) * (cos¦È)^n
     * where ¦È is the angle from +Z.
     *
     * Returned direction is in local coordinates where +Z is the hemisphere axis.
     */
    class CosPowerHemisphere : public Sampler3d
    {
    private:
        constexpr static float C_PI = 3.14159265358979323846264338327950288f;

        default_random_engine e;
        uniform_real_distribution<float> u;

        float exponent = 1.0f;

    public:
        CosPowerHemisphere()
            : e((unsigned int)time(0) + insideSeed())
            , u(0.0f, 1.0f)
        {}

        explicit CosPowerHemisphere(float n)
            : CosPowerHemisphere()
        {
            setExponent(n);
        }

        void setExponent(float n) {
            exponent = std::max(0.0f, n);
        }

        float getExponent() const { return exponent; }

        Vec3 sample3d() override {
            const float u1 = u(e);
            const float u2 = u(e);

            const float n = exponent;
            const float phi = 2.0f * C_PI * u1;

            // Inversion method:
            // cos¦È = u2^(1/(n+1))
            const float cosTheta = pow(u2, 1.0f / (n + 1.0f));
            const float sinTheta = sqrt(std::max(0.0f, 1.0f - cosTheta * cosTheta));

            const float x = cos(phi) * sinTheta;
            const float y = sin(phi) * sinTheta;
            const float z = cosTheta;
            return { x, y, z };
        }

        /**
         * PDF for a given local direction (assumes dir is normalized).
         */
        float pdf(const Vec3& localDir) const {
            const float cosTheta = std::max(0.0f, localDir.z);
            return (exponent + 1.0f) / (2.0f * C_PI) * pow(cosTheta, exponent);
        }
    };
}

#endif
