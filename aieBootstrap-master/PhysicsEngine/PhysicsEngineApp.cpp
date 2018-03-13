#include "PhysicsEngineApp.h"
#include "Texture.h"
#include "Font.h"
#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "Sphere.h"
#include "AABB.h"
#include "Plane.h"

#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <string>
#include <limits>
#include <iostream>


#define _USE_MATH_DEFINES

#include <math.h>



PhysicsEngineApp::PhysicsEngineApp() {

}

PhysicsEngineApp::~PhysicsEngineApp() {

}

bool PhysicsEngineApp::startup() 
{
	aie::Gizmos::create(255U, 255U, 65535U, 65535U);

	m_2dRenderer = new aie::Renderer2D();
	m_physicsScene = new PhysicsScene();
	m_physicsScene->setTimeStep(0.01f);
	m_physicsScene->setGravity(glm::vec2(0.0f, -9.8f));

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);
	m_input = aie::Input::getInstance();

	aie::Gizmos::create(100, 600, 600, 600);

	return true;
}

void PhysicsEngineApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PhysicsEngineApp::update(float deltaTime) 
{
	aie::Gizmos::clear();
	mousePos = { m_input->getMouseX(), m_input->getMouseY() };
	// input example
 
	ImGUI();
	//Resets the scene, Removes all objects
	if (m_input->wasKeyPressed(aie::INPUT_KEY_R))
	{
		m_physicsScene->clearActors();

		m_spheres.clear();
		m_spheres.resize(0);

		m_aabbs.clear();
		m_aabbs.resize(0);

		m_planes.clear();
		m_planes.resize(0);
	}
	if (!m_physicsScene->allowedFixedUpdate())
	{
		if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT) && !ranAlready)
		{
			
			for (int i = 0; i < m_spheres.size(); i++)
			{
				if (glm::distance(m_spheres[i]->getPosition(), mousePos) < currentShortestDistanceSphere)
				{
					currentShortestDistanceSphere = glm::distance(m_spheres[i]->getPosition(), mousePos);
					sphereIndex = i;
				}
			}
			for (int i = 0; i < m_aabbs.size(); i++)
			{
				if (glm::distance(m_aabbs[i]->getPosition(), mousePos) < currentShortestDistanceAABB)
				{
					currentShortestDistanceAABB = glm::distance(m_aabbs[i]->getPosition(), mousePos);
					boxIndex = i;
				}
			}

			ranAlready = true;
			closeObject = false;
		}
	}
	if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT) && m_input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL))
	{
		if (m_createSphere)
		{
			CreateSphere();
		}
		if (m_createAABB)
		{
			CreateAABB();
		}		
	}
	if (m_input->wasKeyPressed(aie::INPUT_KEY_ENTER))
	{
		if (m_createPlane && m_step1)
		{
			CreatePlane();
			m_step1 = false;
			m_step2 = false;
		}
	}
	if (m_createPlane)
	{
		//To lock the mouse to specific axis's
		if (m_input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
		{
			float dx = std::abs(0.0f - mousePos.x);
			float dy = std::abs(0.0f - mousePos.y);

			if (dx > dy)
			{
				mousePos.x = m_input->getMouseX();
				mousePos.y = 0.0f;
			}
			else
			{
				mousePos.x = 0.0f;
				mousePos.y = m_input->getMouseY();
			}
		}
		if (m_step1 && !m_step2)
		{
			if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
			{
				//Gets the vector between the mouse position and the step one position
				glm::vec2 vecBetween = mousePos - m_step1ClickPoint;
				//Normilizes the vector between the 2 position
				m_normalisedVecBetween = glm::normalize(vecBetween);
				m_newPlaneDistanceFromOrigin = glm::dot(m_step1ClickPoint, m_normalisedVecBetween);
				m_step2 = true;
			}
		}
		if (!m_step1 && !m_step2)
		{
			if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
			{
				//Gets the vector between the origin(0,0) and the mouse position
				glm::vec2 vecBetween = glm::vec2(mousePos.x, mousePos.y) - glm::vec2(0, 0);
				//Gets the magnitude of the vector between
				//m_newPlaneDistanceFromOrigin = glm::length(vecBetween);
				m_step1ClickPoint = glm::vec2(mousePos.x, mousePos.y);
				m_step1 = true;
			}
		}		
	}

	m_physicsScene->update(deltaTime);

	// exit the application
	if (m_input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	
}

void PhysicsEngineApp::draw() 
{

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	if (m_createPlane)
	{
		if (m_step1 == false && m_step2 == false)
		{
			m_2dRenderer->drawCircle(0, 0, 5.0f);
			m_2dRenderer->drawCircle(mousePos.x, mousePos.y, 5.0f);
			m_2dRenderer->drawLine(0, 0, mousePos.x, mousePos.y, 2.0f);
		}
		if (m_step1 == true && m_step2 == false)
		{
			m_2dRenderer->drawCircle(m_step1ClickPoint.x, m_step1ClickPoint.y, 5.0f);
			m_2dRenderer->drawCircle(mousePos.x, mousePos.y, 5.0f);
			m_2dRenderer->drawLine(m_step1ClickPoint.x, m_step1ClickPoint.y, mousePos.x, mousePos.y, 2.0f);
		}
	}
	
	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	float aspectRation = (float)getWindowWidth() / getWindowHeight();

	//Render each shape and plane
	for (Sphere* sphere : m_spheres)
	{
		m_2dRenderer->drawCircle(sphere->getPosition().x, sphere->getPosition().y, sphere->getRadius());
		sphere->debug();
	}
	for (AABB* aabb : m_aabbs)
	{
		m_2dRenderer->drawBox(aabb->getPosition().x, aabb->getPosition().y, aabb->GetExtends().x, aabb->GetExtends().y);
		aabb->debug();
	}
	for (Plane* plane : m_planes)
	{
		glm::vec2 newPos = plane->getNormal() * plane->getDistance();
		m_2dRenderer->drawCircle(newPos.x, newPos.y, 5.0f);
		m_2dRenderer->drawLine(plane->getEdge1().x, plane->getEdge1().y, plane->getEdge2().x, plane->getEdge2().y, 2.0f);
		plane->makeGizmo();
	}

	aie::Gizmos::draw2D(getWindowWidth(), getWindowHeight());
	// done drawing sprites
	m_2dRenderer->end();
}

void PhysicsEngineApp::setupContinuousDemo(glm::vec2 startPos, float inclination, float speed, float gravity)
{
	float t = 0;
	float tStep = 0.5f;
	float radius = 1.0f;
	int segments = 12;
	glm::vec4 colour = glm::vec4(1, 1, 0, 1);

	while (t <= 5)
	{
		float y = (startPos.y) + (speed * t) + (0.5f * (gravity * (t*t)));
		float x = (startPos.x) + (speed * t);

		aie::Gizmos::add2DCircle(glm::vec2(x, y), radius, segments, colour);
		t += tStep;
	}
}

void PhysicsEngineApp::ImGUI()
{
	ImGui::Begin("FPS");

	std::string s = "FPS: ";
	s.append(&std::to_string(m_fps)[0]);
	ImGui::Text(&s[0]);

	ImGui::End();

	ImGui::Begin("Create Objects");

	ImGui::Checkbox("Sphere", &m_createSphere);
	if (m_createSphere)
	{
		m_createAABB = false;
		m_createPlane = false;

		ImGui::BeginChild("Sphere Values", { 0, 200 }, true);
		ImGui::Checkbox("Kinematic", &m_newSphereKinematic);
		if (!m_newSphereKinematic)
		{		
			ImGui::InputFloat("Mass", &m_newSphereMass);
			ImGui::InputFloat("Velocity X", &m_newSphereVelocityX);
			ImGui::InputFloat("Velocity Y", &m_newSphereVelocityY);
		}
		ImGui::InputFloat("Radius", &m_newSphereRadius);		
		
		ImGui::EndChild();
	}
	ImGui::Checkbox("AABB", &m_createAABB);
	if (m_createAABB)
	{
		m_createSphere = false;
		m_createPlane = false;

		ImGui::BeginChild("AABB Values", { 0, 200 }, true);
		ImGui::Checkbox("Kinematic", &m_newAABBKinematic);
		if (!m_newAABBKinematic)
		{
			ImGui::InputFloat("Mass", &m_newAABBMass);
			ImGui::InputFloat("Velocity X", &m_newAABBVelocityX);
			ImGui::InputFloat("Velocity Y", &m_newAABBVelocityY);
		}
		ImGui::InputFloat("Extends X", &m_newAABBExtendsX);
		ImGui::InputFloat("Extends Y", &m_newAABBExtendsY);		
		
		ImGui::EndChild();
	}
	ImGui::Checkbox("Plane", &m_createPlane);
	if (m_createPlane)
	{
		m_createSphere = false;
		m_createAABB = false;		
	}

	if (!closeObject)
	{
		if (currentShortestDistanceAABB < currentShortestDistanceSphere)
		{
			ImGui::Begin("Box Values");
			ImGui::InputFloat("Mass", m_aabbs[boxIndex]->getMassAddress());
			ImGui::InputFloat("Elasticity", m_aabbs[boxIndex]->getElasticityAddress());
			ImGui::Checkbox("Finished?", &closeObject);
			ImGui::End();
		}
	}

	ImGui::End();
}

void PhysicsEngineApp::CreateSphere()
{
	Sphere* tempSphere = new Sphere({ m_input->getMouseX(), m_input->getMouseY() }, { m_newSphereVelocityX, m_newSphereVelocityY }
										, (m_newSphereKinematic) ? std::numeric_limits<float>::max() : m_newSphereMass
										, m_newSphereRadius, { 1.0f, 0.0f, 0.0f, 1.0f }, m_newSphereKinematic);

	m_spheres.push_back(tempSphere);
	m_physicsScene->addActor(tempSphere);
}

void PhysicsEngineApp::CreateAABB()
{
	AABB* tempAABB = new AABB({ m_input->getMouseX(), m_input->getMouseY() }, { m_newAABBExtendsX, m_newAABBExtendsY },
								{ m_newAABBVelocityX, m_newAABBVelocityY }, 0.0f, (m_newAABBKinematic) ? std::numeric_limits<float>::max() : m_newAABBMass
								, m_newAABBKinematic);

	m_aabbs.push_back(tempAABB);
	m_physicsScene->addActor(tempAABB);
}

void PhysicsEngineApp::CreatePlane()
{
	Plane* tempPlane = new Plane(m_normalisedVecBetween, m_newPlaneDistanceFromOrigin, { getWindowWidth() * 0.5f, getWindowHeight() * 0.5f });

	m_planes.push_back(tempPlane);
	m_physicsScene->addActor(tempPlane);
}
