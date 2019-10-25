#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "utility.h"
#pragma once
class camera
{
public:
	camera();
	~camera();
	glm::vec3 GetPos();
	glm::vec3 GetLookDir();
	glm::vec3 GetUpDir();
	glm::mat4 GetprojView();
	void SetprojView();

private:
	glm::vec3 camPos;
	glm::vec3 camLookDir;
	glm::vec3 camUpDir;
	
	glm::mat4 projView;
};
