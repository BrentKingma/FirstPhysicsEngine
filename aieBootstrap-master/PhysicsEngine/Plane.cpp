#include "Plane.h"

Plane::Plane() : PhysicsObject(ShapeType::PLANE)
, m_normal(0, 0)
, m_distanceToOrigin(0)
{}
Plane::Plane(glm::vec2 normal, float distance, glm::vec2 a_screenSize) : PhysicsObject(ShapeType::PLANE)
, m_normal(normal)
, m_distanceToOrigin(distance)
, screenSize(a_screenSize)
{
	glm::vec2 perp = { m_normal.y, -m_normal.x };
	float lineLength = 5000;
	glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
	m_edge1 = centerPoint + (perp * lineLength);
	m_edge2 = centerPoint - (perp * lineLength);
}
Plane::~Plane()
{}

void Plane::fixedUpdate(glm::vec2 a_gravity, float timeStep)
{

}
void Plane::resetPosition()
{
	
}

void Plane::makeGizmo()
{
	glm::vec4 colour(1, 1, 1, 1);

	aie::Gizmos::add2DLine(m_edge1, m_edge2, colour);
}
