#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include <glm/glm.hpp>
#include <imgui.h>

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

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;

	Plane*				m_plane1;
	Plane*				m_plane2;
	Plane*				m_plane3;

	Sphere*				m_sphere1;
	Sphere*				m_sphere2;

	PhysicsScene*		m_physicsScene;

};