#include "Move.h"
#include <iostream>

void Move::Start()
{
	std::cout << "Start of the Effector"<<std::endl;
}

void Move::Update(float deltaTime)
{
	std::cout << "effector is Updating"<<std::endl;
}