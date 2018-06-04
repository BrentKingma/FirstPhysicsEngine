#pragma once
#include <glm/glm.hpp>
#include <Gizmos.h>
class GameObject;
enum ColliderType
{
	AABB,
	SPHERE,
};

class Collider
{
public:
	Collider();
	//If it is a sphere just use the x value of the extents argument for radius
	Collider(ColliderType a_type, GameObject* a_parent, glm::vec3 a_extents);
	~Collider();

	glm::vec4 GetCenter();
	glm::vec3 GetExtents();

	void debug();

private:
	ColliderType m_type;
	glm::vec4* m_center;
	glm::vec3 m_extents;
};

