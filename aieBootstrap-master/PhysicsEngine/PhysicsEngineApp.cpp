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
#include <math.h>

#define _USE_MATH_DEFINES

PhysicsEngineApp::PhysicsEngineApp() {

}

PhysicsEngineApp::~PhysicsEngineApp() {

}

bool PhysicsEngineApp::startup() 
{
	aie::Gizmos::create(255U, 255U, 65535U, 65535U);

	m_2dRenderer = std::make_unique<aie::Renderer2D>();
	m_physicsScene = std::make_unique<PhysicsScene>();
	m_physicsScene->setTimeStep(0.01f);
	m_physicsScene->setGravity(glm::vec2(0.0f, -98.0f));

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);
	m_input = aie::Input::getInstance();

	aie::Gizmos::create(100, 600, 600, 600);

	return true;
}

void PhysicsEngineApp::shutdown() 
{
	//Clears arrays
	if (m_spheres.size() > 0)
	{
		m_spheres.clear();
		m_spheres.resize(0);
	}
	if (m_aabbs.size() > 0)
	{
		m_aabbs.clear();
		m_aabbs.resize(0);
	}
	if (m_planes.size() > 0)
	{
		m_planes.clear();
		m_planes.resize(0);
	}
}

void PhysicsEngineApp::update(float deltaTime) 
{
	//For exterior borders
	if (!yes && !no)
	{
		ImGui::Begin("Do you want external borders?");
		ImGui::Checkbox("Yes", &yes);
		if (yes)
		{
			float width = getWindowWidth();
			float height = getWindowHeight();

			Plane* tempPlane1 = new Plane({ 0.0f, 1.0f }, 0);
			Plane* tempPlane2 = new Plane({ 1.0f, 0.0f }, 0.0f);
			Plane* tempPlane3 = new Plane({ 0.0f, -1.0f }, -height);
			Plane* tempPlane4 = new Plane({ -1.0f, 0.0f }, -width);

			m_planes.push_back(tempPlane1);
			m_physicsScene->addActor(tempPlane1);
			m_planes.push_back(tempPlane2);
			m_physicsScene->addActor(tempPlane2);
			m_planes.push_back(tempPlane3);
			m_physicsScene->addActor(tempPlane3);
			m_planes.push_back(tempPlane4);
			m_physicsScene->addActor(tempPlane4);
		}
		ImGui::Checkbox("No", &no);
		ImGui::End();
	}
	aie::Gizmos::clear();
	//Have a reference point for calculations
	mousePos = { m_input->getMouseX(), m_input->getMouseY() };
 
	ImGUI();
	//Resets the scene, Removes all objects
	if (m_input->wasKeyPressed(aie::INPUT_KEY_R) && !m_input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL))
	{
		m_physicsScene->clearActors();

		m_spheres.clear();
		m_spheres.resize(0);

		m_aabbs.clear();
		m_aabbs.resize(0);

		m_planes.clear();
		m_planes.resize(0);

		m_step1 = false;
		m_step2 = false;
		ranAlready = false;
		closeObject = true;
		sphereIndex = 0;
		boxIndex = 0;
		m_newPlaneDistanceFromOrigin = 0.0f;
	}
	//Removes all spheres
	if (m_input->wasKeyPressed(aie::INPUT_KEY_R) && m_input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL))
	{
		m_spheres.clear();
		m_spheres.resize(0);
	}
	//Selecting an object to adjust
	if (!m_physicsScene->allowedFixedUpdate())
	{
		if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
		{
			
			for (int i = 0; i < m_spheres.size(); i++)
			{
				if (glm::distance(m_spheres[i]->getPosition(), mousePos) < currentShortestDistanceSphere)
				{
					if (glm::distance(m_spheres[i]->getPosition(), mousePos) < 3.0f)
					{
						currentShortestDistanceSphere = glm::distance(m_spheres[i]->getPosition(), mousePos);
						sphereIndex = i;

						ranAlready = true;
						closeObject = false;
					}
				}
			}
			for (int i = 0; i < m_aabbs.size(); i++)
			{
				if (glm::distance(m_aabbs[i]->getPosition(), mousePos) < currentShortestDistanceAABB)
				{
					if (glm::distance(m_aabbs[i]->getPosition(), mousePos) < 5.0f)
					{
						currentShortestDistanceAABB = glm::distance(m_aabbs[i]->getPosition(), mousePos);
						boxIndex = i;

						ranAlready = true;
						closeObject = false;
					}
				}
			}

			
		}
	}
	else
	{
		currentShortestDistanceSphere = std::numeric_limits<float>::max();
		sphereIndex = 0;
		currentShortestDistanceAABB = std::numeric_limits<float>::max();
		boxIndex = 0;

		ranAlready = false;
		closeObject = true;
	}
	//Creating spheres and aabbs
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
	//Creating planes
	if (m_input->wasKeyPressed(aie::INPUT_KEY_ENTER))
	{
		if (m_createPlane && m_step1)
		{
			CreatePlane();
			m_step1 = false;
			m_step2 = false;
		}
	}
	//Setting up the planes
	if (m_createPlane)
	{
		//To lock the mouse to specific axis's
		if (m_input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT) && m_step1)
		{
			float dx = std::abs(m_step1ClickPoint.x - mousePos.x);
			float dy = std::abs(m_step1ClickPoint.y - mousePos.y);

			if (dx > dy)
			{
				mousePos.x = m_input->getMouseX();
				mousePos.y = m_step1ClickPoint.y;
			}
			else
			{
				mousePos.x = m_step1ClickPoint.x;
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
	//Adding force to all objects
	if (m_addForce)
	{
		for (Sphere* sphere : m_spheres)
		{
			float distance = glm::distance(mousePos, sphere->getPosition());
			if (distance < m_maxEffectDistance)
			{
				float modifier = distance / m_maxEffectDistance;
				sphere->setColor(glm::vec4(0.0f, 1.0f - (1.0f * modifier), 0.0f, 1.0f ));
			}
			else
			{
				sphere->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
			}
		}
		for (AABB* aabb : m_aabbs)
		{
			float distance = glm::distance(mousePos, aabb->getPosition());
			if (distance < m_maxEffectDistance)
			{
				float modifier = distance / m_maxEffectDistance;
				aabb->setColor(glm::vec4(0.0f, 1.0f - (1.0f * modifier), 0.0f, 1.0f));
			}
			else
			{
				aabb->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
			}
		}
		if (m_input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
		{
			for (Sphere* sphere : m_spheres)
			{
				float distance = glm::distance(mousePos, sphere->getPosition());
				if (distance < m_maxEffectDistance)
				{
					float modifier = distance / m_maxEffectDistance;
					glm::vec2 tempNormal = glm::normalize(sphere->getPosition() - mousePos);
					sphere->applyLinearForce(tempNormal*(m_forceAmount / modifier), FORCEMODE::IMPULSE);
				}
			}
			for (AABB* aabb : m_aabbs)
			{
				float distance = glm::distance(mousePos, aabb->getPosition());
				if (distance < m_maxEffectDistance)
				{
					float modifier = distance / m_maxEffectDistance;
					glm::vec2 tempNormal = glm::normalize(aabb->getPosition() - mousePos);
					aabb->applyLinearForce(tempNormal*(m_forceAmount / modifier), FORCEMODE::IMPULSE);
				}
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
	//Creating a plane UI help
	if (m_createPlane)
	{
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
	for (int i = 0; i < m_spheres.size(); i ++)
	{
		if (m_spheres[i] != nullptr)
		{
			m_2dRenderer->setRenderColour(m_spheres[i]->getColor().x, m_spheres[i]->getColor().y, m_spheres[i]->getColor().z, m_spheres[i]->getColor().w);
			m_2dRenderer->drawCircle(m_spheres[i]->getPosition().x, m_spheres[i]->getPosition().y, m_spheres[i]->getRadius());
			m_spheres[i]->debug();
		}
	}
	for (int i = 0; i < m_aabbs.size(); i ++)
	{
		if (m_aabbs[i] != nullptr)
		{
			m_2dRenderer->setRenderColour(m_aabbs[i]->getColor().x, m_aabbs[i]->getColor().y, m_aabbs[i]->getColor().z, m_aabbs[i]->getColor().w);
			m_2dRenderer->drawBox(m_aabbs[i]->getPosition().x, m_aabbs[i]->getPosition().y, m_aabbs[i]->GetExtends().x, m_aabbs[i]->GetExtends().y);
			m_aabbs[i]->debug();
		}
	}
	for (int i = 0; i < m_planes.size(); i ++)
	{
		glm::vec2 newPos = m_planes[i]->getNormal() * m_planes[i]->getDistance();
		m_2dRenderer->setRenderColour(1.0f, 0.0f, 0.0f, 1.0f);
		m_2dRenderer->drawCircle(newPos.x, newPos.y, 5.0f);
		m_2dRenderer->drawLine(m_planes[i]->getEdge1().x, m_planes[i]->getEdge1().y, m_planes[i]->getEdge2().x, m_planes[i]->getEdge2().y, 2.0f);
		m_planes[i]->makeGizmo();
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
	//Creating a sphere UI elements
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
	//Creating a aabb UI elements
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
	//Creating a plane UI elements
	ImGui::Checkbox("Plane", &m_createPlane);
	if (m_createPlane)
	{
		m_createSphere = false;
		m_createAABB = false;		
	}
	//Adding a force to all nearby objects
	ImGui::Checkbox("Add force to objects", &m_addForce);
	if (m_addForce)
	{
		ImGui::InputFloat("How much power", &m_forceAmount);
		ImGui::InputFloat("How far?", &m_maxEffectDistance);
	}
	//Pops up when you click on an object and allows for a UI to open
	if (!closeObject)
	{
		if (currentShortestDistanceAABB < currentShortestDistanceSphere)
		{
			ImGui::Begin("Box Values");
			ImGui::InputFloat("Mass", m_aabbs[boxIndex]->getMassAddress());
			ImGui::InputFloat("Elasticity", m_aabbs[boxIndex]->getElasticityAddress());
			ImGui::Checkbox("Delete?", m_aabbs[boxIndex]->getDeleteAddress());
			if (m_aabbs[boxIndex]->getDelete())
			{
				m_aabbs.erase(m_aabbs.begin() + boxIndex);
				m_aabbs.shrink_to_fit();
				closeObject = true;
				currentShortestDistanceAABB = std::numeric_limits<float>::max();
				currentShortestDistanceSphere = std::numeric_limits<float>::max();
			}
			ImGui::Checkbox("Finished?", &closeObject);
			ImGui::End();
		}
		if (currentShortestDistanceAABB > currentShortestDistanceSphere)
		{
			ImGui::Begin("Sphere Values");
			ImGui::InputFloat("Mass", m_spheres[sphereIndex]->getMassAddress());
			ImGui::InputFloat("Elasticity", m_spheres[sphereIndex]->getElasticityAddress());
			ImGui::Checkbox("Delete?", m_spheres[sphereIndex]->getDeleteAddress());
			if (m_spheres[sphereIndex]->getDelete())
			{
				m_spheres.erase(m_spheres.begin() + sphereIndex);
				m_spheres.shrink_to_fit();
				closeObject = true;
				currentShortestDistanceAABB = std::numeric_limits<float>::max();
				currentShortestDistanceSphere = std::numeric_limits<float>::max();
			}
			ImGui::Checkbox("Finished?", &closeObject);
			ImGui::End();
		}
	}
	if (closeObject && ranAlready)
	{
		ranAlready = false;
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
								{ m_newAABBVelocityX, m_newAABBVelocityY }, { 1.0f, 0.0f, 0.0f, 1.0f }, 0.0f, (m_newAABBKinematic) ? std::numeric_limits<float>::max() : m_newAABBMass
								, m_newAABBKinematic);

	m_aabbs.push_back(tempAABB);
	m_physicsScene->addActor(tempAABB);
}
void PhysicsEngineApp::CreatePlane()
{
	Plane* tempPlane = new Plane(m_normalisedVecBetween, m_newPlaneDistanceFromOrigin);
	std::cout << m_normalisedVecBetween.x <<" "<< m_normalisedVecBetween.y << "	" << m_newPlaneDistanceFromOrigin << std::endl;
	m_planes.push_back(tempPlane);
	m_physicsScene->addActor(tempPlane);
}
