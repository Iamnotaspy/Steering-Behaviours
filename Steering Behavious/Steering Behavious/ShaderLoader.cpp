#include "ShaderLoader.h" 
#include<iostream>
#include<fstream>
#include<vector>
#include<iterator>
#include<map>
#include<string>


static std::map<unsigned int, std::string> mShaders;

static std::map<unsigned int, std::string> mPrograms;

ShaderLoader::ShaderLoader(void){}
ShaderLoader::~ShaderLoader(void){}

void
ShaderLoader::ShutDown()
{
	
}

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	//Create program
	GLuint program = glCreateProgram();
	//Create Vertex shader
	GLuint vShader = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename);

	//Create fragment shader
	GLuint fShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);
	
	//Check if program(combination of shaders already exists)
	//As all identical shaders are given the same ID all we 
	//have to check is the concatenated ID's of all the shaders
	
	//Check if a program with a matching combined ID (shader ID's combined)
	//already exists
	

	
	std::string CombinedID = std::to_string(vShader);
	CombinedID += std::to_string(fShader);
	for (auto&pair : mPrograms)
	{
		if (CombinedID == pair.second)
		{
			//Shader already exists. Return the shader ID
			std::cout << "This program already exists";
			return(pair.first);
		}
	}

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	glLinkProgram(program);
	

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		std::string programName = vertexShaderFilename + *fragmentShaderFilename;
		PrintErrorDetails(false, program, programName.c_str());
		return 0;
	}
	//Add new program to map as there have been no errors
	mPrograms.insert(std::pair<unsigned int, std::string>(program, CombinedID));
	return program;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, const char* shaderName)
{
	std::string Shader = ReadShaderFile(shaderName);

	for (auto&pair : mShaders)
	{
		if (Shader == pair.second)
		{
			//Shader already exists. Return the shader ID
			std::cout << "This shader already exists";
			return(pair.first);
		}
	}

	
	const char* ShaderChar = Shader.c_str();
	const GLint length = Shader.size();
	GLuint shaderID = glCreateShader(shaderType);
	mShaders.insert(std::pair<unsigned int, std::string>(shaderID, Shader));
	glShaderSource(shaderID, 1, &ShaderChar, &length);
	glCompileShader(shaderID);
	

	// Check for errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
	}
	return shaderID;
}

std::string ShaderLoader::ReadShaderFile(const char *filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

void ShaderLoader::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> log(infoLogLength);

	// Retrieve the log info and populate log variable
	(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);		
	std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << &log[0] << std::endl;
}

