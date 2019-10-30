#include "Input.h"



Input::Input()
{
}


Input::~Input()
{
}

void Input::KeyBoardDown(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_DOWN;
}

void Input::KeyBoardUp(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_UP;
}

float Input::ProcessInput(unsigned char key)
{
	if (KeyState[key] == INPUT_DOWN)
	{
		return(0.03f);
	}
	else
	{
		return(0.0f);
	}
}

void Input::MouseClick(int button, int state, int x, int y)
{
	if (button >= 3)	//Ensure the array is not accessed out of bounds
	{
		return;
	}
	MouseState[button] = (state == downState) ? INPUT_DOWN : INPUT_UP;
}

void Input::MousePassiveMove(int x, int y)
{
	//Updates the mouse x and y position
	mouseX = x;
	mouseY = y;
	//std::cout << "Passive x: " << x << " | y: " << y << std::endl;
	return;
}

void Input::MouseMove(int x, int y)
{
	//Updates the mouse x and y position
	//std::cout << "Clicked x: " << x << " | y: " << y << std::endl;
	mouseX = x;
	mouseY = y;
	return;
}