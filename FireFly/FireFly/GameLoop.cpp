#include "GameLoop.h"
#include "Box2dWorld.h"
#include <Windows.h>
#include "MusicManager.h"
#include "Log.h"
#include "Hivemind.h"
#include <iostream>
#include "Save.h"
#include "sfeMovie\Movie.hpp"
#include "Movie.h"

const sf::Time GameLoop::TimePerFrame = sf::seconds(1.f/60.f);
void appInFocus(sf::RenderWindow* app);

GameLoop::GameLoop(sf::Vector2u windowSize)
: mWindow(sf::VideoMode(windowSize.x, windowSize.y), "Firefly", sf::Style::Default),
//: mWindow(sf::VideoMode(1600, 900), "Firefly", sf::Style::Default),
//: mWindow(sf::VideoMode::getDesktopMode(), "Firefly", sf::Style::Fullscreen),
mCamera(&mWindow),
mFont(),
mStatisticsText(),
mStatisticsUpdateTime(),
mStatisticsNumFrames(0),
cursorSprite(Loading::getTexture("pointer.png", true)),
mZidsLight(nullptr),
mSimpleMenu(&mWindow)
{
	mWindow.setMouseCursorVisible(false);
	mWindow.setVerticalSyncEnabled(true);
	mFont.loadFromFile("Resources/Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(12);
	Save::init();
	Level::init(&mWindow);
	Movie::init(&mWindow);

//	hivemindProjection.setPosition(200, 200);
}

GameLoop::~GameLoop()
{
}



void GameLoop::run()
{

	
	// Load the level "level1.tmx"
	//Level::startLevel("level1.tmx");
	Level::startLevel("intro movie.tmx");


		
	
	// Game Loop
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		// Set cursor position and correction scale       
		cursorSprite.setPosition(mCamera.getWindowMousePosition());
		cursorSprite.setScale(mCamera.getMouseScale());
	
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			
			update(TimePerFrame);
			processEvents();

		}

		updateStatistics(elapsedTime);
		draw();
		
	}
}

void GameLoop::processEvents()
{
	sf::Event event;
	Zid* zidCast = static_cast<Zid*>(zid);
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::MouseButtonPressed:
			appInFocus(&mWindow);	// Fixes window out of focus bug.
			break;

		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		case::sf::Event::MouseWheelMoved:
			//if (mZidsLight == nullptr)
			mZidsLight = EntityList::getEntityList().getEntity("zidLight");
			
			if (mZidsLight != nullptr && Globals::ZID_INPUT)
			{
				mZidsLight->sendMessage(nullptr, "ChangeRadius", event.mouseWheel.delta);
			}
				break;
		case::sf::Event::TextEntered:
			zidCast = static_cast<Zid*>(zid);
			if(zid != nullptr && zidCast->inPCZone() == true)
			{

				if (event.text.unicode == '\b' && textEntered.end() != textEntered.begin())
				{
					 textEntered.erase(textEntered.getSize() - 1, 1);	//deletes one character from string
				}
				else if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\r') //Dont add to string if enter or backspace was pressed
				{
					if(textEntered.getSize() <= 5)//sets a max length for string
					{
						textEntered += static_cast<char>(event.text.unicode);
					}
				}

				if(pc != nullptr && zid != nullptr)
				{
					 pc->sendSfString(pc, textEntered);
					 zid->sendMessage(zid, "button_pressed");
				}

				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
				{
					textEntered.clear();	//Clears string if enter is pressed
					if(pc != nullptr)
					{
						pc->sendSfString(pc, textEntered);
					}

				}

			}
			break;

		}
	}
}


void GameLoop::draw()
{
	mWindow.clear(sf::Color::Black);

	mWindow.setView(mCamera.getView());

	// Draws every layer of entities
	EntityList::getEntityList().drawBackground(mWindow);
	EntityList::getEntityList().drawBack(mWindow);
	EntityList::getEntityList().drawNPC(mWindow);
	EntityList::getEntityList().drawFront(mWindow);

	if (Globals::SHOW_LIGHT)
		EntityList::getEntityList().drawLight(mWindow);

	EntityList::getEntityList().drawForeground(mWindow);

	mWindow.setView(mWindow.getDefaultView());

	EntityList::getEntityList().drawHivemind(mWindow);

	EntityList::getEntityList().drawGUI(mWindow);

	// Ugly check if in a movie level
	if (Level::getLevel().mCurrentMap != "intro movie.tmx")
	{
		mWindow.draw(mSimpleMenu);
		mWindow.draw(cursorSprite);
	}

	if (Globals::DEBUG_MODE || Globals::SHOW_FPS)
		mWindow.draw(mStatisticsText);




	mWindow.display();
}

