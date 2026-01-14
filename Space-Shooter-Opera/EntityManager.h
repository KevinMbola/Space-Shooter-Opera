#pragma once
#include "Entity.h"

#include <vector>
#include <memory>

class EntityManager
{
private:
	std::vector<std::unique_ptr<Entity>> m_entities;

public:
	void AddEntity(std::unique_ptr<Entity> entity) {
		m_entities.push_back(std::move(entity));
	}

	void UpdateAll(float dt) {
		for (auto& entity : m_entities) {
			if (entity->IsActive()) {
				entity->Update(dt);
			}
		}


		// Remove inactive entities (for object pooling)
		m_entities.erase(
			std::remove_if(m_entities.begin(), m_entities.end(),
				[](const auto& e) { return !e->IsActive(); }),
			m_entities.end()
		);
	}

	void DrawAll() const {
		for (auto& entity : m_entities) {
			if (entity->IsActive()) {
				entity->Draw();
			}
		}
	}
};