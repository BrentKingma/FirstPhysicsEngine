#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Gizmos.h>

enum ShapeType
{
	PLANE = 0,
	SPHERE,
	BOX,
	TOTAL
};
class PhysicsObject
{
protected:
	PhysicsObject() = delete;
	PhysicsObject(ShapeType a_shapeID) : m_shapeID(a_shapeID) {}

public:
	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void debug() = 0;
	virtual void resetPosition() {};

	ShapeType getShapeType() { return m_shapeID; }

protected:
	ShapeType m_shapeID;
};

