#include "PhysicsEngineApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "Sphere.h"
#include "AABB.h"
#include "Plane.h"

#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

	m_plane1 = new Plane({ -0.707f,-0.707f }, -40.0f);
	m_plane2 = new Plane({ 0.707f, -0.707f }, -40.0f);
	m_plane3 = new Plane({ 0.0f, 1.0f }, -40.0f);

	m_sphere1 = new Sphere({ -30.0f, 10.0f }, { 20.0f, 0.0f }, 5.0f, 3.0f, { 1.0f, 0.0f, 0.0f, 1.0f }, false);

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene->addActor(m_plane1);
	m_physicsScene->addActor(m_plane2);
	m_physicsScene->addActor(m_plane3);

	m_physicsScene->addActor(m_sphere1);

	return true;
}

void PhysicsEngineApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PhysicsEngineApp::update(float deltaTime) 
{
	aie::Gizmos::clear();
	// input example
	aie::Input* input = aie::Input::getInstance();

	m_physicsScene->update(deltaTime);
	m_physicsScene->updateGimozs();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void PhysicsEngineApp::draw() 
{

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	
	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	float aspectRation = (float)getWindowWidth() / getWindowHeight();
	aie::Gizmos::draw2D(glm::ortho<float>(-100.0f, 100.0f, -100.0f / aspectRation, 100.0f / aspectRation, -1.0f, 1.0f));

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