#include "BoundingBox.h"
#include "GameObject.h"
#include <Gizmos.h>

BoundingBox::BoundingBox(GameObject* a_parent, glm::vec3 a_extents)
{
	m_boundingBox = a_parent->GetPositionVec4();
	if (a_parent->GetTexture() != nullptr)
	{
		a_extents = { a_parent->GetTexture()->getWidth(), a_parent->GetTexture()->getHeight(), 0 };
	}
	else
	{
		m_extents = a_extents;
	}
	
}


BoundingBox::~BoundingBox()
{
}

glm::vec4* BoundingBox::GetCenter()
{
	return m_boundingBox;
}

glm::vec3 BoundingBox::GetExtents()
{
	return m_extents;
}

void BoundingBox::debug()
{
	if (m_extents.y == 0 && m_extents.z == 0)
	{
		aie::Gizmos::addSphere(*m_boundingBox, m_extents.x, 32, 32, { 1, 0, 1, 1 });
	}
	else
	{
		aie::Gizmos::addAABB(*m_boundingBox, m_extents, { 1, 0, 1, 1 });
	}
}
