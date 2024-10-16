#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include "Entity.h"
#include "Effector.h"
#include "Camera.h"
#include "Lights.h"

class Renderer;

class Scene {
public:
public:
	std::vector<std::shared_ptr<Entity>> rootEntities;
	Camera camera;
	std::vector<DirectionalLight> DirectionalLights;
	std::vector<PointLight> PointLights;
private:
	std::vector<std::shared_ptr<Effector>> m_effectors;
	std::shared_ptr<Entity> m_selectedEntity;
public:
	Scene();
	~Scene() = default;
	std::shared_ptr<Entity> AddEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> AddEntity(const char* name);
	void RemoveEntity(std::shared_ptr<Entity> entity);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddEffector(std::shared_ptr<Entity> entity, Args&&... args);
	//Effectors Loop
	void StartEffectors();
	void UpdateEffectors(float deltaTime);

	void ForEachEntity(const std::function<void(std::shared_ptr<Entity>)>& func);
	void RenderLightingTab();
	void RenderSceneTab(Renderer* renderer);
	void RenderSceneHierarchyUI(std::shared_ptr<Entity> entity);
	/// <summary>
	/// Debug: Print the scene heirarchy
	/// </summary>
	void PrintSceneHeirarchy();
};

#include "Scene.inl"