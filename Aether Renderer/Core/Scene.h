#pragma once
#include <iostream>
#include <vector>
#include "Entity.h"
class Scene {
public:
	void AddEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> AddEntity(const char* name);
	void RemoveEntity(std::shared_ptr<Entity> entity);
	/// <summary>
	/// Debug: Print the scene heirarchy
	/// </summary>
	void PrintSceneHeirarchy();
private:
	std::vector<std::shared_ptr<Entity>> RootEntities;
};