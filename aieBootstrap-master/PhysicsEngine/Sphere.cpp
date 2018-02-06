#include "Sphere.h"
#include <cmath>
#include <algorithm>
Sphere::Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, glm::vec4 colour, bool isKinematic)
	: RigidBody(ShapeType::SPHERE, position, velocity, 0.0f, mass, isKinematic)
	, m_radius(radius)
	, m_colour(colour)
{}

Sphere::~Sphere()
{
}

void Sphere::makeGizmo()
{
	aie::Gizmos::add2DCircle(m_position, m_radius, 32, m_colour);
}
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