void GameLoop::update(sf::Time timePerFrame)
{
	// Update camera
	mCamera.update(timePerFrame);

	// Update music manager
	MusicManager::update(timePerFrame);

	// Update entities
	EntityList::getEntityList().updateList(); //deletes dead entities
	EntityList::getEntityList().update(timePerFrame);	// Updates all entities

	// Box2d physics step
	float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
	Box2dWorld::instance().Step(timeStep, velocityIterations, positionIterations);

	// Check if change map
	Level::update();

	// Update simple menu
	mSimpleMenu.update(timePerFrame);

	//pointers for event
	pc = EntityList::getEntityList().getEntity("PC");
	zid = EntityList::getEntityList().getEntity("Zid");
	//clears text when reloading map
	PC* pcCast = static_cast<PC*>(pc);
	if(pcCast != nullptr) 
	{
		if(pcCast->newPC() == true)
		textEntered.clear();
	}

}

void GameLoop::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString( 
			"Frames / Second = " + std::to_string(mStatisticsNumFrames));// + "\n" +
			//"Time / Update = " + std::to_string(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");
							 
		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0; 
	}
}

void GameLoop::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	// Ugly
	if (Level::getLevel().mCurrentMap == "intro movie.tmx")
	{			
		// Shows the loading screen and preloads all levels into memory
		//preloadLevels(Save::readLevel());

		// Starts up the game on the last level you were on	
		if (Save::readLevel() == "")
			Level::startLevel("level1.tmx");
		else
			Level::startLevel(Save::readLevel());
		
		return;
	}

	if (key == sf::Keyboard::Escape && isPressed == true)
		mSimpleMenu.toggleActive();
	//mWindow.close();

	
	if (key == sf::Keyboard::Num6 && isPressed == false)
		Globals::DEBUG_MODE = !Globals::DEBUG_MODE;
	else if (key == sf::Keyboard::Num7 && isPressed == false)
		Globals::SHOW_FPS = !Globals::SHOW_FPS;
	else if (key == sf::Keyboard::Num8 && isPressed == false)
		Globals::SHOW_LIGHT = !Globals::SHOW_LIGHT;
	

	if (isPressed == false)
	{
		switch (key)
		{	
			break;
		case sf::Keyboard::F1:
			Level::changeMap("level1.tmx");
			break;
		case sf::Keyboard::F2:
			Level::changeMap("level2.tmx");
			break;
		case sf::Keyboard::F3:
			Level::changeMap("schakt1.tmx");
			break;
		case sf::Keyboard::F4:
			Level::changeMap("level3.tmx");
			break;
		case sf::Keyboard::F5:
			Level::changeMap("schakt2.tmx");
			break;
		case sf::Keyboard::F6:
			Level::changeMap("level6.tmx");
			break;
		case sf::Keyboard::F7:
			Level::changeMap("level7.tmx");
			break;
		case sf::Keyboard::F8:
			Level::changeMap("meny.tmx");
			break;
		case sf::Keyboard::Num0:
			mCamera.changeZoom(2);
			break;
		case sf::Keyboard::Num9:
			mCamera.changeZoom(-2);
			break;
		default:
			break;
		}
	}
}

void appInFocus(sf::RenderWindow* app)
{ 
    HWND handle = app->getSystemHandle();
    
    SetFocus(handle);
    SetForegroundWindow(handle);
}

void GameLoop::preloadLevels(string startLevel)
{
	

	if (startLevel == "")
	{
		//Level::startLevel("level1.tmx");
		Level::startLevel("schakt1.tmx");	
		Level::startLevel("level2.tmx");
		//Level::startLevel("schakt2.tmx");
		//Level::startLevel("level3.tmx");		
	}
	else if (startLevel == "schakt1.tmx")
	{
		//Level::startLevel("level1.tmx");
		//Level::startLevel("schakt1.tmx");	
		Level::startLevel("level2.tmx");
		Level::startLevel("schakt2.tmx");
		//Level::startLevel("level3.tmx");
	}
	else if (startLevel == "level2.tmx")
	{
		//Level::startLevel("level1.tmx");
		//Level::startLevel("schakt1.tmx");	
		//Level::startLevel("level2.tmx");
		Level::startLevel("schakt2.tmx");
		Level::startLevel("level3.tmx");
	}
	else if (startLevel == "schakt2.tmx")
	{
		//Level::startLevel("level1.tmx");
		//Level::startLevel("schakt1.tmx");	
		//Level::startLevel("level2.tmx");
		//Level::startLevel("schakt2.tmx");
		Level::startLevel("level3.tmx");
	}
	else if (startLevel == "level3.tmx")
	{
		//Level::startLevel("level1.tmx");
		//Level::startLevel("schakt1.tmx");	
		//Level::startLevel("level2.tmx");
		//Level::startLevel("schakt2.tmx");
		//Level::startLevel("level3.tmx");
	}
}
