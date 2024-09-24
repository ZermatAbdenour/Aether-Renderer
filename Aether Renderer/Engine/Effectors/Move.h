#pragma once
#include"../Effector.h"
class Move:public Effector
{
public:
	void Start() override;
	void Update(float deltaTime) override;
};