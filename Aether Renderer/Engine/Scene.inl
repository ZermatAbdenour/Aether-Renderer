#pragma once

template<typename T, typename... Args>
std::shared_ptr<T> Scene::AddEffector(std::shared_ptr<Entity> entity,Args&&... args) {
    static_assert(std::is_base_of<Effector, T>::value, "T must inherit from Effector");
    auto effector = std::make_shared<T>(std::forward<Args>(args)...);
    effector->EffectedEntity = entity;
    m_effectors.push_back(effector);
    return effector;
}
