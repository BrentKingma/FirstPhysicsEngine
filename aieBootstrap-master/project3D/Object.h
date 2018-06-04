#pragma once
#include <glm/glm.hpp>
class Object
{
public:
	Object();
	~Object();

	const void SetPosition(const float a_x, const float a_y, const float a_z);
	const void SetPosition(const glm::vec4 a_position);
	const void SetPosition(const glm::vec3 a_position);
	const void SetPosition(const glm::mat4 a_matrix);

	glm::vec3 GetPosition();
	glm::vec4* GetPositionVec4();
	void GetPosition(glm::vec4& a_vector);
	void GetPosition(glm::mat4& a_matrix);
	void GetPosition(float& a_x, float& a_y, float& a_z);

	const glm::mat4* GetMatrix();


	void Translate(glm::vec3 a_positionAddition);
	void Translate(glm::vec4 a_positionAddition);

private:
	glm::mat4 m_matrix;
};

