#include "Object.h"



Object::Object()
{
	m_matrix = glm::mat4(1);
}


Object::~Object()
{
}

const void Object::SetPosition(const float a_x, const float a_y, const float a_z)
{
	m_matrix[3][0] = a_x;
	m_matrix[3][1] = a_y;
	m_matrix[3][2] = a_z;
}

const void Object::SetPosition(const glm::vec4 a_position)
{
	m_matrix[3] = a_position;
}

const void Object::SetPosition(const glm::vec3 a_position)
{
	m_matrix[3][0] = a_position.x;
	m_matrix[3][1] = a_position.y;
	m_matrix[3][2] = a_position.z;
}

const void Object::SetPosition(const glm::mat4 a_matrix)
{
	m_matrix[3] = a_matrix[3];
}


glm::vec3 Object::GetPosition()
{
	return glm::vec3(m_matrix[3][0], m_matrix[3][1], m_matrix[3][2]);
}

glm::vec4 * Object::GetPositionVec4()
{
	return &m_matrix[3];
}

void Object::GetPosition(glm::vec4 & a_vector)
{
	a_vector = m_matrix[3];
}

void Object::GetPosition(glm::mat4 & a_matrix)
{

}

void Object::GetPosition(float & a_x, float & a_y, float & a_z)
{
	a_x = m_matrix[3][0];
	a_y = m_matrix[3][1];
	a_z = m_matrix[3][2];
}

const glm::mat4* Object::GetMatrix()
{
	return &m_matrix;
}

void Object::Translate(glm::vec3 a_positionAddition)
{
	m_matrix[3][0] += a_positionAddition.x;
	m_matrix[3][1] += a_positionAddition.y;
	m_matrix[3][2] += a_positionAddition.z;
}

void Object::Translate(glm::vec4 a_positionAddition)
{
	m_matrix[3] += a_positionAddition;
}

