#pragma once
#include "Entity.h"

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

class EntityManager
{
private:
	std::vector<std::unique_ptr<Entity>> m_entities;

public:
	// Reserve space for entities to reduce reallocation
	void Reserve(size_t count) {
		m_entities.reserve(count);
	}

	void Clear() {
		m_entities.clear();
	}

	void AddEntity(std::unique_ptr<Entity> entity) {
		//m_entities.push_back(std::move(entity));
		m_entities.emplace_back(std::move(entity));
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
				[](const std::unique_ptr<Entity>& e) { return !e->IsActive(); }),
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

	template<typename T>
	std::vector<T*> GetEntityOfType() {
		std::vector<T*> result;
		result.reserve(m_entities.size()); // Reserve to avoid reallocations
		for (const auto& entity : m_entities) {
			if (auto* derived = dynamic_cast<T*>(entity.get())) {
				result.push_back(derived);
			}
		}
		return result;
	}

	template<typename T1, typename T2>
	void CheckCollision(const std::function<void(T1*, T2*)>& onCollision) {
		auto ent1 = GetEntityOfType<T1>();
		auto ent2 = GetEntityOfType<T2>();

		for (auto* e1 : ent1) {
			for (auto* e2 : ent2) {
				if (e1 == reinterpret_cast<void*>(e2)) continue; // This helps avoid duplicate checks and self-collision
				if (CheckCollisionRecs(e1->GetBounds(), e2->GetBounds())) {
					onCollision(e1, e2);
				}
			}
		}
	}
};