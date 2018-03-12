#pragma once
#include "RigidBody.h"
#include <vector>
class AABB : public RigidBody
{
public:
	AABB(glm::vec2 a_center, glm::vec2 a_extends, glm::vec2 a_velocity, float a_rotation, float a_mass, bool a_isStatic);
	~AABB();

	virtual void makeGizmo();

#pragma region Getters
	glm::vec2 GetExtends()		{ return m_extends; }
	glm::vec2 GetTopRight()		{ return { m_position.x + m_extends.x, m_position.y + m_extends.y }; }
	glm::vec2 GetBottomLeft()	{ return { m_position.x - m_extends.x, m_position.y - m_extends.y }; }
	float GetWidth()			{ return m_extends.x; }
	float GetHeight()			{ return m_extends.y; }

	std::vector<glm::vec2> GetPointList() { return m_PointList; }
#pragma endregion


	



protected:
	glm::vec2 m_extends;
	//The x and y of the top right corner
	glm::vec2 m_topRight;
	//The x and y of the bottom left corner
	glm::vec2 m_bottomLeft;

	//-x, y
	glm::vec2 m_Point_1;
	//x, y
	glm::vec2 m_Point_2;
	//-x, -y
	glm::vec2 m_Point_3;
	//x, -y
	glm::vec2 m_Point_4;

	std::vector<glm::vec2> m_PointList;
};

