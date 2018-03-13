#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <imgui.h>
#include <vector>

class PhysicsScene;
class Sphere;
class AABB;
class Plane;

class PhysicsEngineApp : public aie::Application 
{
public:

	PhysicsEngineApp();
	virtual ~PhysicsEngineApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void setupContinuousDemo(glm::vec2 startPos, float inclination, float speed, float gravity);
	void ImGUI();
	void CreateSphere();
	void CreateAABB();
	void CreatePlane();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	aie::Input*			m_input;

	PhysicsScene*	m_physicsScene;

	std::vector<Sphere*>  m_spheres;
	std::vector<AABB*>	m_aabbs;
	std::vector<Plane*>	m_planes;

	glm::vec2 mousePos;

	bool m_createSphere = false;
	bool m_createAABB = false;
	bool m_createPlane = false;

	//Sphere
	float m_newSphereRadius = 0.0f;
	float m_newSphereMass = 0.0f;
	float m_newSphereVelocityX = 0.0f;
	float m_newSphereVelocityY = 0.0f;
	bool m_newSphereKinematic = false;

	//AABB
	float m_newAABBExtendsX = 0.0f;
	float m_newAABBExtendsY = 0.0f;
	float m_newAABBVelocityX = 0.0f;
	float m_newAABBVelocityY = 0.0f;
	float m_newAABBMass = 0.0f;
	bool m_newAABBKinematic = false;

	//Plane
	bool m_step1 = false;
	bool m_step2 = false;
	float m_newPlaneDistanceFromOrigin = 0.0f;
	glm::vec2 m_step1ClickPoint;
	glm::vec2 m_normalisedVecBetween;

	//Getting closest object
	bool ranAlready = false;
	bool closeObject = true;
	int sphereIndex = 0;
	int boxIndex = 0;
	float currentShortestDistanceSphere = std::numeric_limits<float>::max();
	float currentShortestDistanceAABB = std::numeric_limits<float>::max();
};