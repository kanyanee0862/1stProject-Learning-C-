#include <vector>
#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
	:mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mPaddleDir(0)
	, mPaddle2_Dir(0)
	,mIsover(false)
{

}

bool Game::Initialize()
{
	//Initailize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL:%s", SDL_GetError());
		return false;
	}

	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"PONG", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	//Create SDL Renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, //Window to create renderer for
		-1,		 //Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
		
	//2nd
	mPaddle2_Pos.x = (1024.0f - thickness*2) -10; //右のほうは壁の厚さあるため、パドルの厚さとたしないといけない
	mPaddle2_Pos.y = 768.0f / 2.0f;

	ball.push_back(Ball());
	ball[0].pos = { 1024.0f / 2.0f,768.0f / 2.0f };
	ball[0].vel = { -200.0f,235.0f };
	ball.push_back(Ball());
	ball[1].pos = { 1024.0f / 2.0f,768.0f / 2.0f };
	ball[1].vel = { 200.0f,-235.0f };
	return true;

}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}

}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// Update paddle direction based on Up/Down keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_UP])
	{
		mPaddleDir -= 2;
	}
	if (state[SDL_SCANCODE_DOWN])
	{
		mPaddleDir += 2;
	}

	//Update paddle2 direction based on W/S keys
	mPaddle2_Dir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddle2_Dir -= 2;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddle2_Dir += 2;
	}
}
void Game::UpdateGame()
{
	//Wait until 16ms has elapse since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	

	
	//もしゲームオバーではなければ
	if (!mIsover)
	{
		//Delta time is the difference in ticks from last frame
		//(converted to seconds)
		float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f; //ms から sに

		//Clamp maximum delta time value
		if (deltaTime > 0.05f)
		{
			deltaTime = 0.05f;
		}
		//Update tick counts(for next frame)
		mTicksCount = SDL_GetTicks();

		//Update paddle position based on direction
		if (mPaddleDir != 0)
		{
			mPaddlePos.y += mPaddleDir * 300.0f*deltaTime;
			//make sure paddle doesn't move off the screen
			if (mPaddlePos.y < (paddleH / 2.0f) + thickness)
			{
				mPaddlePos.y = paddleH / 2.0f + thickness;
			}
			else if (mPaddlePos.y >(768.0f - paddleH / 2.0f - thickness))
			{
				mPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}

		//Update paddle2 position based on direction
		if (mPaddle2_Dir != 0)
		{
			mPaddle2_Pos.y += mPaddle2_Dir * 300.0f*deltaTime;
			//make sure paddle doesn't move off the screen
			if (mPaddle2_Pos.y < (paddleH / 2.0f) + thickness)
			{
				mPaddle2_Pos.y = paddleH / 2.0f + thickness;
			}
			else if (mPaddle2_Pos.y > (768.0f - paddleH / 2.0f - thickness))
			{
				mPaddle2_Pos.y = 768.0f - paddleH / 2.0f - thickness;
			}
		}
	
		//Update ball position based on ball velocity
		ball[0].pos.x += ball[0].vel.x*deltaTime;
		ball[0].pos.y += ball[0].vel.y*deltaTime;

		//Update ball2 position
		ball[1].pos.x += ball[1].vel.x*deltaTime;
		ball[1].pos.y += ball[1].vel.y*deltaTime;

		//Bounce if needed
		//Check if we intersect with paddle?
		float diff = mPaddlePos.y - ball[0].pos.y;
		float diff2 = mPaddle2_Pos.y - ball[0].pos.y;
				
		
		//Take absolute value of difference
		diff = (diff > 0.0f) ? diff : -diff;
		diff2 = (diff2 > 0.0f) ? diff2 : -diff2;

		if (
			//Our y-difference is small enough
			diff <= paddleH / 2.0f && 
			//Ball is at the correct x-position
			ball[0].pos.x <= 25.0f && ball[0].pos.x >= 20.0f &&
			//The ball is moving to the left
			ball[0].vel.x < 0.0f)
		{
			ball[0].vel.x *= -1.5f;
		}
		if(
			//Our y-difference is small enough
			diff2 <= paddleH / 2.0f&&
			//Ball is at the correct x-position
			ball[0].pos.x <= mPaddle2_Pos.x && ball[0].pos.x >= mPaddle2_Pos.x -5.0f &&
			//The ball is moving to the left
			ball[0].vel.x > 0.0f)
		{
			ball[0].vel.x *= -1.5f;
		}
		//Did the ball go off the screen?(if so, end game) 
		else if (ball[0].pos.x <= 0.0f || ball[0].pos.x >= 1024.0f + thickness)
		{
			mIsover = true;
		}
		//// Did the ball collide with the right wall?
		//else if (ball[0].pos.x >= (1024.0f - thickness) && ball[0].vel.x > 0.0f)
		//{
		//	ball[0].vel.x *= -1.0f;
		//}

		//Did the ball collide with the top wall?
		if (ball[0].pos.y <= thickness && ball[0].vel.y < 0.0f )
		{
			ball[0].vel.y *= -1;
		}
		//Did the ball collide with the bottom wall?
		else if (ball[0].pos.y >= (768 - thickness) && ball[0].vel.y > 0.0f)
		{
			ball[0].vel.y *= -1;
		}
		//Ball2 bouncing and collider

		//Did the ball2 collide with the top wall?
		if (ball[1].pos.y <= thickness && ball[1].vel.y < 0.0f)
		{
			ball[1].vel.y *= -1;
		}
		//Did the ball2 collide with the bottom wall?
		else if (ball[1].pos.y >= (768 - thickness) && ball[1].vel.y > 0.0f)
		{
			ball[1].vel.y *= -1;
		}
	}
	else
	{
		mIsRunning = false;
		
	}

}

void Game::GenerateOutput()
{
	//Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		//R
		0,		//G
		255,	//B
		255		//A
	);

	//Clear back buffer
	SDL_RenderClear(mRenderer);

	//Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	//Draw top wall
	SDL_Rect wall{
		0,			//Top left x
		0,			//Top left y
		1024,		//Width
		thickness	//Height
	};
	SDL_RenderFillRect(mRenderer, &wall);

	//Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);

	//Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRect(mRenderer, &wall);

	//Draw paddle
	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x),
		static_cast<int>(mPaddlePos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle);
	
	//Draw paddle2
	SDL_Rect paddle2{
		static_cast<int>(mPaddle2_Pos.x),
		static_cast<int>(mPaddle2_Pos.y - paddleH / 2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddle2);

	//Draw ball
	SDL_Rect draw_ball{
		static_cast<int>(ball[0].pos.x - thickness / 2),
		static_cast<int>(ball[0].pos.y - thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &draw_ball);
	//Draw ball2
	SDL_Rect draw_ball2{
		static_cast<int>(ball[1].pos.x - thickness / 2),
		static_cast<int>(ball[1].pos.y - thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &draw_ball2);
	//Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}