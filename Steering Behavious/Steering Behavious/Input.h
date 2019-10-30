#include <iostream>
#pragma once



class Input
{
	enum InputState
	{
		INPUT_UP,
		INPUT_DOWN,
		INPUT_UP_FIRST,
		INPUT_DOWN_FIRST,
	};
public:
	Input();
	~Input();
	static InputState KeyState[255];
	static InputState MouseState[3];
	static int mouseX;
	static int mouseY;
	static int downState;
	static void KeyBoardDown(unsigned char key, int x, int y);
	static void KeyBoardUp(unsigned char key, int x, int y);
	static void MouseClick(int button, int state, int x, int y);
	static void MousePassiveMove(int x, int y);
	static void MouseMove(int x, int y);
	static float ProcessInput(unsigned char key);	//Returns a float with value of one if key is down
};

