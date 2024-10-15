#pragma once
#include <Imgui/imGuIZMO.quat/vgMath.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
class Camera {
public:
	Camera();
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	float FOV = 45.0f;
	glm::vec3 position ;
	glm::vec3 eulerAngles;
	float speed;
	float sensitivity;
	glm::mat4 projection;
	glm::mat4 view;
private:
	double lastX = 800.0 / 2.0, lastY = 600.0 / 2.0;
public:
	glm::vec3 getForward();
	glm::vec3 getRight();
	vec3 directionToViewSpace(glm::vec3 direction);
	glm::vec3 viewSpaceToDirection(vec3 viewSpaceDirection);

	void Update(GLFWwindow* window,float deltaTime);
private:
	glm::mat4 View();
	glm::mat4 Projection(int width, int height);
};