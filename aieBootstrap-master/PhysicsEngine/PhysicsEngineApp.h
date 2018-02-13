#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include <glm/glm.hpp>

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

	PhysicsScene*		m_physicsScene;
	Sphere*				m_sphere1;
	Sphere*				m_sphere2;
	Plane*				m_plane;
};