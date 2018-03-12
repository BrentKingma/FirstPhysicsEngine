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
	glm::vec2 normal = collision.normal;
	//glm::vec2 relativeVelocity = object2->getVelocity() - object1->getVelocity();

	glm::vec2 perp(normal.y, -normal.x);

	float r1 = glm::dot(collision.contact - object1->getPosition(), -perp);
	float r2 = glm::dot(collision.contact - object2->getPosition(), perp);

	float v1 = glm::dot(object1->getVelocity(), normal) - r1 * object1->getRotation();
	float v2 = glm::dot(object2->getVelocity(), normal) + r2 * object2->getRotation();

	if (v1 > v2)
	{
		float elasticity = (object1->getElasticity() + object2->getElasticity()) / 2.0f;

		float mass1 = 1.0f / (1.0f / object1->getMass() + (r1*r1) / object1->getMoment());
		float mass2 = 1.0f / (1.0f / object2->getMass() + (r2*r2) / object2->getMoment());

		glm::vec2 force = (1.0f + elasticity) * mass1 * mass2 / (mass1 + mass2) * (v1 - v2) * normal;

		object1->applyLinearForce(-force, FORCEMODE::IMPULSE);
		object1->applyRotationalForce(-force, collision.contact - object1->getPosition());

		object2->applyLinearForce(force, FORCEMODE::IMPULSE);
		object2->applyRotationalForce(force, collision.contact - object2->getPosition());
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

	glm::vec2 relativeVelocity = rigid->getVelocity();
	float elasticity = rigid->getElasticity();

	float j = glm::dot(-(1 + elasticity) * relativeVelocity, collision.normal) / (1 / rigid->getMass());

	glm::vec2 force = collision.normal * j;

	rigid->applyLinearForce(force, FORCEMODE::IMPULSE);
	rigid->applyRotationalForce(force, collision.contact - rigid->getPosition());
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

		//Gets both spheres x and y position
		float sphere1X = sphere1->getPosition().x;
		float sphere1Y = sphere1->getPosition().y;
		float sphere2X = sphere2->getPosition().x;
		float sphere2Y = sphere2->getPosition().y;

		//Squares the distances between each spheres x and y because it is less resource intensive
		float xSqaured = (sphere2X - sphere1X)*(sphere2X - sphere1X);
		float ySquared = (sphere2Y - sphere1Y)*(sphere2Y - sphere1Y);

		if ((xSqaured + ySquared) < radius)
		{		
			glm::vec2 vecBetween = sphere1->getPosition() - sphere2->getPosition();
			float overlap = (sphere1->getRadius() + sphere2->getRadius()) - glm::length(vecBetween);
			glm::vec2 normal = glm::normalize(sphere2->getPosition() - sphere1->getPosition());	
			glm::vec2 contact = (sphere1->getPosition() + sphere2->getPosition()) * 0.5f;
			return CollisionData(true, overlap, normal, 0.0f, contact);
		}
	}
	return CollisionData(false);
}
CollisionData PhysicsScene::sphere2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere = dynamic_cast<Sphere*>(object1);
	Plane* plane = dynamic_cast<Plane*>(object2);

	//If both casted succesfully we continue
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
			glm::vec2 contact = sphere->getPosition() + ( plane->getNormal() * -sphere->getRadius());
			aie::Gizmos::add2DCircle(contact, 1.0f, 12, { 1.0f, 1.0f, 1.0f, 1.0f });
			return CollisionData(true, overlap, plane->getNormal(), 0.0f, contact);
		}
	}
	return CollisionData(false);
}
CollisionData PhysicsScene::sphere2AABB(PhysicsObject* object1, PhysicsObject* object2)
{
	Sphere* sphere;
	AABB* aabb;
	//0: Negate overlap		1: overlap
	//0: Sphere to AABB		1: AABB to Sphere
	int type;

	//Checks and does succesful casting and type checking for weather it is sphere to aabb or aabb to sphere
	if (dynamic_cast<Sphere*>(object1) != nullptr)
	{
		sphere = dynamic_cast<Sphere*>(object1);
		aabb = dynamic_cast<AABB*>(object2);
		type = 0;
	}
	else
	{
		sphere = dynamic_cast<Sphere*>(object2);
		aabb = dynamic_cast<AABB*>(object1);
		type = 1;
	}
	//Gets the vector between the 2 positions
	glm::vec2 vecBetween = sphere->getPosition() - aabb->getPosition();
	//It is the clamped point to the AABB
	glm::vec2 offset;

	//Clamps the point to the axis's
	offset.x = glm::dot(vecBetween, { 1,0 });
	offset.y = glm::dot(vecBetween, { 0,1 });

	if (glm::abs(offset.x) > (aabb->GetExtends().x * 0.5f))
	{
		offset.x = (aabb->GetExtends().x) * ((offset.x > 0) ? 1.0 : -1.0f);
	}
	if (glm::abs(offset.y) > aabb->GetExtends().y * 0.5f)
	{
		offset.y = (aabb->GetExtends().y) * ((offset.y > 0) ? 1.0 : -1.0f);
	}

	offset += aabb->getPosition();

	glm::vec2 vecBetweenClamp = offset - sphere->getPosition();

	if (glm::length(vecBetweenClamp) < sphere->getRadius())
	{
		float overlap;
		//Checks weather you need to negate the overlap
		if (type == 0)	{	overlap = -(glm::length(vecBetweenClamp) - sphere->getRadius());	}
		else			{	overlap = (glm::length(vecBetweenClamp) - sphere->getRadius());		}

		glm::vec2 normal = glm::normalize(vecBetweenClamp);;
		return CollisionData(true, overlap, normal);
	}
	return CollisionData(false);
}
CollisionData PhysicsScene::plane2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return sphere2Plane(object2, object1);
}
CollisionData PhysicsScene::plane2AABB(PhysicsObject* object1, PhysicsObject* object2)
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

	if (std::abs(aabb1->getPosition().x - aabb2->getPosition().x) > (aabb1->GetExtends().x + aabb2->GetExtends().x)) return CollisionData(false);
	if (std::abs(aabb1->getPosition().y - aabb2->getPosition().y) > (aabb1->GetExtends().y + aabb2->GetExtends().y)) return CollisionData(false);

	glm::vec2 xOverlap1 = { aabb1->GetTopRight().x - aabb2->GetBottomLeft().x, 0.0f };
	glm::vec2 xOverlap2 = { aabb1->GetBottomLeft().x - aabb2->GetTopRight().x, 0.0f };

	glm::vec2 yOverlap1 = { 0.0f , aabb1->GetTopRight().y - aabb2->GetBottomLeft().y };
	glm::vec2 yOverlap2 = { 0.0f , aabb1->GetBottomLeft().y - aabb2->GetTopRight().y };


	float lowestDist = abs(xOverlap1.x);
	glm::vec2 lowestVec = xOverlap1;

	if (abs(xOverlap2.x) < lowestDist)
	{
		lowestDist = abs(xOverlap2.x);
		lowestVec = xOverlap2;
	}

	if (abs(yOverlap1.y) < lowestDist)
	{
		lowestDist = abs(yOverlap1.y);
		lowestVec = yOverlap1;
	}

	if (abs(yOverlap2.y) < lowestDist)
	{
		lowestDist = abs(yOverlap2.y);
		lowestVec = yOverlap2;
	}

	return CollisionData(true, lowestDist, glm::normalize(lowestVec));
}
CollisionData PhysicsScene::AABB2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return plane2AABB(object2, object1);
}
CollisionData PhysicsScene::AABB2Sphere(PhysicsObject* object1, PhysicsObject* object2)
{
	return sphere2AABB(object1, object2);
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
