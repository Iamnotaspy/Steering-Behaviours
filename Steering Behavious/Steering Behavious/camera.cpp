#include "camera.h"



camera::camera()
{
	camPos = glm::vec3(0.0f, 0.0f, 3.0f);
	camLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	camUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
}


camera::~camera()
{
}


glm::vec3
camera::GetPos()
{
	return(camPos);
}

void camera::SetPos(float x, float y)
{
	if (camPos.x != x)
	{
		//Camera has moved. Reset proj matrix
		SetprojView();
	}
	camPos.x = x;
	camPos.y = y;
	return;
}

glm::vec3
camera::GetLookDir()
{
	return(camLookDir);
}

glm::vec3
camera::GetUpDir()
{
	return(camUpDir);
}

glm::mat4 
camera::GetprojView()
{
	return(projView);
}

void
camera::SetprojView()
{
	//Initialise view matrix with camera
	glm::mat4 view = glm::lookAt(GetPos(),
		GetPos() + GetLookDir(),
		GetUpDir());

	//GLuint viewLoc = glGetUniformLocation(program, "view");
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj;
	proj = glm::perspective(45.0f, (float)utility::SCR_WIDTH / (float)utility::SCR_HEIGHT, 0.1f, 100.0f);
	//proj = glm::ortho(-(float)utility::SCR_WIDTH / 2.0f, (float)utility::SCR_WIDTH / 2.0f, -(float)utility::SCR_HEIGHT / 2.0f, +(float)utility::SCR_HEIGHT / 2.0f, 0.1f, 100.0f);
	//GLuint projLoc = glGetUniformLocation(program, "proj");
	//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
	projView = proj * view;
}

