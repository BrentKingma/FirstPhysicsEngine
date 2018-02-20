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
	, collision(false)
{
	
}
PhysicsScene::~PhysicsScene()
{
	for (auto& actor : m_actors)
	{
		delete actor;
	}
}

// function pointer array for doing our collisions 
typedef CollisionData(*fn)(PhysicsObject*, PhysicsObject*);
static fn collisionFunctionArray[] = {
	nullptr,					PhysicsScene::plane2Sphere,		PhysicsScene::plane2AABB,
	PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,	PhysicsScene::sphere2AABB,
	PhysicsScene::AABB2Plane,	PhysicsScene::AABB2Sphere,		PhysicsScene::AABB2AABB
};

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
	}
}
void PhysicsScene::updateGimozs()
{
	for (auto pActor : m_actors)
	{
		pActor->makeGizmo();
	}
}
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
				collision = collisionFunctionPtr(object1, object2);
				if (collision.collision)
				{
					
					//Checks if there is no plane involved in the collision
					//Then seperates the two objects from each other
					//Then resolves the collision
					if (dynamic_cast<Plane*>(object1) == nullptr && dynamic_cast<Plane*>(object2) == nullptr)
					{
						seperateCollision(dynamic_cast<RigidBody*>(object1), dynamic_cast<RigidBody*>(object2), collision);
						resolveCollision(dynamic_cast<RigidBody*>(object1), dynamic_cast<RigidBody*>(object2));
					}
					else
					{
						seperateCollision(object1, object2, collision);
						resolveCollision(object1, object2);						
					}
				}

			}
		}
	}
}
void PhysicsScene::seperateCollision(RigidBody * object1, RigidBody * object2, CollisionData collision)
{
	float obj1MoveRatio = 0;
	float obj2MoveRatio = 0;

	if (!object1->isStatic() && !object2->isStatic())
	{
		float totalMass = object1->getMass() + object2->getMass();
		obj1MoveRatio = 1 - (object1->getMass() / totalMass);
		obj2MoveRatio = 1 - (object2->getMass() / totalMass);
	}
	else if (!object1->isStatic())
	{
		obj1MoveRatio = 1;
	}
	else
	{
		obj2MoveRatio = 1;
	}

	if (!object1->isStatic())
	{
		object1->setPosition(object1->getPosition() - (obj1MoveRatio * collision.overlap * collision.normal));
	}
	if (!object2->isStatic())
	{
		object2->setPosition(object2->getPosition() + (obj2MoveRatio * collision.overlap * collision.normal));
	}
}
void PhysicsScene::seperateCollision(PhysicsObject * object1, PhysicsObject * object2, CollisionData collision)
{
	RigidBody* rigid;
	if (dynamic_cast<Plane*>(object1) != nullptr)
	{
		rigid = (RigidBody*)object2;
	}
	else
	{
		rigid = (RigidBody*)object1;
	}
	float obj1MoveRatio = 1;

	if (!rigid->isStatic())
	{
		rigid->setPosition(rigid->getPosition() - (obj1MoveRatio * collision.overlap * collision.normal));
	}
}
void PhysicsScene::resolveCollision(RigidBody * object1, RigidBody * object2)
{
	//Perpendicular vector to the collision normal
	glm::vec2 perp(collision.normal.y, -collision.normal.x);

	float r1 = glm::dot(collision.contactPoint - object1->getPosition(), -perp);
	float r2 = glm::dot(collision.contactPoint - object2->getPosition(), perp);

	float v1 = glm::dot(object1->getVelocity(), collision.normal) - r1 * object1->getRotation();
	float v2 = glm::dot(object2->getVelocity(), collision.normal) + r2 * object2->getRotation();

	if (v1 > v2)
	{

		glm::vec2 normal = collision.normal;
		glm::vec2 relativeVelocity = object2->getVelocity() - object1->getVelocity();

		float mass1 = 1.0f / (1.0f / object1->getMass() + (r1*r1) / object1->getMoment());
		float mass2 = 1.0f / (1.0f / object2->getMass() + (r2*r2) / object2->getMoment());

		float elasticity = CalculateElasticity(object1, object2);

		/*float invMass1 = (!object1->isStatic()) ? 1.0f / object1->getMass() : 0.0f;
		float invMass2 = (!object2->isStatic()) ? 1.0f / object2->getMass() : 0.0f;
		float top = -(1 + elasticity) * glm::dot((relativeVelocity), normal);
		float bottom = (glm::dot(normal, normal) * (invMass1 + invMass2));
		float j = top / bottom;*/

		glm::vec2 force = (1.0f + elasticity)*mass1*mass2 / (mass1 + mass2)*(v1 - v2)*collision.normal;

		object1->applyForce(force, FORCEMODE::IMPULSE, collision.contactPoint - object1->getPosition());
		object2->applyForce(-force, FORCEMODE::IMPULSE, collision.contactPoint - object2->getPosition());
	}
}
void PhysicsScene::resolveCollision(PhysicsObject * a_object1, PhysicsObject * a_object2)
{
	Plane* plane;
	RigidBody* rigid;
	if (dynamic_cast<Plane*>(a_object1) != nullptr)
	{
		plane = (Plane*)a_object1;
		rigid = (RigidBody*)a_object2;
	}
	else
	{
		plane = (Plane*)a_object2;
		rigid = (RigidBody*)a_object1;
	}

	//Perpendicular vector to the collision normal
	glm::vec2 perp(collision.normal.y, -collision.normal.x);

	float r1 = glm::dot(collision.contactPoint - rigid->getPosition(), -perp);
	float v1 = glm::dot(rigid->getVelocity(), collision.normal) - r1 * rigid->getRotation();
	
	float mass1 = 1.0f / (1.0f / rigid->getMass() + (r1*r1) / rigid->getMoment());

	glm::vec2 normal = collision.normal;
	glm::vec2 relativeVelocity = rigid->getVelocity();

	float elasticity = CalculateElasticity(rigid);

	/*float invMass = (!rigid->isStatic()) ? 1.0f / rigid->getMass() : 0.0f;
	float top = (-(1 + elasticity) * glm::dot((relativeVelocity), normal));
	float j = top / invMass;*/

	glm::vec2 force = (1.0f + elasticity)*mass1 / (mass1)*(v1)*collision.normal;

	rigid->applyForce(force, FORCEMODE::IMPULSE, collision.contactPoint - rigid->getPosition());
}

