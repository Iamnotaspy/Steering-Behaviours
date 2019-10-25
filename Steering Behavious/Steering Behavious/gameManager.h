#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include<glew.h>
#include<freeglut.h>
#include<SOIL.h>
#include<vld.h>
#include<iostream>
#include <string>
#include"ShaderLoader.h"
#include "camera.h"
#include "utility.h"
#pragma once
class gameManager
{
public:
	gameManager();
	~gameManager();
	void Run();
	void ShutDown();
};

