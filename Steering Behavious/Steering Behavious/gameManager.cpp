#include "gameManager.h"



gameManager::gameManager()
{
}


gameManager::~gameManager()
{
}

//Create global variables
GLuint program;

//Create audioManager
audioManager audioM;
FMOD::Channel* channel;
FMOD::Channel* channelBG;

//Create TexLabel
TextLabel* label;
TextLabel* MainMenuLabel;	//For Menu Text
TextLabel* WonLabel;		//For winning message


//Initialise the clock
auto start = std::chrono::steady_clock::now();
auto end = std::chrono::steady_clock::now();
std::chrono::duration<double> diff;

//Initialize static input variables
Input::InputState Input::KeyState[255];
Input::InputState Input::MouseState[3];	//Mouse currently not used
int Input::mouseX = 0;	//Initialised to 0
int Input::mouseY = 0;	//Initialised to 0
int Input::downState = GLUT_DOWN;

//Initialise global camera
camera Camera;

//Projection view model matrix to be used in update/rendering
glm::mat4 pvm;

bool RunGame = false;	//Used for lose condition/main menu

//Initialise player object
Object player;
Object enemies[5];	//bool (IsJumping) to trigger choose direction and bool (isRunning) to store if travelling right or left
Object platform;
Object BackGround;
unsigned int score = 0;
bool GameWon;

bool CanJump = true;	//Only used for player so is left out of Object class

//Vertices for quad
GLfloat quadVertices[] = {
	//pos					//colour				//Texture Coords
	-50.0f, 50.0f, 0.0f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//T L
	-50.0f, -50.0f, 0.0f,			1.0f, 0.0f, 1.0f,	0.0f, 1.0f,		//B L
	50.0f, -50.0f, 0.0f,			1.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//B R
	50.f, 50.0f, 0.0f,				0.0f, 0.0f, 1.0f,	1.0f, 0.0f,		//T R

};

//Indices for quad
GLuint indices2[] = {
	0, 2, 3,	//First tri
	0, 1, 2,	//Second tri
};

GLuint VBO;
GLuint VAO;
GLuint EBO;

GLuint BGtexture;
GLuint * playerTex = new GLuint[14];	//Textures kept seperate from object class as the amount of textures
										//needed differs for each object
GLuint * enemyTex = new GLuint[3];
GLuint platformTex;
float frame;	//Used to animate textures
int JumpFrame;	//Used to play the jumping animation once only per jump
				//Initialise background
glm::mat4 BGtranslationMatrix = glm::translate(glm::mat4(), glm::vec3(player.pos.x, 0.0f, 0.0f));
glm::vec3 BGScale = glm::vec3(10.0f, 10.0f, 10.0f);
glm::mat4 BGscaleMatrix = glm::scale(glm::mat4(), BGScale);