float PhysicsScene::CalculateElasticity(RigidBody * object1, RigidBody * object2)
{
	if (object2 != nullptr)
	{
		return (object1->getElasticity() + object2->getElasticity()) / 2.0f;
	}
	else
	{
		return object1->getElasticity();
	}
}

CollisionData PhysicsScene::sphere2Sphere(PhysicsObject* object1, PhysicsObject* object2)
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
			float overlap = radius - (xSqaured + ySquared);
			overlap = sqrtf(overlap);
			glm::vec2 normal = glm::normalize(sphere2->getPosition() - sphere1->getPosition());		
			return CollisionData(true, overlap, normal);
		}
	}
	return CollisionData(false);
}
CollisionData PhysicsScene::sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
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
			float overlap = -intersection;
			glm::vec2 normal = plane->getNormal();
			glm::vec2 contactPoint = sphere->getPosition() + (normal * -sphere->getRadius());
			return CollisionData(true, overlap, normal, contactPoint);			
		}
	}
	return CollisionData(false);
}
CollisionData PhysicsScene::sphere2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere =	dynamic_cast<Sphere*>(object1);
	AABB* aabb =		dynamic_cast<AABB*>(object2);

	glm::vec2 vecBetween = sphere->getPosition() - aabb->getPosition();

	glm::vec2 offset;

	offset.x = glm::dot(vecBetween, { 1,0 });
	offset.y = glm::dot(vecBetween, { 0,1 });

	if (glm::abs(offset.x) > aabb->GetExtends().x)
	{
		(offset.x > 0) ? offset.x = aabb->getPosition().x + aabb->GetExtends().x : offset.x = aabb->getPosition().x - aabb->GetExtends().x;
	}
	if (glm::abs(offset.y) > aabb->GetExtends().y)
	{
		(offset.y > 0) ? offset.y = aabb->getPosition().y + aabb->GetExtends().y : offset.y = aabb->getPosition().y - aabb->GetExtends().y;
	}

	offset += aabb->getPosition();

	glm::vec2 vecBetweenClamp = sphere->getPosition() - offset;

	if (glm::distance(sphere->getPosition(), offset) < sphere->getRadius())
	{
		float overlap = glm::distance(sphere->getPosition(), offset) - sphere->getRadius();
		glm::vec2 normal = glm::normalize(aabb->getPosition() - sphere->getPosition());
		return CollisionData(true, overlap, normal);
	}
	return CollisionData(false);
	
}
CollisionData PhysicsScene::plane2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return sphere2Plane(object2, object1);
}
/*Finish up*/CollisionData PhysicsScene::plane2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	AABB* aabb;
	Plane* plane;
	if (dynamic_cast<Plane*>(object1) != nullptr)
	{
		plane = dynamic_cast<Plane*>(object1);
		aabb = dynamic_cast<AABB*>(object2);
	}
	else
	{
		plane = dynamic_cast<Plane*>(object2);
		aabb = dynamic_cast<AABB*>(object1);
	}

	float tlCornerDistance = glm::dot({ (aabb->getPosition().x - aabb->GetExtends().x),(aabb->getPosition().y + aabb->GetExtends().y) }, plane->getNormal()) - plane->getDistance();
	float trCornerDistance = glm::dot({ (aabb->getPosition().x + aabb->GetExtends().x),(aabb->getPosition().y + aabb->GetExtends().y) }, plane->getNormal()) - plane->getDistance();
	float blCornerDistance = glm::dot({ (aabb->getPosition().x - aabb->GetExtends().x),(aabb->getPosition().y - aabb->GetExtends().y) }, plane->getNormal()) - plane->getDistance();
	float brCornerDistance = glm::dot({ (aabb->getPosition().x + aabb->GetExtends().x),(aabb->getPosition().y - aabb->GetExtends().y) }, plane->getNormal()) - plane->getDistance();

	if (tlCornerDistance < 0.0f)
	{
		//Top left corner is crossing
		float overlap = tlCornerDistance;
		return CollisionData(true, overlap, plane->getNormal());
	}
	else if (trCornerDistance < 0.0f)
	{
		//Top right corner is crossing
		float overlap = trCornerDistance;
		return CollisionData(true, overlap, plane->getNormal());
	}
	else if (blCornerDistance < 0.0f)
	{
		//Bottom left corner is crossing
		float overlap = blCornerDistance;
		return CollisionData(true, overlap, plane->getNormal());
	}
	else if (brCornerDistance < 0.0f)
	{
		//Bottom right corner is crossing
		float overlap = brCornerDistance;
		return CollisionData(true, overlap, plane->getNormal());
	}

	return CollisionData(false);
}
CollisionData PhysicsScene::AABB2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	AABB* aabb1 = dynamic_cast<AABB*>(object1);
	AABB* aabb2 = dynamic_cast<AABB*>(object2);

	if ((aabb1->getPosition().x - aabb2->getPosition().x) > (aabb1->GetExtends().x + aabb2->GetExtends().x)) return CollisionData(false);
	if ((aabb1->getPosition().y - aabb2->getPosition().y) > (aabb1->GetExtends().y + aabb2->GetExtends().y)) return CollisionData(false);
	
	float intersection = (aabb2->getPosition().x - aabb1->getPosition().x) + (aabb2->getPosition().y - aabb1->getPosition().y);
	glm::vec2 normal = glm::normalize(aabb2->getPosition() - aabb1->getPosition());	
	return CollisionData(true, intersection, normal);
}
/*Finish up*/CollisionData PhysicsScene::AABB2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}
CollisionData PhysicsScene::AABB2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object2);
	AABB* aabb = dynamic_cast<AABB*>(object1);

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
		float overlap = sqrtf((dist.x*dist.x + dist.y*dist.y)) - sphere->getRadius();
		glm::vec2 normal = glm::normalize(aabb->getPosition() - sphere->getPosition());
		return CollisionData(true, overlap, normal);
	}
	else
	{
		return CollisionData(false);
	}
}

bool PhysicsScene::SATCollision(RigidBody * object1, RigidBody * object2)
{
	/*std::vector<glm::vec2> shadowAxis;
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

	int totalVerts = object1->getVertices().size + object2->getVertices().size;*/




	return false;
}
