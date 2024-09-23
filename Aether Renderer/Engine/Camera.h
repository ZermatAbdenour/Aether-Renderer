#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class Camera {
public:
	Camera();
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float FOV = 45.0f;
	glm::vec3 position;
	glm::vec3 eulerAngles;
	float speed;
	float sensitivity;

	glm::mat4 View();
	glm::mat4 Projection(int width, int height);

	glm::vec3 getForward();
	glm::vec3 getRight();

	void ProcessInputs(GLFWwindow* window,float deltaTime);
	double lastX = 800.0 / 2.0, lastY = 600.0 / 2.0;
};