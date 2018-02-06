#include "Plane.h"

Plane::Plane(glm::vec2 normal, float distance) : PhysicsObject(ShapeType::PLANE)
, m_normal(normal)
, m_distanceToOrigin(distance)
{}
Plane::~Plane()
{}

void Plane::makeGizmo()
{
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
	glm::vec4 colour(1, 1, 1, 1);

	aie::Gizmos::add2DLine(start, end, colour);
}