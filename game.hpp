#ifndef _GAME_HPP
#define _GAME_HPP

#include <SDL3/SDL.h>

//Vector2 struct just stores x/y coordinates for now
struct Vector2 {
	float x;
	float y;
};

class Game {
public:
	// constructor
	Game();
	// initializes the game
	bool Initialize();
	// runs the game loop until the game is over
	void RunLoop();
	// shuts down the game
	void Shutdown();

private:
	// helper functions for game loop
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	// window created by SDL
	SDL_Window* mWindow;
	// Renderer for 2D drawing
	SDL_Renderer* mRenderer;
	// Number of ticks since start of game
	uint32_t mTicksCount;
	// Game should continue to run
	bool mIsRunning;

	// Gameplay specific class members
	// Will first do pong
	int mPaddleDir;
	// Position of paddle
	Vector2 mPaddlePos;
	// Position of ball
	Vector2 mBallPos;
	// Velocity of ball
	Vector2 mBallVel;
};


#endif // _GAME_HPP