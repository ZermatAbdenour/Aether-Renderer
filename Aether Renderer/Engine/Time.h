#pragma once
#include <GLFW/glfw3.h>
//Time
struct Time {
	double time;
	double deltaTime;
	int FPS;
	void UpdateTime() {
		double currentTime = glfwGetTime();
		deltaTime = currentTime - time;
		time = currentTime;
		FPS = (int)(1.0 / deltaTime);
	}
};