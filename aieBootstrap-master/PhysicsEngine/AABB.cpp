#include "AABB.h"



AABB::AABB(glm::vec2 a_center, glm::vec2 a_extends, glm::vec2 a_velocity, float a_rotation, float a_mass, float a_elasticity, bool a_isStatic)
	: RigidBody(ShapeType::BOX, a_center, a_velocity, a_rotation, a_mass, a_elasticity, a_isStatic)
{
	m_extends = a_extends;
	m_topRight =	glm::vec2(a_center.x + a_extends.x, a_center.y + a_extends.y);
	m_bottomLeft =	glm::vec2(a_center.x - a_extends.x, a_center.y - a_extends.y);

	m_Point_1.x = m_position.x - m_extends.x;
	m_Point_1.y = m_position.y + m_extends.y;

	m_Point_2.x = m_position.x + m_extends.x;
	m_Point_2.y = m_position.y + m_extends.y;

	m_Point_3.x = m_position.x - m_extends.x;
	m_Point_3.y = m_position.y - m_extends.y;

	m_Point_4.x = m_position.x + m_extends.x;
	m_Point_4.y = m_position.y - m_extends.y;

	m_PointList.push_back(m_Point_1);
	m_PointList.push_back(m_Point_2);
	m_PointList.push_back(m_Point_3);
	m_PointList.push_back(m_Point_4);

	m_moment = 1.0f / 12.0f * m_mass * m_extends.x * m_extends.y;
}


AABB::~AABB()
{
}

void AABB::makeGizmo()
{
	aie::Gizmos::add2DAABB(m_position, m_extends, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}

bool AABB::checkCollision(PhysicsObject * pOther)
{
	return false;
}
