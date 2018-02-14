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
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, bool a_isStatic);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();

	void applyForce(glm::vec2 force, FORCEMODE a_mode);
	void applyForceToActor(RigidBody* actor2, glm::vec2 force, FORCEMODE a_mode_1, FORCEMODE a_mode_2);

	virtual bool checkCollision(PhysicsObject* pOther) = 0;

	glm::vec2 getPosition()					{ return m_position; }
	glm::vec2 getVelocity()					{ return m_velocity; }
	glm::vec2 getForce()					{ return m_velocity * m_mass; }
	float getRotation()						{ return m_rotation; }
	float getMass()							{ return m_mass; }
	bool isStatic()							{ return m_static; }
	void setVelocity(glm::vec2 a_velocity)	{ m_velocity = a_velocity; }
	std::vector<glm::vec2> getVertices()	{ return m_vertices; }
	void setPosition(glm::vec2 a_pos)		{ m_position = a_pos; }
	void addVelocity(glm::vec2 a_velocity)	{ m_velocity += a_velocity; }
	void setForcemode(FORCEMODE a_mode)		{ forcemode = a_mode; }

protected:
	glm::vec2 m_position;
	glm::vec2 m_originalPosition;
	glm::vec2 m_velocity;
	glm::vec2 m_acceleration;
	float m_rotation;
	float m_mass;
	bool m_static;
	FORCEMODE forcemode;
	float m_linearDrag;
	float m_angularDrag;

	std::vector<glm::vec2> m_vertices;
};

