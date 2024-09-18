#pragma once
#include <iostream>
#include <vector>
#include "Entity.h"
#include "Effector.h"

class Scene {
public:
	std::vector<std::shared_ptr<Entity>> RootEntities;
private:
	std::vector<std::shared_ptr<Effector>> m_effectors;
public:
	void AddEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> AddEntity(const char* name);
	void RemoveEntity(std::shared_ptr<Entity> entity);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddEffector(std::shared_ptr<Entity> entity, Args&&... args);
	//Effectors Loop
	void StartEffectors();
	void UpdateEffectors();

	/// <summary>
	/// Debug: Print the scene heirarchy
	/// </summary>
	void PrintSceneHeirarchy();
};

#include "Scene.inl"