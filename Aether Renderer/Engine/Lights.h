#pragma once
#include <glm/glm.hpp>

struct PointLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 color;
	PointLight() = default;
	PointLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 col) {
		position = pos;
		direction = dir;
		color = col;
	}
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec4 color;
	DirectionalLight() = default;
	DirectionalLight(glm::vec3 dir, glm::vec4 col) {
		direction = dir;
		color = col;
	}
};