void  Update()
{
	//Calculate delta time(diff)
	end = std::chrono::steady_clock::now();
	diff = end - start;
	start = end;

	//Update audioSystem
	audioM.audioSystem->update();
	if (audioM.MuteTimer < 1.0f)
	{
		audioM.MuteTimer += (float)diff.count();
	}

	if (RunGame == true)
	{
		//Process Input (updates acceleration info which is used to calculate distance using delta time
		if (Input::ProcessInput('d') > 0)
		{
			//Reverse direction for quick turning
			if (player.acceleration.x < 0)
			{
				player.IsRunning = true;
				player.acceleration.x = -player.acceleration.x;
			}
			else
			{
				player.IsRunning = true;
				player.acceleration.x = 80.0f;
				//player.acceleration.x += Input::ProcessInput('d')*80.0f;
			}
		}
		else if (Input::ProcessInput('a') > 0)
		{
			//Reverse direction for quick turning
			if (player.acceleration.x > 0)
			{
				player.IsRunning = true;
				player.acceleration.x = -player.acceleration.x;
			}
			else
			{
				player.IsRunning = true;
				player.acceleration.x = -80.0f;
				//player.acceleration.x -= Input::ProcessInput('a')*80.0f;
			}
		}
		else
		{
			player.IsRunning = false;
			//Player is not trying to move and should be slowed down quickly
			//Simulate friction
			if (player.acceleration.x > 0.0f)
			{
				player.acceleration.x -= 2.5f;
			}
			else if (player.acceleration.x < 0.0f)
			{
				player.acceleration.x += 2.5f;
			}

		}

		for (int i = 0; i < 5; i++)
		{
			//Enemy horizontal movement
			if (enemies[i].IsRunning == true)
			{
				//should be moving right
				//Reverse direction for quick turning
				if (enemies[i].acceleration.x < 0)
				{
					enemies[i].acceleration.x = -enemies[i].acceleration.x;
				}
				else
				{
					enemies[i].acceleration.x = 4.0f;
				}
			}
			else if (enemies[i].IsRunning == false)
			{
				//Should be moving left
				//Reverse direction for quick turning
				if (enemies[i].acceleration.x > 0)
				{
					enemies[i].acceleration.x = -enemies[i].acceleration.x;
				}
				else
				{
					enemies[i].acceleration.x = -4.0f;
				}
			}

		}
		if (Input::ProcessInput(' ') > 0 && CanJump == true)
		{
			//Play jump sound effect
			audioM.result = audioM.audioSystem->playSound(audioM.fxJump, 0, false, &channel);
			CanJump = false;
			//Jumping off ground
			player.IsJumping = true;
			JumpFrame = 12;
			player.acceleration.y += 100.0f;

		}
		else if (player.pos.y > -100.0f)	//Checking if still in air
		{
			//Falling
			player.acceleration.y -= 1.5f;

		}
		else
		{
			//Reset jump
			player.IsJumping = false;
			player.acceleration.y = 0;
			//Sometimes glitches and game starts with player lower than the ground platform
			if (player.pos.y < -100.0f)
			{
				player.pos.y = -100.0f;
			}
			CanJump = true;
		}

		for (int i = 0; i < 5; i++)
		{
			//Enemy jump calculation
			if (enemies[i].IsJumping == false)	//Enemy is on the ground
			{
				enemies[i].IsJumping = true;
				////Jumping off ground
				//enemy.acceleration.y += 8.0f;
				////Update direction (isrunning depending on the player location
				if (player.pos.x > enemies[i].pos.x)
				{
					//Should move right
					enemies[i].IsRunning = true;
				}
				else
				{
					//Should move left
					enemies[i].IsRunning = false;
				}
			}
			else if (enemies[i].pos.y > -120.0f)	//Checking if still in air
			{
				//Falling
				enemies[i].acceleration.y -= 1.5f;

			}
			else
			{
				//Not falling and can jump again
				//Reset jump
				enemies[i].acceleration.y = 180.0f;
				//Sometimes glitches and game starts lower than the ground platform
				if (enemies[i].pos.y < -120.0f)
				{
					enemies[i].pos.y = -120.0f;
				}
				enemies[i].IsJumping = false;
			}

		}

		//Physics calculation
		//Check to see if player is colliding with the left wall
		if (player.pos.x < -800.0f - 8.0f*50.0f)
		{
			//Colliding with wall (player cannot travel left anymore)
			player.pos.x = -800.0f - 8.0f*50.0f;
			player.acceleration.x = -player.acceleration.x;	//Bounce off wall
		}

		player.pos.x += player.acceleration.x * (float)diff.count() * (float)diff.count()*200.0f;
		player.pos.y += player.acceleration.y * (float)diff.count() * (float)diff.count()*100.0f;

		for (int i = 0; i < 5; i++)
		{
			enemies[i].pos.x += enemies[i].acceleration.x*(float)diff.count()*10.0f;	//Acceleration used as velocity to make the enemy slower
			enemies[i].pos.y += enemies[i].acceleration.y * (float)diff.count() * (float)diff.count()*10.0f;
		}

		//Make camera and bg follow player
		Camera.SetPos(player.pos.x, 0.0f);

		BGtranslationMatrix = glm::translate(glm::mat4(), glm::vec3(player.pos.x, 0.0f, 0.0f));
		BGScale = glm::vec3(10.0f, 10.0f, 10.0f);
		BGscaleMatrix = glm::scale(glm::mat4(), BGScale);
		BackGround.model = BGtranslationMatrix * BGscaleMatrix;


		//Update game info

		//Translation matrix
		glm::vec3 objPosition = glm::vec3(0.5f*100.0f, 0.5f*100.0f, 0.0f);	//All position values need to be multiplied by 400
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), objPosition);

		//Rotation matrix
		glm::vec3 rotationAxisZ = glm::vec3(0.0f, 0.0f, 1.0f);
		float rotationAngle = 180;
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(rotationAngle) * (frame / 5.5f), rotationAxisZ);



		glm::vec3 objScale = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), objScale);

		glm::mat4 model = translationMatrix * rotationZ * scaleMatrix;

		//Initialise player.model
		translationMatrix = glm::translate(glm::mat4(), player.pos);
		objScale = glm::vec3(0.80f, 0.80f, 0.80f);
		scaleMatrix = glm::scale(glm::mat4(), objScale);
		player.model = translationMatrix * scaleMatrix;


		//Initialise EnemyModel
		for (int i = 0; i < 5; i++)
		{
			translationMatrix = glm::translate(glm::mat4(), enemies[i].pos);
			objScale = glm::vec3(0.4f, 0.4f, 0.4f);	//Same scale as player
			scaleMatrix = glm::scale(glm::mat4(), objScale);
			enemies[i].model = translationMatrix * scaleMatrix;
		}

		//Initialise PlatformModel
		translationMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, -150.0f, 0.0f));
		objScale = glm::vec3(9.0f, 0.2f, 1.0f);	//Larger scale
		scaleMatrix = glm::scale(glm::mat4(), objScale);
		platform.model = translationMatrix * scaleMatrix;

		//Initialise pvm matrix with model 1
		pvm = Camera.GetprojView() * model;

		//Update frame counter which is used to
		//animate the textures and rotate the hexagons
		frame += 0.1f;
		if (frame > 14.0f)
		{
			frame = 1;
		}
		glutPostRedisplay();	//Function in glutDisplayFund() is called

		//Update UI elements
		std::string text = "Enemies jumped: " + std::to_string(score);
		label->SetText(text);
		label->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		label->SetScale(0.5f);
		label->SetPosition(glm::vec2(-350.0f, 250.0f));

		//Calculate score
		int tempScore = 0;
		for (int i = 0; i < 5; i++)
		{

			if (enemies[i].pos.x < player.pos.x)
			{
				tempScore++;
			}
		}
		score = tempScore;


		for (int i = 0; i < 5; i++)
		{
			//Check lose condition
			if (abs(player.pos.x - enemies[i].pos.x) < 50.0f && abs(player.pos.y - enemies[i].pos.y) < 60.0f)
			{
				//Collided with enemy
				//Game Lost
				//Set RunGame to false
				RunGame = false;

				std::string text = "High Altitude: A game by Jacob Gallagher";
				label->SetText(text);
				label->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
				label->SetScale(0.6f);
				label->SetPosition(glm::vec2(-350.0f, -50.0f));

				//Reset Player, Enemy and camera
				player.pos = glm::vec3(-100.0f, -100.0f, 0.0f);
				Camera.SetPos(player.pos.x, 0.0f);
				for (int i = 0; i < 5; i++)
				{
					enemies[i].pos.x = (float)(((rand() % 10) + 1) * 100);	//Spawn enemy at a semi random location
					enemies[i].pos.y = 0.0f;
					enemies[i].acceleration = glm::vec2(0.0f, 0.0f);
					enemies[i].IsJumping = false;
					enemies[i].IsRunning = true;	//True if moving right, false if moving left
				}
			}
		}
		//Check win condition
		if (player.pos.x > 550.0f)
		{
			GameWon = true;
			//Set RunGame to false
			RunGame = false;
			//Ensure wintext is updated with the proper score
			std::string WinText = "You won! Congrats! You jumped over: " + std::to_string(score) + " enemies.";
			WonLabel->SetText(WinText);
			//Reset Player, Enemy and camera
			player.pos = glm::vec3(-100.0f, -100.0f, 0.0f);
			Camera.SetPos(player.pos.x, 0.0f);
			for (int i = 0; i < 5; i++)
			{
				enemies[i].pos.x = (float)(((rand() % 10) + 1) * 100);	//Spawn enemy at a semi random location
				enemies[i].pos.y = 0.0f;
				enemies[i].acceleration = glm::vec2(0.0f, 0.0f);
				enemies[i].IsJumping = false;
				enemies[i].IsRunning = true;	//True if moving right, false if moving left
			}
		}
	}
	else
	{
		//Main menu update functionality goes here
		//Update UI elements for main menu
		std::string text = "High Altitude: A game by Jacob Gallagher";
		label->SetText(text);
		label->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		label->SetScale(0.6f);
		label->SetPosition(glm::vec2(-350.0f, -50.0f));

		std::string MainMenuText = "Press 'space' to play. 'm' to mute. 'q' to quit. Jump over enemies to reach the end";
		MainMenuLabel->SetText(MainMenuText);
		MainMenuLabel->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
		MainMenuLabel->SetScale(0.4f);
		MainMenuLabel->SetPosition(glm::vec2(-350.0f, -150.0f));

		if (GameWon == true)
		{
			//Display congrats message
			std::string WinText = "You won! Congrats! You jumped over: " + std::to_string(score) + "enemies";
			WonLabel->SetText(WinText);
			WonLabel->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
			WonLabel->SetScale(0.6f);
			WonLabel->SetPosition(glm::vec2(-350.0f, -250.0f));
		}

		//Reset camera and background to ensure correct placement
		Camera.SetPos(player.pos.x, 0.0f);
		BGtranslationMatrix = glm::translate(glm::mat4(), glm::vec3(player.pos.x, 0.0f, 0.0f));
		BGScale = glm::vec3(10.0f, 10.0f, 10.0f);
		BGscaleMatrix = glm::scale(glm::mat4(), BGScale);
		BackGround.model = BGtranslationMatrix * BGscaleMatrix;

		//Check for Main Menu Inputs
		if (Input::ProcessInput(' ') > 0)
		{
			RunGame = true;
			GameWon = false;
		}
		if (Input::ProcessInput('q') > 0)
		{
			glutLeaveMainLoop();	//Quit game
		}
		if (Input::ProcessInput('m') > 0)
		{
			//Mute game
			//Check if enough time has passed
			if (audioM.MuteTimer >= 1.0f)
			{
				audioM.MuteTimer = 0.0f;
				std::cout << audioM.Muted << std::endl;
				if (audioM.Muted == false)
				{
					audioM.Muted = true;
					channelBG->setVolume(0.0f);
				}
				else if (audioM.Muted == true)
				{
					audioM.Muted = false;
					channelBG->setVolume(1.0f);
				}
			}
		}
	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//Render code will be written here
	glUseProgram(program);

	//Quad
	glBindVertexArray(VAO);	//Bind VAO

	//Render the background
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, BGtexture);
	glUniform1i(glGetUniformLocation(program, "tex"), 1);

	pvm = Camera.GetprojView() * BackGround.model;
	GLuint pvmLoc = glGetUniformLocation(program, "pvm");
	glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Check if game is running(instead of main menu)
	if (RunGame == true)
	{
		//Rebind texures to the new texture
		//***Need to change logic for which texture to use
		if (player.IsJumping == true)
		{
			//Play jumping animation
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, playerTex[JumpFrame]);
			glUniform1i(glGetUniformLocation(program, "tex"), 0);
			if (JumpFrame < 13)
			{
				JumpFrame++;	//Next time Renders the next frame as jump animation hasn't played fully
			}

		}
		else if (player.IsRunning == true)
		{
			//Play running animation
			if ((int)frame % 12 < 4)
			{
				//This is to ensure that the frame counter is between 4 and 12 for running animation
				frame = 4;
			}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, playerTex[(int)frame % 12]);
			glUniform1i(glGetUniformLocation(program, "tex"), 0);
		}
		else
		{
			//Play idle animation
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, playerTex[(int)frame % 4]);
			glUniform1i(glGetUniformLocation(program, "tex"), 0);
		}


		pvm = Camera.GetprojView() * player.model;
		pvmLoc = glGetUniformLocation(program, "pvm");
		glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Draw enemy using the same VAO and static program as the player
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, enemyTex[(int)frame % 3]);
		glUniform1i(glGetUniformLocation(program, "tex"), 0);

		for (int i = 0; i < 5; i++)
		{
			pvm = Camera.GetprojView() * enemies[i].model;
			pvmLoc = glGetUniformLocation(program, "pvm");
			glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		//Draw platforms
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, platformTex);
		glUniform1i(glGetUniformLocation(program, "tex"), 0);
		for (int i = -2; i < 3; i++)
		{
			//Update location of new platform
			//Initialise new PlatformModel
			glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3((i * 400.0f), -150.0f, 0.0f));
			glm::vec3 objScale = glm::vec3(9.0f, 0.2f, 1.0f);	//Larger scale
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(), objScale);
			platform.model = translationMatrix * scaleMatrix;
			//Draw platform
			pvm = Camera.GetprojView() * platform.model;
			pvmLoc = glGetUniformLocation(program, "pvm");
			glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		//Draw a wall on the left hand side of the map
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), glm::vec3((-800.0f - 9.0f*50.0f), 0.0f, 0.0f));
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 objScale = glm::vec3(9.0f, 0.2f, 1.0f);	//Larger scale
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), objScale);
		platform.model = translationMatrix * rotationMatrix * scaleMatrix;
		//Draw wall
		pvm = Camera.GetprojView() * platform.model;
		pvmLoc = glGetUniformLocation(program, "pvm");
		glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		glUseProgram(0);
	}
	//Render UI elements
	label->Render();

	//Render extra text for main menu
	if (RunGame == false)
	{
		MainMenuLabel->Render();

		if (GameWon == true)
		{
			WonLabel->Render();
		}
	}

	glutSwapBuffers();
}

