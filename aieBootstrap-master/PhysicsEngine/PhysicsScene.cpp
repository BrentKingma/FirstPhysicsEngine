#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "Sphere.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <list>

PhysicsScene::PhysicsScene()
	: m_timeStep(0.01f)
	, m_gravity(0.0f, -9.8f)
{
	
}
PhysicsScene::~PhysicsScene()
{
	for (auto& actor : m_actors)
	{
		delete actor;
	}
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	assert(actor != nullptr);
	assert(std::find(std::begin(m_actors), std::end(m_actors), actor) == m_actors.end());
	m_actors.push_back(actor);
}
void PhysicsScene::removeActor(PhysicsObject* actor)
{
	std::remove(std::begin(m_actors), std::end(m_actors), actor);
}
void PhysicsScene::update(float deltaTime)
{
	static std::list<PhysicsObject*> checkedObjects;

	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;

		for (auto pActor : m_actors)
		{
			for (auto pOther : m_actors)
			{
				//if the 2 rigidbodies are the same, skip this iterration ( pOther moves on to the next one)
				if (pActor == pOther)
				{
					continue;
				}
				//After first run dirty will start being populated, this is a check to make sure that we dont check the same objects again.
				if (std::find(checkedObjects.begin(), checkedObjects.end(), pActor) != checkedObjects.end() &&
					std::find(checkedObjects.begin(), checkedObjects.end(), pOther) != checkedObjects.end())
				{
					continue;
				}

				RigidBody* pRigid = dynamic_cast<RigidBody*>(pActor);
				if (pRigid->checkCollision(pOther) == true)
				{
					std::cout << "Collision";
					pRigid->applyForceToActor(dynamic_cast<RigidBody*>(pOther), pRigid->getForce() * 1000.0f);
					checkedObjects.push_back(pRigid);
					checkedObjects.push_back(pOther);
				}
			}
		}
		checkedObjects.clear();
	}
}
void PhysicsScene::updateGimozs()
{
	for (auto pActor : m_actors)
	{
		pActor->makeGizmo();
	}
}

// function pointer array for doing our collisions 
typedef bool(*fn)(PhysicsObject*, PhysicsObject*); 
static fn collisionFunctionArray[] = 
{	
	PhysicsScene::plane2Plane, PhysicsScene::plane2Sphere,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere, 
};
void PhysicsScene::checkForCollision()
{
	int actorCount = m_actors.size();

	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];

			int shapeID1 = object1->getShapeType();
			int shapeID2 = object2->getShapeType();

			int functionIDx = (shapeID1 * ShapeType::TOTAL) + shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIDx];
			if (collisionFunctionPtr != nullptr)
			{
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}
bool PhysicsScene::sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere* sphere2 = dynamic_cast<Sphere*>(object2);
	//if both objects could be casted to sphere continue
	if (sphere1 != nullptr && sphere2 != nullptr)
	{
		float radius = sphere1->getRadius() + sphere2->getRadius();
		radius *= radius;
		float xSqaured = (m_position.x - otherSphere->m_position.x)*(m_position.x - otherSphere->m_position.x);
		float ySquared = (m_position.y - otherSphere->m_position.y)*(m_position.y - otherSphere->m_position.y);

		if ((xSqaured + ySquared) < radius)
		{
			return true;
		}
	}
}
