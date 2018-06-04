#include "Collider.h"
#include "GameObject.h"


Collider::Collider()
{
	m_type = ColliderType::AABB;
}

Collider::Collider(ColliderType a_type, GameObject* a_parent, glm::vec3 a_extents)
{
	m_type = a_type;
	m_center = a_parent->GetPositionVec4();
	m_extents = a_extents;
}


Collider::~Collider()
{
}

glm::vec4 Collider::GetCenter()
{
	return *m_center;
}

glm::vec3 Collider::GetExtents()
{
	return m_extents;
}

void Collider::debug()
{
	if (m_type == ColliderType::AABB)
	{
		aie::Gizmos::addAABB(*m_center, m_extents, { 1, 1, 0, 1 });
	}
	else if (m_type == ColliderType::SPHERE)
	{
		aie::Gizmos::addSphere(*m_center, m_extents.x, 32, 32, { 1, 1, 0, 1 });
	}
}
