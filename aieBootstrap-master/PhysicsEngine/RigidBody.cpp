#include "RigidBody.h"
#include <iostream>

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, bool a_isStatic)
:	PhysicsObject(shapeID)
,	m_acceleration(0,0)
,	m_position(position)
,	m_mass(mass)
,	m_velocity(velocity)
,	m_rotation(rotation)
,	m_originalPosition(position)
,	m_static(a_isStatic)
{}
RigidBody::~RigidBody()
{
	delete this;
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (gravity != glm::vec2(0.0f, 0.0f))
	{
		applyForce(gravity * m_mass);
	}
	m_velocity += m_acceleration * timeStep;
	m_position += m_velocity * timeStep;

	m_acceleration = { 0, 0 };
}
void RigidBody::debug()
{
	
}

void RigidBody::applyForce(glm::vec2 force)
{
	if (!m_static)
	{
		m_velocity += force / m_mass;
	}
}
void RigidBody::applyForceToActor(RigidBody * actor2, glm::vec2 force)
{
	applyForce(force);
	actor2->applyForce(-force);
}