#pragma once
#include "PhysicsObject.h"
#include <vector>

#define MIN_LINEAR_THERSHOLD 0.01f
#define MIN_ANGULAR_THERSHOLD 0.001f

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

	void applyLinearForce(glm::vec2 force, FORCEMODE a_mode);
	void applyRotationalForce(glm::vec2 force, glm::vec2 pos = { 0.0f, 0.0f });

	glm::vec2 getPosition()					{ return m_position; }
	glm::vec2 getVelocity()					{ return m_velocity; }
	glm::vec2 getForce()					{ return m_velocity * m_mass; }
	glm::vec2 getCentreOfMass()				{ return m_centreOfMass; }

	float getAngularVelocity()				{ return m_angularVelocity; }
	float getRotation()						{ return m_rotation; }
	float getMass()							{ return m_mass; }
	float getElasticity()					{ return m_elasticity; }
	float getMoment()						{ return m_moment; }
	float* getMassAddress()					{ return &m_mass; }
	float* getElasticityAddress()			{ return &m_elasticity; }

	bool isStatic()							{ return m_static; }
	
	std::vector<glm::vec2> getVertices()	{ return m_vertices; }

	void setVelocity(glm::vec2 a_velocity)  { m_velocity = a_velocity; }
	void setPosition(glm::vec2 a_pos)		{ m_position = a_pos; }
	void addVelocity(glm::vec2 a_velocity)	{ m_velocity += a_velocity; }
	void setForcemode(FORCEMODE a_mode)		{ forcemode = a_mode; }
	void setElasticity(float a_elasticity)	{ m_elasticity = a_elasticity; }

protected:
	glm::vec2 m_position;
	glm::vec2 m_originalPosition;
	glm::vec2 m_velocity;
	glm::vec2 m_acceleration;
	glm::vec2 m_centreOfMass;
	
	float m_angularVelocity;
	float m_moment;
	float m_rotation;
	float m_mass;
	float m_linearDrag;
	float m_angularDrag;
	float m_elasticity;

	bool m_static;

	FORCEMODE forcemode;	

	std::vector<glm::vec2> m_vertices;
};

