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
//#include<vld.h>

//Library Includes
#include <chrono>
#include <map>
#include<iostream>
#include <string>

//Local Includes
#include"ShaderLoader.h"
#include "camera.h"
#include "utility.h"
#include "Input.h"
#include "Object.h"
#include "audioManager.h"
#include "TextLabel.h"
class gameManager
{
public:
	gameManager();
	~gameManager();
	void Run();
	void ShutDown();
};

