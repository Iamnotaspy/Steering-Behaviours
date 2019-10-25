#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include<glew.h>
#include<freeglut.h>
#include<SOIL.h>
#include<iostream>
#include "utility.h"
#include"gameManager.h"

gameManager* pGame = new gameManager();

void ShutDown()
{
	pGame->ShutDown();
	delete pGame;
	pGame = nullptr;
}

int main(int argc, char **argv) {

	
	//Setup and create at glut controlled window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(utility::SCR_WIDTH, utility::SCR_HEIGHT);
	glutCreateWindow("Steering Behaviours");

	//Sets up all gl function callbacks based on pc hardware
	if (glewInit() != GLEW_OK)
	{
		//If glew setup failed then application will not run graphics correctly
		std::cout << "Glew Initialization Failed. Aborting Application." << std::endl;
		system("quit");
	}
	
	pGame->Run();
	glutCloseFunc(ShutDown);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop(); //Ensure this is the last glut line called
	return 0;
}

