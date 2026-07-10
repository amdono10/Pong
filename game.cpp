
#include "game.hpp"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game():
mWindow(nullptr),
mRenderer(nullptr),
mTicksCount(0),
mIsRunning(true),
mPaddleDir(0){

}

bool Game::Initialize(){
	// Initialize SDL
	bool sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != true){
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Pong", // window title
		1024,	// Width of window
		768,	// Height of window
		0		// flags (0 for no flags set)
	);

	if (!mWindow){
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	// Create SDL Renderer
	mRenderer = SDL_CreateRenderer(
		mWindow,	// the window where rendering is displayed
		NULL		// name of rendering driver to initialize, NULL = Default
	);

	if (!mRenderer){
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f/2.0f;
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;
	return true;
}

void Game::RunLoop(){
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput(){
	SDL_Event event;
	while (SDL_PollEvent(&event)){
		switch (event.type){
			// if we get an SDL_QUIT event, end loop
			case SDL_EVENT_QUIT:
				mIsRunning = false;
				break;
		}
	}

	// Get state of Keyboard
	const bool* state = SDL_GetKeyboardState(NULL);
	// if esc is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE]){
		mIsRunning = false;
	}

	// update paddle direction based on W/S keys
	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W]) {
		mPaddleDir -= 1;	
	}
	if (state[SDL_SCANCODE_S]) {
		mPaddleDir += 1;
	}
}

void Game::UpdateGame(){
	// wait until 16ms has elapsed since last frame
	while ( !(SDL_GetTicks() >= mTicksCount + 16))
		;
	
	// delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// clamp maximum delta time value
	if (deltaTime > 0.05f){
		deltaTime = 0.05f;
	}

	// update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	if ( mPaddleDir != 0 ){
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		// ensure that the paddle does not move off the screen
		if (mPaddlePos.y < (paddleH/2.0f + thickness)){
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness)){
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	// Bounce if needed
	// Did we intersect with the paddle?
	float diff = mPaddlePos.y - mBallPos.y;
	// Take the absolute value of difference
	diff = (diff > 0.0f) ? diff : -diff;
	if (
		// our y-difference is small enough
		diff <= paddleH / 2.0f &&
		// we are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// the ball is moving to the left
		mBallVel.x < 0.0f){
		mBallVel.x *= -1.0f;	
	}
	// Did the ball go off the screen? If so, end the game
	else if (mBallPos.x <= 0.0f){
		mIsRunning = false;
	}
	// Did the ball collide with the right wall?
	else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f){
		mBallVel.x *= -1.0f;
	}

	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f){
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) && mBallVel.y > 0.0f){
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput(){
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_FRect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRects(mRenderer, &wall, 1); // 

	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRects(mRenderer, &wall, 1); // 

	// Draw right wall
	wall.x = 1024 - thickness;
	wall.y = 0;
	wall.w = thickness;
	wall.h = 1024;
	SDL_RenderFillRects(mRenderer, &wall, 1);

	// Draw paddle
	SDL_FRect paddle{
		static_cast<float>(mPaddlePos.x),
		static_cast<float>(mPaddlePos.y - paddleH/2),
		thickness,
		static_cast<float>(paddleH)
	};
	SDL_RenderFillRects(mRenderer, &paddle, 1);

	// Draw ball
	SDL_FRect ball{
		static_cast<float>(mBallPos.x - thickness/2),
		static_cast<float>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRects(mRenderer, &ball, 1);

	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown(){
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}