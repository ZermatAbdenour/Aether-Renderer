#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "Entity.h"
#include "Effector.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class Scene {
public:
	struct Camera {
		Camera() {
			position = glm::vec3(0);
			eulerAngles = glm::vec3(0);
		}
		glm::vec3 position;
		glm::vec3 eulerAngles;
		glm::mat4 view() {
			glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(eulerAngles.y), glm::radians(eulerAngles.x), glm::radians(eulerAngles.z));
			glm::mat4 translation = glm::translate(glm::mat4(1), -position);
			return translation * rotation;
		}
		glm::mat4 projection(int width,int height) {
			return glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
		}
	};
public:
	std::vector<std::shared_ptr<Entity>> rootEntities;
	Camera camera;
private:
	std::vector<std::shared_ptr<Effector>> m_effectors;
public:
	Scene();
	~Scene() = default;
	void AddEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> AddEntity(const char* name);
	void RemoveEntity(std::shared_ptr<Entity> entity);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddEffector(std::shared_ptr<Entity> entity, Args&&... args);
	//Effectors Loop
	void StartEffectors();
	void UpdateEffectors();

	void ForEachEntity(const std::function<void(std::shared_ptr<Entity>)>& func);
	/// <summary>
	/// Debug: Print the scene heirarchy
	/// </summary>
	void PrintSceneHeirarchy();
};

#include "Scene.inl"