#include "GameObject.h"



GameObject::GameObject()
{
}


GameObject::GameObject(aie::Texture* a_texture, bool a_hasBB, glm::vec3 a_bbExtents, bool a_hasCol, ColliderType a_type, glm::vec3 a_collExtents, bool a_hasRigid) : Object()
{
	m_texture = (a_texture != nullptr) ? a_texture : nullptr;
	m_boundingBox = (a_hasBB) ? new BoundingBox(this, a_bbExtents) : nullptr;
	m_collider = (a_hasCol) ? new Collider(a_type, this, a_collExtents) : nullptr;
	m_rigidbody = (a_hasRigid) ? new RigidBody() : nullptr;
}

GameObject::~GameObject()
{
}

BoundingBox * GameObject::GetBoundingBox()
{
	return (m_boundingBox != nullptr) ? m_boundingBox : 0;
}

Collider * GameObject::GetCollider()
{
	return (m_collider != nullptr) ? m_collider : 0;
}

RigidBody * GameObject::GetRigidBody()
{
	return (m_rigidbody != nullptr) ? m_rigidbody : 0;
}

aie::Texture * GameObject::GetTexture()
{
	return m_texture;
}
