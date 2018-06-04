#pragma once
#include "Object.h"
#include "BoundingBox.h"
#include "Collider.h"
#include "RigidBody.h"
#include <Texture.h>
class GameObject : public Object
{
public:
	GameObject();
	GameObject(aie::Texture* a_texture, bool a_hasBB = true, glm::vec3 a_bbExtents = { 0, 0, 0 }, bool a_hasCol = true, ColliderType a_type = ColliderType::AABB, glm::vec3 a_collExtents = { 0, 0, 0 }, bool a_hasRigid = true);
	~GameObject();

	BoundingBox* GetBoundingBox();
	Collider* GetCollider();
	RigidBody* GetRigidBody();
	aie::Texture* GetTexture();

private:
	BoundingBox* m_boundingBox;
	Collider* m_collider;
	RigidBody* m_rigidbody;
	aie::Texture* m_texture;
};

