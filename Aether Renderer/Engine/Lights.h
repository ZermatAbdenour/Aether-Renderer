#pragma once
#include <glm/glm.hpp>

struct PointLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec4 color;
	float intensity;
	PointLight() {
		position = glm::vec3();
		direction = glm::vec3(0, 0, 1);
		color = glm::vec4(1);
		intensity = 1;
	}
	PointLight(glm::vec3 pos, glm::vec3 dir, glm::vec4 col,float intens) {
		position = pos;
		direction = dir;
		color = col;
		intensity = intens;
	}
};

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec4 color;
	float intensity;
	DirectionalLight() {
		direction = glm::vec3(0, 0, 1);
		color = glm::vec4(1);
		intensity = 1;
	};
	DirectionalLight(glm::vec3 dir, glm::vec4 col,float intens) {
		direction = dir;
		color = col;
		intensity = intens;
	}
};