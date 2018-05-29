#include "RigidBody.h"
#include <iostream>

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, glm::vec4 a_color, float rotation, float mass, bool a_isStatic)
:	PhysicsObject(shapeID)
,	m_acceleration(0.0f, 0.0f)
,	m_position(position)
,	m_mass(mass)
,	m_velocity(velocity)
,	m_rotation(rotation)
,	m_originalPosition(position)
,	m_static(a_isStatic)
,	m_angularVelocity(0.0f)
,	m_linearDrag(0.01f)
,   m_angularDrag(0.0001f)
,	m_elasticity(0.7f)
,	m_moment(0.0f)
,	m_color(a_color)
{}
RigidBody::~RigidBody()
{
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (glm::length(m_velocity) < MIN_LINEAR_THERSHOLD)
	{
		m_velocity = glm::vec2{ 0.0f, 0.0f };
	}
	//if (glm::abs(m_angularVelocity) < MIN_ANGULAR_THERSHOLD)
	//{
	//	m_angularVelocity = 0.0f;
	//}
	if (gravity != glm::vec2(0.0f, 0.0f))
	{
		applyLinearForce(gravity * m_mass, FORCEMODE::CONSTANT);
	}

	m_velocity += m_acceleration * timeStep;
	m_position += m_velocity * timeStep;

	m_velocity -= m_velocity * m_linearDrag * timeStep;
	m_rotation += m_angularVelocity * timeStep;
	m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;
	
	m_acceleration = { 0, 0 };
}
void RigidBody::debug()
{
	
}

void RigidBody::applyLinearForce(glm::vec2 force, FORCEMODE a_mode)
{
	if (!m_static)
	{
		if (a_mode == FORCEMODE::CONSTANT)
		{
			m_acceleration += force / m_mass;
		}
		else if (a_mode == FORCEMODE::IMPULSE)
		{
			m_velocity += force / m_mass;
		}
	}
}
void RigidBody::applyRotationalForce(glm::vec2 force, glm::vec2 pos)
{
	if (!m_static)
	{
		m_angularVelocity += (force.y * pos.x - force.x * pos.y) / (m_moment);
	}
}