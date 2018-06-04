#pragma once
#include <glm/glm.hpp>
class GameObject;
class BoundingBox
{
public:
	BoundingBox() = delete;
	BoundingBox(GameObject* a_parent, glm::vec3 a_extents);
	~BoundingBox();

	glm::vec4* GetCenter();
	glm::vec3 GetExtents();

	void debug();

private:
	//Bouding box and circle 
	glm::vec4* m_boundingBox;
	//Extents
	glm::vec3 m_extents;
};

