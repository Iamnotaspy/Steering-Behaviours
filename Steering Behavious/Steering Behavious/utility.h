#include<glew.h>
#include<SOIL.h>
#pragma once
class utility
{
public:
	utility();
	~utility();
	static void loadTexture(GLuint* texture, const char* filename);

	static const unsigned int SCR_WIDTH = 800;
	static const unsigned int SCR_HEIGHT = 600;
};

