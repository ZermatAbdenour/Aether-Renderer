#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Entity::Entity()
{
    parent = nullptr;
}
Entity::~Entity() {
    std::cout << "Entity " << Name << " destroyed" << std::endl;
}


void Entity::RemoveChild(std::shared_ptr<Entity> child)
{
    child->parent = nullptr;
    childs.erase(std::remove(childs.begin(), childs.end(), child), childs.end());
}

void Entity::AddChild(std::shared_ptr<Entity> child)
{
    child->parent = this;
	childs.push_back(child);
}

std::shared_ptr<Entity> Entity::AddChild(const char* name)
{
	std::shared_ptr<Entity> child = std::make_shared<Entity>();
    child->Name = name;
    child->parent = this;
	childs.push_back(child);
	return child;
}

void Entity::CalculateModel()
{
    glm::mat4 translationMat = glm::translate(glm::mat4(1), localPosition);
    glm::mat4 rotationMat = glm::eulerAngleYXZ(glm::radians(eulerAngles.y), glm::radians(eulerAngles.x), glm::radians(eulerAngles.z));
    glm::mat4 scaleMat = glm::scale(glm::mat4(1),scale);

    if (parent != nullptr)
        model = parent->model * translationMat * rotationMat * scaleMat;
    else
        model = translationMat *rotationMat * scaleMat;
}

void Entity::PrintEntityHierarchy(int depth) {
    // Print the entity's name and position with indentation based on depth
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Name: " << Name << ", Position: ("
        << localPosition.x << ", "
        << localPosition.y << ", "
        << localPosition.z << ")\n";

    // Recursively print child entities
    for (const auto& child : childs) {
        child->PrintEntityHierarchy(depth + 1);
    }
}