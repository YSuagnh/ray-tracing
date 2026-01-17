#pragma once
#ifndef __PHOTON_HPP__
#define __PHOTON_HPP__

#include <optional>
#include "geometry/vec.hpp"
#include "Ray.hpp"

namespace RayTracer {
	struct PhotonBase {
		Ray ray;
		glm::vec3 power;
	};

	using Photon = std::optional<PhotonBase>;

	inline Photon GetPhoton(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		return std::make_optional<PhotonBase>(Ray(a, b), c);
	}
	inline Photon GetPhoton(Ray ray, glm::vec3 power) {
		return std::make_optional<PhotonBase>(ray, power);
	}
};

#endif // __PHOTON_HPP__
