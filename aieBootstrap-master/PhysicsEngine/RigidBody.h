#pragma once
#include "PhysicsObject.h"
#include <vector>
class RigidBody : public PhysicsObject
{
public:
	RigidBody() = delete;
	RigidBody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, float rotation, float mass, bool a_isStatic);
	~RigidBody();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug();

	void applyForce(glm::vec2 force);
	void applyForceToActor(RigidBody* actor2, glm::vec2 force);

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

protected:
	glm::vec2 m_position;
	glm::vec2 m_originalPosition;
	glm::vec2 m_velocity;
	glm::vec2 m_acceleration;
	float m_rotation;
	float m_mass;
	bool m_static;

	std::vector<glm::vec2> m_vertices;
};

