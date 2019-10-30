#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#pragma once
class Object
{
public:
	Object();
	~Object();
	bool IsRunning;
	bool IsJumping;
	glm::mat4 model;
	glm::vec3 pos;
	glm::vec2 acceleration;
};

