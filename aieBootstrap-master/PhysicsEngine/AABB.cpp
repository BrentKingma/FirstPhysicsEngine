#include "AABB.h"



AABB::AABB(glm::vec2 a_center, glm::vec2 a_extends, glm::vec2 a_velocity, float a_rotation, float a_mass, bool a_isStatic)
	: RigidBody(ShapeType::BOX, a_center, a_velocity, a_rotation, a_mass, a_isStatic)
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

	m_vertices.push_back(m_Point_1);
	m_vertices.push_back(m_Point_2);
	m_vertices.push_back(m_Point_3);
	m_vertices.push_back(m_Point_4);	
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
