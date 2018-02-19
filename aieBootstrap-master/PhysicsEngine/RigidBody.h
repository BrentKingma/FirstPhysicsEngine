#pragma once
#include "PhysicsObject.h"
#include <vector>

enum FORCEMODE
{
	CONSTANT,
	IMPULSE
};

class RigidBody : public PhysicsObject
{
public:
	RigidBody() = delete;
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float a_elasticity, bool a_isStatic);
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, float a_elasticity, bool a_isStatic, float a_linearDrag, float a_angularDrag);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();

	void applyForce(glm::vec2 force, FORCEMODE a_mode, glm::vec2 pos = { 0.0f, 0.0f });
	//void applyForceToActor(RigidBody* actor2, glm::vec2 force, FORCEMODE a_mode_1, FORCEMODE a_mode_2, glm::vec2 pos);

	virtual bool checkCollision(PhysicsObject* pOther) = 0;

	glm::vec2 getPosition()					{ return m_position; }
	glm::vec2 getVelocity()					{ return m_velocity; }
	glm::vec2 getForce()					{ return m_velocity * m_mass; }

	float getRotation()						{ return m_rotation; }
	float getMass()							{ return m_mass; }
	float getElasticity()					{ return m_elasticity; }
	float getMoment()						{ return m_moment; }

	bool isStatic()							{ return m_static; }
	
	std::vector<glm::vec2> getVertices()	{ return m_vertices; }

	void setVelocity(glm::vec2 a_velocity) { m_velocity = a_velocity; }
	void setPosition(glm::vec2 a_pos)		{ m_position = a_pos; }
	void addVelocity(glm::vec2 a_velocity)	{ m_velocity += a_velocity; }
	void setForcemode(FORCEMODE a_mode)		{ forcemode = a_mode; }

protected:
	glm::vec2 m_position;
	glm::vec2 m_originalPosition;
	glm::vec2 m_velocity;
	glm::vec2 m_acceleration;
	float m_angularVelocity;
	float m_rotation;
	float m_moment;
	float m_mass;
	float m_elasticity;
	bool m_static;
	FORCEMODE forcemode;
	float m_linearDrag;
	float m_angularDrag;

	std::vector<glm::vec2> m_vertices;

	const float MIN_LINEAR_THRESHOLD = 0.1f;
	const float MIN_ROTATION_THRESHOLD = 0.01f;
};

