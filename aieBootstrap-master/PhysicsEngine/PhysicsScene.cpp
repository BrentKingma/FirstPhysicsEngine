#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "RigidBody.h"
#include "Sphere.h"
#include "Plane.h"
#include "AABB.h"

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

		checkForCollision();

		//for (auto pActor : m_actors)
		//{
		//	for (auto pOther : m_actors)
		//	{
		//		//if the 2 rigidbodies are the same, skip this iterration ( pOther moves on to the next one)
		//		if (pActor == pOther)
		//		{
		//			continue;
		//		}
		//		//After first run dirty will start being populated, this is a check to make sure that we dont check the same objects again.
		//		if (std::find(checkedObjects.begin(), checkedObjects.end(), pActor) != checkedObjects.end() &&
		//			std::find(checkedObjects.begin(), checkedObjects.end(), pOther) != checkedObjects.end())
		//		{
		//			continue;
		//		}

		//		RigidBody* pRigid = dynamic_cast<RigidBody*>(pActor);
		//		if (pRigid->checkCollision(pOther) == true)
		//		{
		//			std::cout << "Collision";
		//			
		//			checkedObjects.push_back(pRigid);
		//			checkedObjects.push_back(pOther);
		//		}
		//	}
		//}
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
	nullptr,					PhysicsScene::plane2Sphere,		PhysicsScene::plane2AABB,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,	PhysicsScene::sphere2AABB,
	PhysicsScene::AABB2Plane,	PhysicsScene::AABB2Sphere,		PhysicsScene::AABB2AABB
};
void PhysicsScene::checkForCollision()
{
	int actorCount = m_actors.size();

	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			//Creates pointers to the 2 objects being checked for collions
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];

			int shapeID1 = object1->getShapeType();
			int shapeID2 = object2->getShapeType();

			int functionIDx = (shapeID1 * (ShapeType::TOTAL)) + shapeID2;
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

		float sphere1X = sphere1->getPosition().x;
		float sphere1Y = sphere1->getPosition().y;
		float sphere2X = sphere2->getPosition().x;
		float sphere2Y = sphere2->getPosition().y;

		float xSqaured = (sphere2X - sphere1X)*(sphere2X - sphere1X);
		float ySquared = (sphere2Y - sphere1Y)*(sphere2Y - sphere1Y);

		if ((xSqaured + ySquared) < radius)
		{
			sphere1->setVelocity(glm::vec2(0.0f, 0.0f));
			sphere2->setVelocity(glm::vec2(0.0f, 0.0f));
			return true;
		}
	}
	return false;
}
bool PhysicsScene::sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object1);
	Plane* plane = dynamic_cast<Plane*>(object2);

	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		//if we are behind the plane the normal gets flipped
		if (sphereToPlane < 0)
		{
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0)
		{
			sphere->setVelocity(glm::vec2(0.0f, 0.0f));
			return true;
		}
	}
	return false;
}
bool PhysicsScene::sphere2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere =	dynamic_cast<Sphere*>(object1);
	AABB* aabb =		dynamic_cast<AABB*>(object2);

	// Get the center of the sphere relative to the center of the box
	glm::vec2 sphereCenterRelBox = sphere->getPosition() - aabb->getPosition();
	// Point on surface of box that is closest to the center of the sphere
	glm::vec2 boxPoint;

	// Check sphere center against box along the X axis alone. 
	// If the sphere is off past the left edge of the box, 
	// then the left edge is closest to the sphere. 
	// Similar if it's past the right edge. If it's between 
	// the left and right edges, then the sphere's own X 
	// is closest, because that makes the X distance 0, 
	// and you can't get much closer than that :)

	if (sphereCenterRelBox.x < -aabb->GetWidth())
	{
		boxPoint.x = -aabb->GetWidth();
	}
	else if (sphereCenterRelBox.x > aabb->GetWidth())
	{
		boxPoint.x = aabb->GetWidth();
	}
	else
	{
		boxPoint.x = sphereCenterRelBox.x;
	}

	// ...same for Y axis
	if (sphereCenterRelBox.y < -aabb->GetHeight())
	{
		boxPoint.y = -aabb->GetHeight();
	}		
	else if (sphereCenterRelBox.y > aabb->GetHeight())
	{
		boxPoint.y = aabb->GetHeight();
	}
	else
	{
		boxPoint.y = sphereCenterRelBox.y;
	}
	// Now we have the closest point on the box, so get the distance from 
	// that to the sphere center, and see if it's less than the radius

	glm::vec2 dist = sphereCenterRelBox - boxPoint;

	if (dist.x*dist.x + dist.y*dist.y < sphere->getRadius()*sphere->getRadius())
	{
		sphere->setVelocity(glm::vec2(0.0f, 0.0f));
		aabb->setVelocity(glm::vec2(0.0f, 0.0f));
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsScene::plane2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object2);
	Plane* plane = dynamic_cast<Plane*>(object1);

	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(plane->getNormal(), sphere->getPosition()) - plane->getDistance();

		//if we are behind the plane the normal gets flipped
		if (sphereToPlane < 0)
		{
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0)
		{
			sphere->setVelocity(glm::vec2(0.0f, 0.0f));
			return true;
		}
	}
	return false;
}
bool PhysicsScene::plane2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Plane* plane = dynamic_cast<Plane*>(object1);
	AABB* aabb = dynamic_cast<AABB*>(object2);
	
	float radius = aabb->GetExtends().x * abs(plane->getNormal().x) + aabb->GetExtends().y * abs(plane->getNormal().y);
	float distance = glm::dot(plane->getNormal(), aabb->getPosition()) - plane->getDistance();

	if (distance <= radius)
	{
		aabb->setVelocity(glm::vec2(0.0f, 0.0f));
	}
	return false;
}
bool PhysicsScene::AABB2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
bool PhysicsScene::AABB2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	AABB* aabb = dynamic_cast<AABB*>(object1);
	Plane* plane = dynamic_cast<Plane*>(object2);

	int indexOfClosestPoint = 0;
	float currentDistance = 100000.0f;
	//Loopthrough all points on the aabb
	for (int i = 0; i < 4; i++)
	{
		//Plane normal
		glm::vec2 n = plane->getNormal();
		//
		glm::vec2 p0 = (plane->getNormal() * plane->getDistance());
		glm::vec2 l0 = aabb->GetPointList()[i];
		glm::vec2 l = glm::normalize(aabb->getVelocity() + aabb->GetPointList()[i]);

		//glm::vec2 dist = (((plane->getNormal() * plane->getDistance()) - aabb->GetPointList()[i]) * plane->getNormal() / vecTowards * plane->getNormal());
		float denom = glm::dot(n, l);
		if (denom > 1e-6)
		{
			glm::vec2 p0l0 = ((plane->getNormal() * plane->getDistance()) - aabb->GetPointList()[i]);
			float dist = glm::dot(p0l0, plane->getNormal()) / denom;

			if (dist < currentDistance)
			{
				currentDistance = dist;
				indexOfClosestPoint = i;
			}
		}		
	}

	if (currentDistance <= 0.0f)
	{
		aabb->setVelocity(glm::vec2(0.0f, 0.0f));
		return true;
	}
	return false;
}
bool PhysicsScene::AABB2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

bool PhysicsScene::SATCollision(RigidBody * object1, RigidBody * object2)
{
	std::vector<glm::vec2> shadowAxis;
	std::vector<glm::vec2> allVerts;
	if (object1->getShapeType() != ShapeType::SPHERE)
	{	
		for (int i = 0; i < object1->getVertices().size - 1; i++)
		{
			glm::vec2 vecNormalised = glm::normalize((object1->getVertices()[i + 1] - object1->getVertices()[i]));
			glm::vec2 vecPerpendicular = glm::vec2(vecNormalised.y, -vecNormalised.x);

			shadowAxis.push_back(vecPerpendicular);
		}
	}
	if (object2->getShapeType() != ShapeType::SPHERE)
	{
		for (int i = 0; i < object2->getVertices().size - 1; i++)
		{
			glm::vec2 vecNormalised = glm::normalize((object1->getVertices()[i + 1] - object1->getVertices()[i]));
			glm::vec2 vecPerpendicular = glm::vec2(vecNormalised.y, -vecNormalised.x);

			shadowAxis.push_back(vecPerpendicular);
		}
	}

	int totalVerts = object1->getVertices().size + object2->getVertices().size;




	return false;
}
