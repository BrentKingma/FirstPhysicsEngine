#include "Sphere.h"
#include <cmath>
#include <algorithm>
Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool a_isStatic)
	: RigidBody(ShapeType::SPHERE, position, velocity, colour, 0.0f, mass, a_isStatic)
	, m_radius(radius)
{
	m_vertex_1.x = m_position.x;
	m_vertex_1.y = m_position.y;

	m_vertices.push_back(m_vertex_1);

	if (!m_static)
	{
		m_moment = 0.5f * m_mass * (m_radius * m_radius);
	}
	m_centreOfMass = m_position;
}

Sphere::~Sphere()
{
	delete this;
}

void Sphere::debug()
{
	//aie::Gizmos::add2DCircle(m_position, m_radius, 32, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
}
