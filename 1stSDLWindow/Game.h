//Game Programming in C++ by Sanjay Madhav
#pragma once
#include <vector>
#include "SDL.h"

//ベクトルの構造体
struct Vector2
{
	float x;
	float y;
};
struct Ball
{
	Vector2 pos;
	Vector2 vel;
};
//Game Class
class Game
{
public:
	//constructor?
	Game();
	//initialize the Game
	bool Initialize();
	//Runs the game loop until the game is over
	void RunLoop();
	//Shutdown the Game
	void Shutdown();
private:
	//Helper functions for the game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	//Windows created by SDL
	SDL_Window* mWindow;
	//Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	//Number of ticks since start of game unsigned int
	Uint32 mTicksCount;
	//Game should continue to run
	bool mIsRunning;

	//Pong specific
	//Direction of paddle
	int mPaddleDir;
	//Position of paddle
	Vector2 mPaddlePos;
	
	
	//Direction of paddle2
	int mPaddle2_Dir;
	//Position of paddle2
	Vector2 mPaddle2_Pos;

	std::vector<Ball> ball;
	
	//Check is game over
	bool mIsover;
};