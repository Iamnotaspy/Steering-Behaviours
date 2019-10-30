#include "glm.hpp"
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

	//Structure for font char to be used with font/text rendering
	struct FontChar
	{
		GLuint TextureID;
		glm::ivec2 Size;	//Size of the glyph
		glm::ivec2 Bearing;	//Offset of the glyph(top left) from the baseline
		GLuint Advance;		//How far to move to reach the next character
	};
};

