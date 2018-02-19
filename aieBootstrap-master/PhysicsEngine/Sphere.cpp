#include "Sphere.h"
#include <cmath>
#include <algorithm>
Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, float a_elasticity, glm::vec4 colour, bool a_isStatic, float a_linearDrag, float a_angularDrag)
	: RigidBody(ShapeType::SPHERE, position, velocity, 0.0f, mass, a_elasticity, a_isStatic, a_linearDrag, a_angularDrag)
	, m_radius(radius)
	, m_colour(colour)
{
	m_vertex_1.x = m_position.x;
	m_vertex_1.y = m_position.y;

	m_vertices.push_back(m_vertex_1);

	m_moment = 0.5f * m_mass * m_radius * m_radius;
}

Sphere::~Sphere()
{
}

void Sphere::makeGizmo()
{
	aie::Gizmos::add2DCircle(m_position, m_radius, 32, m_colour);
}
//Not beign used
bool Sphere::checkCollision(PhysicsObject * pOther)
{
	assert(pOther != nullptr);
	Sphere* otherSphere = dynamic_cast<Sphere*>(pOther);
	assert(otherSphere != nullptr);

	//Multiplication is less expensive
	//Gets the radius and squares it
	//Gets the x component and squares it, gets the y component and squares it, adds them
	//If radius sqaured is less than x and y component squared, true else false
	float radius = m_radius + otherSphere->m_radius;
	radius *= radius;
	float xSqaured = (m_position.x - otherSphere->m_position.x)*(m_position.x - otherSphere->m_position.x);
	float ySquared = (m_position.y - otherSphere->m_position.y)*(m_position.y - otherSphere->m_position.y);

	if ((xSqaured + ySquared) < radius)
	{
		return true;
	}
	return false;
}
