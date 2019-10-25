#include "gameManager.h"



gameManager::gameManager()
{
}


gameManager::~gameManager()
{
}

//Input
enum InputState
{
	INPUT_UP,
	INPUT_DOWN,
	INPUT_UP_FIRST,
	INPUT_DOWN_FIRST,
};

InputState KeyState[255];

void KeyboardDown(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_DOWN;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_UP;
}

//Create global variables
GLuint program;
GLfloat currentTime;
camera Camera;
glm::mat4 pvm;
glm::mat4 circleModel;	//Uses a quad but has a fragment shader that only draws the points within a certain radius
glm::vec3 objPos = glm::vec3(0.0f, 0.0f, 0.0f);

//Vertices for hexagon model
GLfloat hexagonVertices[] = {
	//pos						//colour			//Texture Coords
	-1.0f, 0.5f, 0.0f,			0.0f, 1.0f, 0.0f,	0.0f, 0.3f,		// L T
	-1.0f, -0.5f, 0.0f,			1.0f, 0.0f, 1.0f,	0.0f, 0.7f,		// L B
	0.0f, -1.0f, 0.0f,			1.0f, 0.0f, 1.0f,	0.5f, 1.0f,		// M B
	1.0f, -0.5f, 0.0f,			1.0f, 1.0f, 0.0f,	1.0f, 0.7f,		// R B
	1.0f, 0.5f, 0.0f,			0.0f, 0.0f, 1.0f,	1.0f, 0.3f,		// R T
	0.0f, 1.0f, 0.0f,			0.0f, 0.0f, 1.0f,	0.5f, 0.0f,		// M T


};

//Vertices for quad
GLfloat quadVertices[] = {
	//pos					
	-0.5f, 0.5f, 0.0f,		//T L
	-0.5f, -0.5f, 0.0f,		//B L
	0.5f, -0.5f, 0.0f,		//B R
	0.5f, 0.5f, 0.0f,		//T R

};

//Indices for hexagons
GLuint hexagonIndices[] = {
	0, 1, 3,	//First tri
	0, 3, 4,	//Second tri
	0, 4, 5,	//Third tri
	1, 2, 3,	//Fourth tri

};

//Indices for quad
GLuint quadIndices[] = {
	0, 2, 3,	//First tri
	0, 1, 2,	//Second tri
};

GLuint VBO;
GLuint VAO;
GLuint EBO;


void ProcessInput()
{
	if (KeyState['w'] == INPUT_DOWN)
	{
		objPos.y += 1.0f;
	}
	if (KeyState['s'] == INPUT_DOWN)
	{
		objPos.y -= 1.0f;
	}
	if (KeyState['d'] == INPUT_DOWN)
	{
		objPos.x += 1.0f;
	}
	if (KeyState['a'] == INPUT_DOWN)
	{
		objPos.x -= 1.0f;
	}
}


void  Update()
{
	//Update game info

	currentTime = (float)glutGet(GLUT_ELAPSED_TIME);	//Get current time
	currentTime = currentTime * 0.001f;		//Convert to seconds

											//Translation matrix
	glm::vec3 objPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(), objPosition);



	glm::vec3 objScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(), objScale);

	glm::mat4 model = translationMatrix * scaleMatrix;

	ProcessInput();
	
	//Initialise pvm matrix with model 1
	pvm = Camera.GetprojView() * model;

	//Update frame counter which is used to
	//animate the textures and rotate the hexagons

	glutPostRedisplay();	//Function in glutDisplayFund() is called
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//Render code will be written here
	glUseProgram(program);

	//Quad (shows as a circle when drawn by fragment shader
	glBindVertexArray(VAO);	//Bind VAO2
	pvm = Camera.GetprojView() * circleModel;
	GLuint pvmLoc = glGetUniformLocation(program, "pvm");
	pvmLoc = glGetUniformLocation(program, "pvm");
	glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);

	glutSwapBuffers();
}

void
gameManager::Run()
{
	//Sets the clear colour when calling glClear()
	glClearColor(0.0f, 0.4f, 0.6f, 1.0f); //Red

	program = ShaderLoader::CreateProgram("Resources/Shaders/VertexShader.txt",
		"Resources/Shaders/FragmentShader.txt");
	//VAO, EBO, VBO for quad
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//Pos
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),	//Stride of a single vertex (for pos + col + tex)
		(GLvoid*)0);			//Offest from vertex start
	glEnableVertexAttribArray(0);

	
	//Unbind VAO
	glBindVertexArray(0);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	Camera.SetprojView();

	//Register Callbacks
	Update();
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	glutKeyboardFunc(KeyboardDown);
	glutKeyboardUpFunc(KeyboardUp);
	return;
}

void 
gameManager::ShutDown()
{
	glDeleteProgram(program);
	return;
}