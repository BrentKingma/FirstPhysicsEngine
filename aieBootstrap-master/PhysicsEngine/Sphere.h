#pragma once
#include "RigidBody.h"
#include <glm/glm.hpp>
class Sphere :	public RigidBody
{
public:
	Sphere() = delete;
	Sphere(glm::vec2 position, glm::vec2 velocity, float mass, float radius, float a_elasticity, glm::vec4 colour, bool a_isStatic, float a_linearDrag, float a_angularDrag);
	~Sphere();

	virtual void makeGizmo();
	virtual bool checkCollision(PhysicsObject* pOther);

	float getRadius()		{ return m_radius; }
	glm::vec4 getColour()	{ return m_colour; }

protected:
	float m_radius;
	glm::vec4 m_colour;
	glm::vec2 m_vertex_1;
};

