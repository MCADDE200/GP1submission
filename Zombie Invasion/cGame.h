#ifndef _GAME_H
#define _GAME_H

/*
==================================================================================
cGame.h
==================================================================================
*/

#include <SDL.h>

// Game specific includes
#include "asteroidsGame.h"


using namespace std;

class cGame
{
public:
	cGame();
	//Set up the methods to be used in the cGame.cpp file

	void initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void cleanUp(SDL_Window* theSDLWND);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer);
	void render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre);
	void update();
	void update(double deltaTime);
	bool getInput(bool theLoop);
	double getElapsedSeconds();

	static cGame* getInstance();

private:

	static cGame* pInstance;
	// for framerates
	time_point< high_resolution_clock > m_lastTime;
	time_point< high_resolution_clock > m_CurrentTime;
	duration< double > deltaTime;
	bool loop;

	// Sprites for displaying background and rocket textures
	cSprite spriteBkgd;
	cRocket theRocket;
	cAsteroid theAsteroid;
	cBullet theBullet;
	// game related variables
	vector<LPCSTR> textureName;
	vector<LPCSTR> textName;
	vector<LPCSTR> texturesToUse;
	vector<cAsteroid*> theAsteroids;
	vector<cBullet*> theBullets;
	// Fonts to use
	vector<LPCSTR> fontList;
	vector<LPCSTR> fontsToUse;
	// Text for Game
	vector<LPCSTR> gameTextNames;
	vector<LPCSTR> gameTextList;
	// Game Sounds
	vector<LPCSTR> soundList;
	vector<soundType> soundTypes;
	vector<LPCSTR> soundsToUse;
	// Create vector array of button textures
	vector<LPCSTR> btnNameList;
	vector<LPCSTR> btnTexturesToUse;
	vector<SDL_Point> btnPos;
	vector <cButton> theButtons;
	// Game objects
	int renderWidth, renderHeight;
	gameState theGameState; //Define a gamestate object so we can switch between what we are rendering
	btnTypes theBtnType; //Define an object for the button types
	SDL_Point theAreaClicked; //Make an SDL point that will return the area that is cliked
	SDL_Rect pos; //Create an SDL Rectangle to store the position of boxes text and buttons
	FPoint scale; //Create an object for the scale
	cTexture* tempTextTexture; //Create a variable to hold temporary textures
	cFileHandler theFile; //Creates an instance of the file handler
	int zombieCount= 5; //An integer to store the number of zombies
	int zombiesKilled = 0; //An interger that will return if a zombie is killed
	int enemySpawnCount = 6; //A sort of working integer
	int tempRand; //A temporary random number
	int randNum[5] = {}; //An array that will store the random numbers used in the inistialisation of the enemies
	int playerX; //Store the players x location
	int playerY; //Store the players y location
	double zombieX; //Store the zombies x location
	double zombieY; //Store the zombies y location
	int distanceX; //
	int distanceY; //
	double angle; //Store the angle
	double speedX; //Store the x speed
	double speedY; //Store the y speed
	bool endCheck = false; //Check for if the player has collided with a zombie
	int playerVelocity = 0;//Stores the velocity of the player
	int finalScore; //Creates a variable to display the score in the end screen
};

#endif
