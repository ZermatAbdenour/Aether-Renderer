#include "Entity.h"

Entity::Entity()
{
}
Entity::~Entity() {
    std::cout << "Entity " << Name << " destroyed" << std::endl;
}


void Entity::RemoveChild(std::shared_ptr<Entity> child)
{
    Childs.erase(std::remove(Childs.begin(), Childs.end(), child), Childs.end());
}

void Entity::AddChild(std::shared_ptr<Entity> child)
{
	Childs.push_back(child);
}

std::shared_ptr<Entity> Entity::AddChild(const char* name)
{
	std::shared_ptr<Entity> child = std::make_shared<Entity>();
    child->Name = name;

	Childs.push_back(child);
	return child;
}

void Entity::PrintEntityHierarchy(int depth) {
    // Print the entity's name and position with indentation based on depth
    std::string indent(depth * 2, ' '); // Indentation with spaces
    std::cout << indent << "Name: " << Name << ", Position: ("
        << Position.x << ", "
        << Position.y << ", "
        << Position.z << ")\n";

    // Recursively print child entities
    for (const auto& child : Childs) {
        child->PrintEntityHierarchy(depth + 1);
    }
}