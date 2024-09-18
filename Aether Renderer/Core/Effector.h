#pragma once
#include "Entity.h"
/// <summary>
/// You can think of effectors as components but they have limited functionality
/// They only have access to the EffectedEntity and they are isolated from the Scene
/// Effectors are added to create simple functionality scripts like adding moving an Entity on the X axe
/// </summary>

/// TODO: Add Access to applicaion time ( fps , currentTime, DeltaTime )
class Effector
{
public:
	/// <summary>
	/// Called once before the first frame
	/// </summary>
	virtual void Start() {}
	/// <summary>
	/// Called once every frame after the scene is rendered
	/// </summary>
	virtual void Update(){}

	std::shared_ptr<Entity> EffectedEntity;
};