#include "RigidBody.h"
#include <iostream>

RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float a_elasticity, bool a_isStatic)
:	PhysicsObject(shapeID)
,	m_acceleration(0,0)
,	m_position(position)
,	m_mass(mass)
,	m_velocity(velocity)
,	m_rotation(rotation)
,	m_originalPosition(position)
,	m_static(a_isStatic)
,	m_elasticity(a_elasticity)
{
	m_angularVelocity = 0.0f;
}
RigidBody::RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float a_elasticity, bool a_isStatic, float a_linearDrag, float a_angularDrag)
:	PhysicsObject(shapeID)
,	m_acceleration(0, 0)
,	m_position(position)
,	m_mass(mass)
,	m_velocity(velocity)
,	m_rotation(rotation)
,	m_originalPosition(position)
,	m_static(a_isStatic)
,	m_linearDrag(a_linearDrag)
,	m_angularDrag(a_angularDrag)
,	m_elasticity(a_elasticity)
{
	m_angularVelocity = 0.0f;
}
RigidBody::~RigidBody()
{
	delete this;
}

void RigidBody::fixedUpdate(glm::vec2 gravity, float timeStep)
{
	if (gravity != glm::vec2(0.0f, 0.0f))
	{
		applyForce(gravity * m_mass, FORCEMODE::CONSTANT);
	}
	m_velocity += m_acceleration * timeStep;
	m_position += m_velocity * timeStep;

	m_velocity -= m_velocity * m_linearDrag * timeStep;
	m_rotation += m_angularVelocity * timeStep;
	m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;

	/*if (glm::length(m_velocity) < MIN_LINEAR_THRESHOLD)
	{
		m_velocity = glm::vec2(0.0f, 0.0f);
	}*/
	if (std::abs(m_angularVelocity) < MIN_ROTATION_THRESHOLD)
	{
		m_angularVelocity = 0.0f;
	}

	m_acceleration = { 0, 0 };
}
void RigidBody::debug()
{
	
}

void RigidBody::applyForce(glm::vec2 a_force, FORCEMODE a_mode, glm::vec2 pos)
{
	if (!m_static)
	{
		if (a_mode == FORCEMODE::CONSTANT)
		{
			m_acceleration += a_force / m_mass;
		}
		else if (a_mode == FORCEMODE::IMPULSE)
		{
			m_velocity += a_force / m_mass;
			if ((a_force.y * pos.x - a_force.x * pos.y) == 0.0f)
			{
				m_angularVelocity += 0.0f;
			}

			m_angularVelocity += (a_force.y * pos.x - a_force.x * pos.y) / m_moment;
		}
	}
}
//void RigidBody::applyForceToActor(RigidBody * actor2, glm::vec2 force, FORCEMODE a_mode_1, FORCEMODE a_mode_2)
//{
//	applyForce(force, a_mode_1);
//	actor2->applyForce(-force, a_mode_2);
//}