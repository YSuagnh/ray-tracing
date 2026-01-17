#pragma once
#ifndef __PHOTONMAPPING_HPP__
#define __PHOTONMAPPING_HPP__
#include "Photon.hpp"
#include "PhotonKDTree.hpp"
#include "scene/Scene.hpp"
#include <optional>
#include <vector>

namespace RayTracer {
	class PhotonMapping {
	private :
		SharedPhotonKDTree Tree;
		SharedScene spScene;
		Scene& scene;
		std::vector<AreaLight>* Light;
	public :
		PhotonMapping(SharedScene sp) : spScene(sp), scene(*sp), Light(&(sp->areaLightBuffer)){
			Tree = std::make_shared<PhotonKDTree>();
		}

		void build(int k = 10000000);

		void find(std::vector<Photon>& ret, glm::vec3 pos) {
			if (!Tree) return;
			return Tree->find(ret, pos);
		}
		void find(std::vector<Photon>& ret, glm::vec3 pos, int k) {
			if (!Tree) return;
			return Tree->find(ret, pos, k);
		}

		void findInRadius(std::vector<Photon>& ret, glm::vec3 pos, float radius) {
			if (!Tree) return;
			return Tree->findInRadius(ret, pos, radius);
		}
	};
};

#endif // !__PHOTONMAPPING_HPP__
