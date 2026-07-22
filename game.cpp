
#include "game.hpp"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game():
mWindow(nullptr),
mRenderer(nullptr),
mTicksCount(0),
mIsRunning(true),
mP1PaddleDir(0),
mP2PaddleDir(0){

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

	mP1PaddlePos.x = 10.0f;
	mP1PaddlePos.y = 768.0f/2.0f;
	mP2PaddlePos.x = 1014.0f;
	mP2PaddlePos.y = 768.0f/2.0f;
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	// TODO: Change initial x-velocity to be randomly Pos or Neg
	mBallVel.x = -200.0f;
	// TODO: Change initial y-velocity to be random Pos or Neg
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
	mP1PaddleDir = 0;
	mP2PaddleDir = 0;
	if (state[SDL_SCANCODE_W]) {
		mP1PaddleDir -= 1;	
	}
	if (state[SDL_SCANCODE_S]) {
		mP1PaddleDir += 1;
	}
	if (state[SDL_SCANCODE_I]) {
		mP2PaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_K]) {
		mP2PaddleDir += 1;
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

	// Update P1 paddle position based on direction
	if ( mP1PaddleDir != 0 ){
		mP1PaddlePos.y += mP1PaddleDir * 300.0f * deltaTime;
		// ensure that the paddle does not move off the screen
		if (mP1PaddlePos.y < (paddleH/2.0f + thickness)){
			mP1PaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mP1PaddlePos.y > (768.0f - paddleH/2.0f - thickness)){
			mP1PaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}
	// Update P2 paddle position based on direction
	if ( mP2PaddleDir != 0 ){
		mP2PaddlePos.y += mP2PaddleDir * 300.0f * deltaTime;
		// ensure that the paddle does not move off the screen
		if (mP2PaddlePos.y < (paddleH/2.0f + thickness)){
			mP2PaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mP2PaddlePos.y > (768.0f - paddleH/2.0f - thickness)){
			mP2PaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	// Bounce if needed
	// Did we intersect with the P1 paddle?
	float p1diff = mP1PaddlePos.y - mBallPos.y;
	// Did we intersect with the P2 paddle?
	float p2diff = mP2PaddlePos.y - mBallPos.y;
	// Take the absolute value of difference
	p1diff = (p1diff > 0.0f) ? p1diff : -p1diff;
	if (
		// our y-difference is small enough
		p1diff <= paddleH / 2.0f &&
		// we are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// the ball is moving to the left
		mBallVel.x < 0.0f){
		mBallVel.x *= -1.0f;	
	}
	// Did the ball go off the screen? If so, end the game
	else if (mBallPos.x <= 0.0f || mBallPos.x >= 1024.0f){
		mIsRunning = false;
	}
	// Did the ball collide with the P2 paddle?
	//else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f){
	//	mBallVel.x *= -1.0f;
	//}
	else if (
		// our y-difference is small enough
		p2diff <= paddleH / 2.0f &&
		// we are in the correct x-position
		mBallPos.x <= 999.0f || mBallPos.x >= 1004.0f &&
		// the ball is moving to the right
		mBallVel.x > 0.0f){
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

	// Draw paddle p1
	SDL_FRect paddle1{
		static_cast<float>(mP1PaddlePos.x),
		static_cast<float>(mP1PaddlePos.y - paddleH/2),
		thickness,
		static_cast<float>(paddleH)
	};
	SDL_RenderFillRects(mRenderer, &paddle1, 1);

	// Draw paddle p2
	SDL_FRect paddle2{
		static_cast<float>(mP2PaddlePos.x),
		static_cast<float>(mP2PaddlePos.y - paddleH/2),
		thickness,
		static_cast<float>(paddleH)
	};

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