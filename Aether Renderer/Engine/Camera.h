#pragma once
#include <Imgui/imGuIZMO.quat/vgMath.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Time.h"
class Editor;
class Camera {
public:
	enum CursorMode {
		Normal = 0x00034001,  //GLFW_CURSOR_NORMAL
		Hidden = 0x00034002,  //GLFW_CURSOR_HIDDEN
		Disabled = 0x00034003 //GLFW_CURSOR_DISABLED
	};

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
	bool mouseDown;
public:
	glm::vec3 getForward();
	glm::vec3 getRight();
	vec3 directionToViewSpace(glm::vec3 direction);
	glm::vec3 viewSpaceToDirection(vec3 viewSpaceDirection);
	void SetCursorMode(GLFWwindow* window, Camera::CursorMode cursormode);
	void Update(GLFWwindow* window,Editor* editor, Time* time);
private:
	glm::mat4 View();
	glm::mat4 Projection(int width, int height);
};