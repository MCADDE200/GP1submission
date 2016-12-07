/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();

int score = 0;
string strScore;
string strFinalScore;
string savedScore;


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
//Creates an instance of cGame if one is not already created
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	//Set up the tecture, font and sound managers
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Set filename
	theFile.setFileName("Data/Score.dat");

	//Set the area clicked by the mouse to be 0
	theAreaClicked = { 0, 0 };
	// Store the textures
	textureName = {"theZombie", "bullet", "thePlayer","theBackground", "playingBackground", "endBackground"};
	texturesToUse = { "Images\\zombieSprite.png", "Images\\bullet.png", "Images\\playerSprite.png", "Images\\MainMenu.png", "Images\\PlayingScreen.png", "Images\\EndScreen.png"};
	//Associate the texture names with the actual textures that we use
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	//Store the textures for the buttons
	btnNameList = { "exit_btn", "load_btn", "menu_btn", "play_btn", "save_btn"};
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png" };
	//Set up positions for the buttons
	btnPos = { { 400, 375 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }};
	//Associate the texture names with the actual textures that we use
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	//Add the buttons to the button manager using their textures, positions and the dimensions
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	//Set the game state to the menu so that the menu scene is rendered first
	theGameState = MENU;
	theBtnType = EXIT;
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge", "zombie" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/colonnaMT.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	//Create text that we will display in the game
	gameTextNames = { "aTxt", "ScoreTxt", "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt" };
	gameTextList = { "Asteroids", "Score: ", "Zombies", "Fight the Undead Horde!", "Shoot to Survive!", "Thanks for playing!", "See you again soon!"};
	//Associate the names with their text and give them a font and colour
	for (int text = 0; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("zombie")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	}
	
	//Create a title and score texture
	theTextureMgr->addTexture("Title", theFontMgr->getFont("zombie")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	theTextureMgr->addTexture("Score", theFontMgr->getFont("zombie")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "shot", "zombieDeath", "click" };
	soundTypes = { MUSIC, SFX, SFX, SFX }; //defines the type of sound
	soundsToUse = { "Audio/Jamie T - Zombie.mp3", "Audio/shot.mp3", "Audio/zombieDeath.wav", "Audio/SFX/ClickOn.wav" }; //links to the sound file locations
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]); //Adds the sounds to the sound manager
	}

	theSoundMgr->getSnd("theme")->play(-1); //-1 loops the music indefinetly

	//Sets up the background
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
	
	//Sets up the player
	theRocket.setSpritePos({ 500, 350 });
	theRocket.setTexture(theTextureMgr->getTexture("thePlayer"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("thePlayer")->getTWidth(), theTextureMgr->getTexture("thePlayer")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	for (int astro = 0; astro < zombieCount; astro++)
	{
		//Makes sure the random number selected has not been used before to avoid enemies spawing in the same location
		tempRand = rand() % 7;
		randNum[astro + 1] = tempRand;
		for (int i = 0; i < zombieCount; i++)
		{
			switch (i)
			{
			case 0:
			{
				while (tempRand == randNum[0])
				{
					tempRand = rand() % 7;
				}
			}
			break;
			case 1:
			{
				while (tempRand == randNum[0] || tempRand == randNum[1])
				{
					tempRand = rand() % 7;
				}
			}
			break;
			case 2:
			{
				while (tempRand == randNum[0] || tempRand == randNum[1] || tempRand == randNum[2])
				{
					tempRand = rand() % 7;
				}
			}
			break;
			case 3:
			{
				while (tempRand == randNum[0] || tempRand == randNum[1] || tempRand == randNum[2] || tempRand == randNum[3])
				{
					tempRand = rand() % 7;
				}
			}
			break;
			default:
				break;
			}
			randNum[astro] = tempRand;
		}
		//Sets the positions of the enemies based on the random number givem
		theAsteroids.push_back(new cAsteroid);
		switch (tempRand)
		{
		case 0:
		{
			theAsteroids[astro]->setSpritePos({ -200, 384 });
		}
		break;
		case 1:
		{
			theAsteroids[astro]->setSpritePos({ 512, 900 });
		}
		break;
		case 2:
		{
			theAsteroids[astro]->setSpritePos({ 512, -200 });
		}
		break;
		case 3:
		{
			theAsteroids[astro]->setSpritePos({ 1300, 384 });
		}
		break;
		case 4:
		{
			theAsteroids[astro]->setSpritePos({ -200, -200 });
		}
		break;
		case 5:
		{
			theAsteroids[astro]->setSpritePos({ -200, 900 });
		}
		break;
		case 6:
		{
			theAsteroids[astro]->setSpritePos({ 1300, -200 });
		}
		break;
		case 7:
		{
			theAsteroids[astro]->setSpritePos({ 1300, 900 });
		}
		break;
		default:
			break;
		}
		// Create vector array of textures
		theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture("theZombie"));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture("theZombie")->getTWidth(), theTextureMgr->getTexture("theZombie")->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[astro]->setActive(true);
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);//Clear the renderer
	//Render different objects based on the current gamestate
	switch (theGameState)
	{
	case MENU:
	{
		//Render the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		//Render the messages to be displayed
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CreateTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Buttons
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 400, 300});
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		//Render the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("playingBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("playingBackground")->getTWidth(), theTextureMgr->getTexture("playingBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		//Renders another enemy if any are destroyed
		if (zombiesKilled > 0)
		{
			for (int astro = enemySpawnCount; astro < enemySpawnCount + 1; astro++)
			{
				//Spawns an enemy at a random set position
				tempRand = rand() % 7;
				theAsteroids.push_back(new cAsteroid);
				switch (tempRand)
				{
				case 0:
				{
					theAsteroids[astro]->setSpritePos({ -200, 384 });
				}
				break;
				case 1:
				{
					theAsteroids[astro]->setSpritePos({ 512, 900 });
				}
				break;
				case 2:
				{
					theAsteroids[astro]->setSpritePos({ 512, -200 });
				}
				break;
				case 3:
				{
					theAsteroids[astro]->setSpritePos({ 1300, 384 });
				}
				break;
				case 4:
				{
					theAsteroids[astro]->setSpritePos({ -200, -200 });
				}
				break;
				case 5:
				{
					theAsteroids[astro]->setSpritePos({ -200, 900 });
				}
				break;
				case 6:
				{
					theAsteroids[astro]->setSpritePos({ 1300, -200 });
				}
				break;
				case 7:
				{
					theAsteroids[astro]->setSpritePos({ 1300, 900 });
				}
				break;
				default:
					break;
				}
				// Create vector array of textures
				theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture("theZombie"));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture("theZombie")->getTWidth(), theTextureMgr->getTexture("theZombie")->getTHeight());
				theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
				theAsteroids[astro]->setActive(true);
			}
			//Resets the variables used to check
			zombiesKilled = 0;
			enemySpawnCount++;
		}
		//Set up variables so the zombies move towards the player
		playerX = theRocket.getSpritePos().x;
		playerY = theRocket.getSpritePos().y;
		// Render each enemy in the vector array and moves them towards the player
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			zombieX= theAsteroids[draw]->getSpritePos().x;
			zombieY = theAsteroids[draw]->getSpritePos().y;
			distanceX = playerX - zombieX;
			distanceY = playerY - zombieY;
			angle = atan2(distanceY, distanceX);
			speedX = 2 * cos(angle);
			speedY = 2 * sin(angle);
			zombieX += speedX;
			zombieY += speedY;
			theAsteroids[draw]->setSpritePos({ zombieX, zombieY });
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
		
		// Render each bullet in the vector array
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}
		// Render the Title
		cTexture* tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render the Score
		theTextureMgr->addTexture("Score", theFontMgr->getFont("zombie")->createTextTexture(theRenderer, strScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		//Render the exit button
		theButtonMgr->getBtn("load_btn")->setSpritePos({ 740, 575 });
		theButtonMgr->getBtn("load_btn")->render(theRenderer, &theButtonMgr->getBtn("load_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("load_btn")->getSpritePos(), theButtonMgr->getBtn("load_btn")->getSpriteScale());
		theButtonMgr->getBtn("save_btn")->setSpritePos({ 740, 500 });
		theButtonMgr->getBtn("save_btn")->render(theRenderer, &theButtonMgr->getBtn("save_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("save_btn")->getSpritePos(), theButtonMgr->getBtn("save_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 740, 650 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		
	}
	break;
	case END:
	{
		theAreaClicked = { 0, 0 }; //Set the area clicked to be 0 so the program does not keep clicking the same button
		//Render the background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("endBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("endBackground")->getTWidth(), theTextureMgr->getTexture("endBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		//Renders the messages for the end screen
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		// Render the Score
		theTextureMgr->addTexture("Score2", theFontMgr->getFont("zombie")->createTextTexture(theRenderer, strFinalScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("Score2");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theTextureMgr->deleteTexture("Score2");
		//Render the buttons on the exit menu
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 500, 425 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT: //End the game
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	//Check button clicked and change state
	if (theGameState == MENU || theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		//Updates the gamestate and resets the score
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
		theAreaClicked = { 0, 0 };

	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
		if (theButtonMgr->getBtn("exit_btn")->getClicked())
		{
			finalScore = score;
			strFinalScore = gameTextList[1] + to_string(score);
			score = 0;
			strScore = gameTextList[1] + to_string(score);
			endCheck = true; //Resets the game
			theButtonMgr->getBtn("exit_btn")->setClicked(false); //Reset the exit button
			theAreaClicked = { 0, 0 };
			theButtonMgr->getBtn("play_btn")->setClicked(false);
		}
	}
	if (theGameState == END)
	{
		endCheck = false;
		theAreaClicked = { 0, 0 };
		
	}
	switch (theGameState) //Checks for the current gamestate
	{
	case PLAYING:
	{
		//Saving and loading
		theGameState = theButtonMgr->getBtn("load_btn")->update(theGameState, LOADMAP, theAreaClicked);
		if (theGameState == LOADMAP)
		{
			if (!theFile.openFile(ios::in))
			{
				cout << "could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl;
			}
			else
			{
				cout << "File '" << theFile.getFileName() << "' opened for input!" << endl;
				savedScore = theFile.readDataFromFile();
				theFile.closeFile();
				score = atoi(savedScore.c_str());
				strScore = gameTextList[1] + to_string(score);
				theTextureMgr->deleteTexture("Score");
				theGameState = PLAYING;
				theAreaClicked = { 0, 0 };
			}
		}
		theGameState = theButtonMgr->getBtn("save_btn")->update(theGameState, SAVEMAP, theAreaClicked);
		if (theGameState == SAVEMAP)
		{
			// Check file is available
			if (!theFile.openFile(ios::out)) //open file for output
			{
				cout << "Could not open specified file '" << theFile.getFileName() << "'. Error " << SDL_GetError() << endl;
			}
			else
			{
				cout << "File '" << theFile.getFileName() << "' opened for output!" << endl;
				savedScore = (to_string(score).c_str());
				theFile.writeDataToFile(savedScore);
				theFile.closeFile();
			}
			theGameState = PLAYING;
			theAreaClicked = { 0, 0 };
		}
		// Update the visibility and position of each asteriod
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}
			else
			{
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}
		// Update the visibility and position of each bullet
		vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
		while (bulletIterartor != theBullets.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = theBullets.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		}
		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and asteroid to false
					(*asteroidIterator)->setActive(false);
					(*bulletIterartor)->setActive(false);
					enemySpawnCount--; //Decreases the enemy spawning counter
					theSoundMgr->getSnd("zombieDeath")->play(0); //Plays the enemy death statement
					score += 100; // increase the score
					zombiesKilled++; // Increase the amount of zombies killed 
					strScore = gameTextList[1] + to_string(score); // Put the score into a string and concatenate it with the score
					theTextureMgr->deleteTexture("Score"); //Delete the texture
				}
			}
		}
		//Checks if an enemy collides with a player
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(theRocket.getBoundingRect)()))
			{
				finalScore = score;
				strFinalScore = gameTextList[1] + to_string(score);
				score = 0;
				strScore = gameTextList[1] + to_string(score);
				theTextureMgr->deleteTexture("Score");
				endCheck = true; //Resets the game
			}
		}


		// Update the Rockets position
		theRocket.update(deltaTime);
	}
		break;
	default:
		break;
	}
	if (endCheck)
	{
		/*
		====================================================================
		| Reset The Game
		====================================================================
		*/
		theAsteroids.clear(); //Deletes all enemies on the screen
		theBullets.clear(); //Removes all bullets from screen
		theRocket.setSpritePos({ 512, 384}); //Resets the sprites position
		theRocket.setSpriteTranslation({ 0, 0 }); //Remove movement from the sprite
		theRocket.setSpriteRotAngle({0}); //Reset the rotation angle
		//Reset the enemies into random locations
		for (int astro = 0; astro < zombieCount; astro++)
		{
			tempRand = rand() % 7;
			randNum[astro + 1] = tempRand;
			for (int i = 0; i < zombieCount; i++)
			{
				switch (i)
				{
				case 0:
				{
					while (tempRand == randNum[0])
					{
						tempRand = rand() % 7;
					}
				}
				break;
				case 1:
				{
					while (tempRand == randNum[0] || tempRand == randNum[1])
					{
						tempRand = rand() % 7;
					}
				}
				break;
				case 2:
				{
					while (tempRand == randNum[0] || tempRand == randNum[1] || tempRand == randNum[2])
					{
						tempRand = rand() % 7;
					}
				}
				break;
				case 3:
				{
					while (tempRand == randNum[0] || tempRand == randNum[1] || tempRand == randNum[2] || tempRand == randNum[3])
					{
						tempRand = rand() % 7;
					}
				}
				break;
				default:
					break;
				}
				randNum[astro] = tempRand;
			}
			theAsteroids.push_back(new cAsteroid);
			switch (tempRand)
			{
			case 0:
			{
				theAsteroids[astro]->setSpritePos({ -200, 384 });
			}
			break;
			case 1:
			{
				theAsteroids[astro]->setSpritePos({ 512, 900 });
			}
			break;
			case 2:
			{
				theAsteroids[astro]->setSpritePos({ 512, -200 });
			}
			break;
			case 3:
			{
				theAsteroids[astro]->setSpritePos({ 1300, 384 });
			}
			break;
			case 4:
			{
				theAsteroids[astro]->setSpritePos({ -200, -200 });
			}
			break;
			case 5:
			{
				theAsteroids[astro]->setSpritePos({ -200, 900 });
			}
			break;
			case 6:
			{
				theAsteroids[astro]->setSpritePos({ 1300, -200 });
			}
			break;
			case 7:
			{
				theAsteroids[astro]->setSpritePos({ 1300, 900 });
			}
			break;
			default:
				break;
			}
			theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
			theAsteroids[astro]->setTexture(theTextureMgr->getTexture("theZombie"));
			theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture("theZombie")->getTWidth(), theTextureMgr->getTexture("theZombie")->getTHeight());
			theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
			theAsteroids[astro]->setActive(true);
		}
		/*
		===================================================================

		==================================================================
		*/
		theGameState = END; //Change the gamestate to the end
		theAreaClicked = { 0, 0 };
		theButtonMgr->getBtn("play_btn")->setClicked(false);

	}
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y }; //Get the position of the pointer when the left mouse is pressed
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
					if ((theRocket.getSpritePos().x < (renderWidth - theRocket.getSpritePos().w)) && playerVelocity > -5)
					{
						playerVelocity -= 5; //Decreases the players velocity by 5
						theRocket.setSpriteTranslation({ playerVelocity, playerVelocity }); // Sets the player velocity
					}
				}
				break;

				case SDLK_UP:
				{
					if (theRocket.getSpritePos().x > 0 && playerVelocity < 5)
					{
						playerVelocity += 5; // Increases players velocity by 5
						theRocket.setSpriteTranslation({ playerVelocity, playerVelocity }); // Sets the players velocity
					}
				}
				break;
				//Used th=o change the players rotation angle
				case SDLK_RIGHT:
				{
					theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() + 15); 
				}
				break;

				case SDLK_LEFT:
				{
					theRocket.setSpriteRotAngle(theRocket.getSpriteRotAngle() - 15);
				}
				break;
				case SDLK_SPACE:
				{
					//Used to create a bullet
					theBullets.push_back(new cBullet);
					int numBullets = theBullets.size() - 1;
					theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
					theBullets[numBullets]->setSpriteTranslation({ 2.0f, 2.0f });
					theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet"));
					theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet")->getTWidth(), theTextureMgr->getTexture("bullet")->getTHeight());
					theBullets[numBullets]->setBulletVelocity({ 2.0f, 2.0f });
					theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle());
					theBullets[numBullets]->setActive(true);
					cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
				}
				if (theGameState == PLAYING)
				{
					theSoundMgr->getSnd("shot")->play(0); //Plays the sound of the bullet
				}
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

