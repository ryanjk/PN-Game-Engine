#include "PN/Util/Math.h"

#include <iostream>

void print_vec3(vec3 v) {
	std::cout << "[ " << v[0] << ", " << v[1] << ", " << v[2] << " ]" << std::endl;
}

void print_mat4(mat4 m) {
	std::cout << "[ " << std::endl;
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			std::cout << m[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "]" << std::endl;
}

glm::fquat make_quat(const vec3& axis, float angle) {
	float w = (float)glm::cos(angle / 2.0f);
	vec3 rotation_axis = (float)glm::sin(angle / 2.0f) * glm::normalize(axis);

	return glm::fquat(w, rotation_axis);
}