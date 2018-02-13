#pragma once
#include <glm/glm.hpp>
#include <vector>

class PhysicsObject;
class RigidBody;

struct CollisionData
{
	CollisionData()
		:collision(false)
		,overlap(0.0f)
		,normal(glm::vec2(0.0f, 0.0f))
	{}
	CollisionData(bool a_collision, float a_overlap, glm::vec2 a_normal)
		:collision(a_collision)
		,overlap(a_overlap)
		,normal(a_normal)
	{}
	CollisionData(bool a_collision)
		:collision(a_collision)
		,overlap(0.0f)
		,normal(glm::vec2(0.0f, 0.0f))
	{}
	bool collision;
	float overlap;
	glm::vec2 normal;
};

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

	static CollisionData plane2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData plane2AABB(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData sphere2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData sphere2AABB(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData AABB2AABB(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData AABB2Plane(PhysicsObject* object1, PhysicsObject* object2);
	static CollisionData AABB2Sphere(PhysicsObject* object1, PhysicsObject* object2);

	bool SATCollision(RigidBody* object1, RigidBody* object2);

	void checkForCollision();
	void seperateCollision(RigidBody* object1, RigidBody* object2, CollisionData collision);
	void seperateCollision(PhysicsObject* object1, PhysicsObject* object2, CollisionData collision);
	void resolveCollisionWithoutPlane(RigidBody* object1, RigidBody* object2);
	void resolveCollisionWithPlane(PhysicsObject* object1, PhysicsObject* object2);

protected:
	glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;

	CollisionData collision;
};

