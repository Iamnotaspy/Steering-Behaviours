#pragma once
//Dependency Includes
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include<glew.h>
#include<freeglut.h>
#include<SOIL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

//Library Includes
#include <map>
#include<iostream>
#include <string>

//Local Includes
#include"utility.h"
#include"ShaderLoader.h"
class TextLabel
{
public:
	TextLabel(
		std::string _text,
		std::string _font,
		glm::vec2 _pos,
		glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f),
		float _scale = 1.0f
		);
	~TextLabel();
	void Render();
	void SetText(std::string newText) { text = newText; };
	void SetColor(glm::vec3 newColor) { color = newColor; };
	void SetScale(GLfloat newScale) { scale = newScale; };
	void SetPosition(glm::vec2 newPosition) {position = newPosition; };

private:
	GLuint GenerateTexture(FT_Face face);

	std::string text;
	GLfloat scale;
	glm::vec3 color;
	glm::vec2 position;

	GLuint VAO, VBO, program;
	glm::mat4 proj;
	std::map<GLchar, utility::FontChar> Characters;

};

