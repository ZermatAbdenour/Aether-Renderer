#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "Editor.h"

Camera::Camera()
{
	speed = 2.5f;
	sensitivity = 0.1f;
}

glm::mat4 Camera::View() {
	return glm::lookAt(position, position + getForward(), glm::vec3(0.0f, 1.0f, 0.0f));
}
glm::mat4 Camera::Projection(int width, int height) {
	return glm::perspective(glm::radians(FOV), (float)width / height, nearPlane, farPlane);
}

glm::vec3 Camera::getForward() {
	return glm::normalize(glm::vec3(
		cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x)), 
		sin(glm::radians(eulerAngles.x)),                                    
		sin(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x)) 
	));
}

glm::vec3 Camera::getRight()
{
	return glm::normalize(glm::cross(getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::Update(GLFWwindow* window,Editor* editor, Time* time)
{
	int width, height;
	glfwGetWindowSize(window, &width,&height);
	projection = Projection(width, height);
	view = View();

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		mouseDown = true;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
		mouseDown = false;

	if (!mouseDown|| editor->uiInteracting) {
		glfwGetCursorPos(window, &lastX, &lastY);
		SetCursorMode(window, Normal);
		return;
	}
	SetCursorMode(window,Disabled);

    float velocity = speed * time->deltaTime;

	// Handle keyboard input for camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += getForward() * velocity; 
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= getForward() * velocity; 
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= getRight() * velocity;   
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += getRight() * velocity;    

	// Handle mouse movement for camera rotation
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	float xOffset = xpos - lastX;
	float yOffset = lastY- ypos;
	lastX = xpos;
	lastY = ypos;

	// Sensitivity handling for mouse movement
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	eulerAngles.y += xOffset; 
	eulerAngles.x += yOffset; 

	// Clamp pitch
	if (eulerAngles.x > 89.0f)
		eulerAngles.x = 89.0f;
	if (eulerAngles.x < -89.0f)
		eulerAngles.x = -89.0f;
}

vec3 Camera::directionToViewSpace(glm::vec3 direction)
{
	glm::vec4 dir = glm::vec4(direction.x, direction.y, direction.z, 1);
	dir = dir * glm::inverse(view);
	return vec3(dir[0], dir[1], dir[2]);
}

glm::vec3 Camera::viewSpaceToDirection(vec3 viewSpaceDirection)
{
	glm::vec4 dir = glm::vec4(viewSpaceDirection.x, viewSpaceDirection.y, viewSpaceDirection.z, 1);
	dir = dir * view;
	return glm::vec3(dir.x, dir.y, dir.z);
}

void Camera::SetCursorMode(GLFWwindow* window,Camera::CursorMode cursormode)
{
	glfwSetInputMode(window, GLFW_CURSOR, cursormode);
}