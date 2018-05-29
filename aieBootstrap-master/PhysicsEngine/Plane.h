#pragma once
#include "PhysicsObject.h"
class Plane : public PhysicsObject
{
public:
	Plane();
	Plane(glm::vec2 normal, float distance);
	~Plane();

	virtual void fixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void debug() {}
	virtual void makeGizmo();
	virtual void resetPosition();

	glm::vec2 getNormal()		{ return m_normal; }
	float getDistance()			{ return m_distanceToOrigin; }
	glm::vec2 getEdge1()		{ return m_edge1; }
	glm::vec2 getEdge2()		{ return m_edge2; }

protected:
	glm::vec2 m_normal;
	float m_distanceToOrigin;
	glm::vec2 m_edge1;
	glm::vec2 m_edge2;
};