void
gameManager::Run()
{
	for (int i = 0; i < 5; i++)
	{
		enemies[i].pos.x = (float)(((rand() % 10) + 1) * 100);	//Spawn enemy at a semi random location
		enemies[i].pos.y = 0.0f;
		enemies[i].acceleration = glm::vec2(0.0f, 0.0f);
		enemies[i].IsJumping = false;
		enemies[i].IsRunning = true;	//True if moving right, false if moving left
	}

	//Initialise audio system
	if (audioM.AudioInit() == false)
	{
		return;
	}
	//Start playing bgMusic
	audioM.result = audioM.audioSystem->playSound(audioM.trackBackground, 0, false, &channelBG);
	audioM.Muted = false;
	audioM.MuteTimer = 0.0f;

	//Create textlabel class
	label = new TextLabel("Score: ", "Resources/Fonts/arial.ttf", glm::vec2(-150.0f, 100.0f));

	MainMenuLabel = new TextLabel("Score: ", "Resources/Fonts/arial.ttf", glm::vec2(-150.0f, 100.0f));

	WonLabel = new TextLabel("You won! Congrats!", "Resources/Fonts/arial.ttf", glm::vec2(-350.0f, -250.0f));
	WonLabel->SetColor(glm::vec3(0.0f, 0.0f, 0.0f));
	WonLabel->SetScale(0.6f);

	//Initialise player
	GameWon = false;
	player.pos = glm::vec3(-100.0f, 0.0f, 0.0f);
	player.acceleration = glm::vec2(0.0f, 0.0f);
	Camera.SetPos(player.pos.x, 0.0f);
	BGtranslationMatrix = glm::translate(glm::mat4(), glm::vec3(player.pos.x, 0.0f, 0.0f));
	BGScale = glm::vec3(10.0f, 10.0f, 10.0f);
	BGscaleMatrix = glm::scale(glm::mat4(), BGScale);
	BackGround.model = BGtranslationMatrix * BGscaleMatrix;
	//Sets the clear colour when calling glClear()
	glClearColor(0.0f, 0.4f, 0.6f, 1.0f); //Red

	program = ShaderLoader::ShaderLoader::CreateProgram("Resources/Shaders/VertexShader.txt",
		"Resources/Shaders/FragmentShaderStatic.txt");

	//VAO, VBO and EBO for quad
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);
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

	//Colour
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),	//Stride of a single vertex (for pos + colour + tex)
		(GLvoid*)(3 * sizeof(GLfloat)));			//Offest from vertex start
	glEnableVertexAttribArray(1);

	//Tex
	glVertexAttribPointer(
		2,
		2,		//2 float components for coordnates
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(GLfloat),	//Stride of a single vertex (for pos + col + tex)
		(GLvoid*)(6 * sizeof(GLfloat)));	//Offest from vertex start
	glEnableVertexAttribArray(2);
	//Unbind VAO
	glBindVertexArray(0);

	//BG texture
	glGenTextures(1, &BGtexture);
	glBindTexture(GL_TEXTURE_2D, BGtexture);
	utility::loadTexture(&BGtexture, "Resources/Textures/Sprites/clouds.png");

	for (int i = 0; i < 3; i++)
	{
		glGenTextures(1, &enemyTex[i]);
		glBindTexture(GL_TEXTURE_2D, enemyTex[i]);
		std::string filename = "Resources/Textures/Sprites/NPC's/bluepatrol/azul" + std::to_string(i + 1) + ".png";
		utility::loadTexture(&enemyTex[i], filename.c_str());
	}

	//Platform texture
	glGenTextures(1, &platformTex);
	glBindTexture(GL_TEXTURE_2D, platformTex);
	utility::loadTexture(&platformTex, "Resources/Textures/Sprites/platform.png");

	//idle textures
	for (int i = 0; i < 4; i++)
	{
		glGenTextures(1, &playerTex[i]);
		glBindTexture(GL_TEXTURE_2D, playerTex[i]);
		std::string filename = "Resources/Textures/Sprites/Player/idle/anim" + std::to_string(i + 1) + ".png";
		utility::loadTexture(&playerTex[i], filename.c_str());
	}

	//run textures
	for (int i = 4; i < 12; i++)
	{
		glGenTextures(1, &playerTex[i]);
		glBindTexture(GL_TEXTURE_2D, playerTex[i]);
		std::string filename = "Resources/Textures/Sprites/Player/run/anim" + std::to_string(i + 1) + ".png";
		utility::loadTexture(&playerTex[i], filename.c_str());
	}

	//jump textures
	for (int i = 12; i < 14; i++)
	{
		glGenTextures(1, &playerTex[i]);
		glBindTexture(GL_TEXTURE_2D, playerTex[i]);
		std::string filename = "Resources/Textures/Sprites/Player/jump/anim" + std::to_string(i + 1) + ".png";
		utility::loadTexture(&playerTex[i], filename.c_str());
	}

	//Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	Camera.SetprojView();

	//Register Callbacks
	Update();
	glutDisplayFunc(Render);
	glutIdleFunc(Update);

	glutKeyboardFunc(Input::KeyBoardDown);
	glutKeyboardUpFunc(Input::KeyBoardUp);
	glutMouseFunc(Input::MouseClick);
	glutMotionFunc(Input::MouseMove);
	glutPassiveMotionFunc(Input::MousePassiveMove);
	return;
}

void
gameManager::ShutDown()
{
	audioM.Release();
	delete label;
	label = 0;
	delete MainMenuLabel;
	MainMenuLabel = 0;
	delete WonLabel;
	WonLabel = 0;
	delete playerTex;
	playerTex = 0;
	glDeleteProgram(program);
	return;
}