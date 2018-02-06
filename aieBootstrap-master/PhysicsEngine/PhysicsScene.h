#pragma once
#include <glm/glm.hpp>
#include <vector>

class PhysicsObject;
class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void addActor(PhysicsObject* actor);
	void removeActor(PhysicsObject* actor);
	void update(float deltaTime);
	void updateGimozs();

	void setGravity(const glm::vec2 gravity) { m_gravity = gravity;	}
	glm::vec2 getGravity() const { return m_gravity; }

	void setTimeStep(const float timeStep) { m_timeStep = timeStep; }
	float getTimeStep() const { return m_timeStep; }

	static bool plane2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static bool plane2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static bool sphere2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static bool sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2);

	void checkForCollision();

protected:
	glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;
};

