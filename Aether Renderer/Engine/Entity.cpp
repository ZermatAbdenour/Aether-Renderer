#include "Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Entity::Entity()
{
    Parent = nullptr;
}
Entity::~Entity() {
    std::cout << "Entity " << Name << " destroyed" << std::endl;
}


void Entity::RemoveChild(std::shared_ptr<Entity> child)
{
    child->Parent = nullptr;
    Childs.erase(std::remove(Childs.begin(), Childs.end(), child), Childs.end());
}

void Entity::AddChild(std::shared_ptr<Entity> child)
{
    child->Parent = this;
	Childs.push_back(child);
}

std::shared_ptr<Entity> Entity::AddChild(const char* name)
{
	std::shared_ptr<Entity> child = std::make_shared<Entity>();
    child->Name = name;
    child->Parent = this;
	Childs.push_back(child);
	return child;
}

void Entity::CalculateModel()
{
    glm::mat4 translation = glm::translate(glm::mat4(1), LocalPosition);
    glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(EulerAngles.y), glm::radians(EulerAngles.x), glm::radians(EulerAngles.z));
    glm::mat4 scale = glm::scale(glm::mat4(1),Scale);

    if (Parent != nullptr)
        Model = Parent->Model * translation * rotation * scale;
    else
        Model = translation *rotation * scale;
}

void Entity::PrintEntityHierarchy(int depth) {
    // Print the entity's name and position with indentation based on depth
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Name: " << Name << ", Position: ("
        << LocalPosition.x << ", "
        << LocalPosition.y << ", "
        << LocalPosition.z << ")\n";

    // Recursively print child entities
    for (const auto& child : Childs) {
        child->PrintEntityHierarchy(depth + 1);
    }
}