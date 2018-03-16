#pragma once
#include <glm/glm.hpp>
#include <iostream>

namespace Logging {
	void printVector(const glm::vec3 & Vector) {
		std::cout << "X: " << Vector.x << " Y:" << Vector.y << " Z:" << Vector.z << std::endl;
	}
}