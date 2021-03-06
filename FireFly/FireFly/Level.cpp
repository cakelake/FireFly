#include "Level.h"
#include "Loading.h"
#include "Box2dWorld.h"
#include "Camera.h"
#include "MusicManager.h"
#include "Log.h"

#include "Zid.h"
#include "EntitySprite.h"
#include "StaticLineCollider.h"
#include "LevelBoundryCollider.h"
#include "Jar.h"
#include "Mal.h"
#include "Wasp.h"
#include "Trigger.h"
#include "Room1_Coat.h"
#include "FadeToBlack.h"
#include "ForceZone.h"
#include "StickyZone.h"
#include "ToggleSprite.h"
#include "SuCam.h"
#include "Room2_Fan.h"
#include "BalkPort.h"
#include "FireflyNPC.h"
#include "PC.h"
#include "SugarBowlTop.h"
#include "Light.h"
#include "AntPath.h"
#include "Boiler.h"
#include "Room2_AC.h"
#include "Termometer.h"
#include "Telefonsvarare.h"
#include "ParallaxSprite.h"
#include "FireflyZone.h"
#include "Dust.h"
#include "SpiderPath.h"
#include "Sparks.h"
#include "Hivemind.h"
#include "CrackZone.h"
#include "WaterDrop.h"
#include "ServerRoomDoor.h"
#include "ServerRoomButton.h"
#include "CameraScreen.h"
#include "SCHAKT2_FAN.h"
#include "ServerRoomEyeScreen.h"
#include "FadeSprite.h"
#include "EatingWasp.h"
#include "lever.h"
#include "Movie.h"


#include <iostream>
using namespace std;

Level Level::level;
sf::RenderWindow* Level::sWindow;

Level::Level()
{
}

Level::~Level()
{
}

Level& Level::getLevel()
{
	return level;
}

sf::Vector2f Level::getLevelSize()
{
	return level.mMapSize;
}
void Level::fadeToBlackChangeLevel(string filename)
{
	float fadeDelay = 3.f;
	EntityList::getEntityList().addEntity(new FadeToBlack(fadeDelay, true, filename), Layer::Foreground);
	MusicManager::fadeDownAll();
}

void Level::changeMap(string filename)
{
	level.mChangeMap = true;
	level.mChangeMapTo = filename;
}

void Level::update()
{
	

	if (level.mRestartingLevel && level.mDelayTimer.getElapsedTime().asSeconds() > level.mDelay)
	{
		fadeToBlackChangeLevel(level.mChangeMapTo);
	}

	if (level.mChangeMap)
	{
		level.startLevel(level.mChangeMapTo);
		level.mChangeMap = false;
	}
	
}

void Level::restartLevel(float delay)
{
	if (!level.mRestartingLevel)
	{
		level.mRestartingLevel = true;
		level.mDelayTimer.restart();
		level.mDelay = delay;
	}
}

void Level::startLevel(string levelName)
{
	showLoadingScreen();

	getLevel().mCurrentMap = levelName;

	// Reset restarting level
	level.mRestartingLevel = false;
	level.mChangeMapTo = levelName;

	Log::write("Starts loading level \"" + levelName + "\"");

	// Clear all entities
	EntityList::getEntityList().emptyList();

	// Creates a box2d world
	Box2dWorld::newWorld(b2Vec2(0, -10.f));

	// Creates a music manager
	MusicManager::newManager();
	
	// Loads the level
	string mapStr = "Maps/";
	mapStr.append(levelName);
	level.loadMap(mapStr);

	// Plays all the loaded music
	MusicManager::playAll();

	// Runs start() on all entities
	EntityList::getEntityList().startList();

	// Fade from black
	float fadeDelay = 5.f;
	EntityList::getEntityList().addEntity(new FadeToBlack(fadeDelay, false), Layer::Foreground);
	

	Log::write("Finished loading level \"" + levelName + "\"");
}

void Level::loadMap(string filename)
{
	// Loads all the map data in to the Map class object
	Map map;
	if (map.loadMap(filename) == false) {
		cout << endl << "There is no map with the name \"" << filename << "\"" << endl;
		return;
	}

	cout << "Loads level \"" << filename << "\"" << endl;

	// Gets the entitylist
	EntityList& eList = EntityList::getEntityList();

	// Get the map width and height
	float mapWidth = float(map.getTileWidth());
	float mapHeight = float(map.getTileHeight());
	mMapSize = sf::Vector2f(mapWidth, mapHeight);

	// Level boundry
	sf::FloatRect levelBoundry(0, 0, mapWidth, mapHeight);

	// Sets level boundry for the camera
	Camera::currentCamera().setBounds(levelBoundry);
		
	
	// Adds a collider around the entire level
	eList.addEntity(new LevelBoundryCollider(levelBoundry), Layer::Front, false);


	// Goes through images used in the map
	cout << "\n[Image set]\n";
	for (MapTileset tileset : map.getTilesets())
	{
		cout << "First gid=" << tileset.getFirstgid() << " name=" << tileset.getName() 
			<< "\twidth=" << tileset.getTilewidth() << "\theight=" << tileset.getTileheight() << endl;
		cout << "src=" << tileset.getImageSource() << endl;
		cout << endl;
	}
	cout << endl;



	// Goes through all object in the map file						
	for (MapObjectGroup group : map.getObjectGroups())
	{		
		cout << "--[" << group.getName() << "]--" << endl;
		for (MapObject obj : group.getObjects())
		{
			// Gets the data for the object
			float width = float(obj.getWidth());
			float height = float(obj.getHeight());
			sf::Vector2f position(float(obj.getX()), float(obj.getY()));
			MapTileset tileset = map.getTileset(obj.getGid());
			float imageWidth  = float(tileset.getTilewidth());
			float imageHeight = float(tileset.getTileheight());
			sf::Vector2f positionSprite = sf::Vector2f(position.x+imageWidth/2, position.y-imageHeight/2);
			Layer layer = getLayerFromString(group.getName());
			string mapDir = "Maps/";
			mapDir.append(tileset.getImageSource());
			string imageSrc = tileset.getImageSource();
			string entityType = obj.getType();
			string id = "";
			if (obj.isProperty("id"))
				id = obj.getProperty("id").getValueString();
			sf::FloatRect rect(position.x, position.y, width, height);


			// Write to console window info about objects loaded from map file
			cout << "[" << obj.getType() << "](" << position.x << ", " <<  position.y << ")\t" 
					<< "\"" << obj.getName() << "\"" << "\t"
					<< "gid=" << obj.getGid() << " "
					<< endl;

			// --------- Spawn the right entity based on type and/or layer ---------- //
			//
			//	EntitySprite
			//
			if (entityType == "EntitySprite")
			{
				eList.addEntity(new EntitySprite(imageSrc, positionSprite), layer, false);
			}

			//
			//	ParallaxSprite
			//
			if (entityType == "ParallaxSprite")
			{
				eList.addEntity(new ParallaxSprite(imageSrc, positionSprite), layer, false);
			}

			//
			//	Jar
			//
			else if (entityType == "Jar")
			{
				position = sf::Vector2f(position.x+imageWidth/2, position.y-imageHeight/2);
				Entity *jar = nullptr;

				if (obj.isProperty("static"))
					jar = new Jar(imageSrc, position, 1.f, false);
				else if (obj.isProperty("density"))
					jar = new Jar(imageSrc, position, obj.getProperty("density").getValueFloat());
				else if (obj.isProperty("Plastic"))
					jar = new Jar(imageSrc, position, 999.f, true, true);
				else
					jar = new Jar(imageSrc, position);
				jar->setProperties(obj.getProperties());
				eList.addEntity(jar , layer, false);

				for (auto prop : obj.getProperties())
					cout << "Name=" + prop.getName() << ", Value=" << prop.getValueString() << endl;

			}

			//
			//	ZidSpawn
			//
			else if (entityType == "ZidSpawn") 
			{
				Entity* zod = new Zid(position);
				zod->setProperties(obj.getProperties());
				eList.addEntity(zod, Layer::NPC, false);
			}

			//
			//	Moth
			//
			else if (entityType == "Moth") 
			{
				Entity* mal = new Mal(position);
				mal->setProperties(obj.getProperties());
				eList.addEntity(mal, Layer::NPC, false);
			}

			//
			// Wasp
			//
			else if (entityType == "Wasp")
			{
				Entity* wasp = new Wasp(position);
				wasp->setProperties(obj.getProperties());
				eList.addEntity(wasp, Layer::NPC, false);
			}

			//
			// Firefly NPC
			//
			else if (entityType == "FireflyNPC")
			{
				Entity* dora = new FireflyNPC(position);
				eList.addEntity(dora, Layer::NPC, false);
			}

			//
			// SuCam
			//
			else if (entityType == "SuCam")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				SuCam* zone = new SuCam(rect);
				zone->setProperties(obj.getProperties());				
				zone->setID(id);
				eList.addEntity(zone, Layer::Back, false);
			}

			//
			//	Collision
			//
			else if (layer == Layer::Collision || entityType == "StaticCollision" || entityType == "StaticCollisionLoop")
			{
				bool loop = entityType == "StaticCollisionLoop" ? true : false;
				vector<sf::Vector2f> sfPoints;

				
				//cout << "Polylines= ";
				for (MapPoint p : obj.getPolyline().getPoints())
				{
					//cout << p.x << "," << p.y << " ";
					sf::Vector2f sfPoint(float(p.x), float(p.y));
					sfPoint = sfPoint + position;
					sfPoints.push_back(sfPoint);
				}
				//cout << endl;
				

				if (!sfPoints.empty())
				{
					Entity* col = new StaticLineCollider(sfPoints, loop);
					if (id != "")
						col->setID(id);
					else
						col->setID("StaticCollider");
					eList.addEntity(col, Layer::Foreground, false);
				}
			}
			
			//
			//	Trigger
			//
			else if (entityType == "Trigger")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				Trigger* trigger = new Trigger(rect);
				for (auto prop : obj.getProperties())
					cout << "Name=" + prop.getName() << ", Value=" << prop.getValueString() << endl;
				trigger->setProperties(obj.getProperties());
				
				trigger->setID(id);
				eList.addEntity(trigger, Layer::Foreground, false);
			}

			//
			//	MusicManager
			//
			else if (entityType == "MusicManager")
			{
				for (auto prop : obj.getProperties()) 
				{
					string filepath = prop.getName();
					string id = prop.getValueString();
					MusicManager::addMusic(filepath, id);
				}
			}

			//
			//	Room1_Coat
			//
			else if (entityType == "Room1_Coat")
			{
				eList.addEntity(new Room1_Coat(positionSprite), layer, false);
			}

			//
			//	Point
			//
			else if (entityType == "Point")
			{
				Entity* point = new Entity();
				point->setPosition(position);
				point->setID(id);
				eList.addEntity(point, Layer::Foreground, false);
			}

			//
			//	ForceZone
			//
			else if (entityType == "ForceZone")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				ForceZone* zone = new ForceZone(rect);
				zone->setProperties(obj.getProperties());				
				zone->setID(id);
				eList.addEntity(zone, Layer::Foreground, false);
			}

			//
			//	StickyZone
			//
			else if (entityType == "StickyZone")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				StickyZone* zone = new StickyZone(rect);
				zone->setProperties(obj.getProperties());				
				zone->setID(id);
				eList.addEntity(zone, Layer::Foreground, false);
			}

			//
			//	FireflyZone
			//
			else if (entityType == "FireflyZone")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				FireflyZone* Fzone = new FireflyZone(rect);
				Fzone->setProperties(obj.getProperties());
				Fzone->setID(id);
				eList.addEntity(Fzone, Layer::Foreground, false);
			}

			//
			//	CrackZone
			//
			else if (entityType == "HivemindZone")
			{
				sf::FloatRect rect;
				rect.left = position.x;
				rect.top = position.y;
				rect.width = width;
				rect.height = height;
				Hivemind* mind = new Hivemind(rect, obj.getProperty("image").getValueString());
				mind->setProperties(obj.getProperties());				
				mind->setID(id);
				eList.addEntity(mind, Layer::Hivemind, false);
			}

			//
			//	ToggleSprite
			//
			else if (entityType == "ToggleSprite")
			{
				string toggleOnTex = obj.getProperty("ToggleOnTexture").getValueString();
				string toggleOffTex = obj.getProperty("ToggleOffTexture").getValueString();
				ToggleSprite* toggle = new ToggleSprite(toggleOnTex, toggleOffTex, false, positionSprite);
				toggle->setProperties(obj.getProperties());				
				toggle->setID(id);
				eList.addEntity(toggle, layer, false);
			}

			//
			//	Room2_Fan
			//
			else if (entityType == "Room2_Fan")
			{
				Room2_Fan* fan = new Room2_Fan(imageSrc, positionSprite);
				fan->setProperties(obj.getProperties());
				eList.addEntity(fan, layer, false);
			}

			//
			//	Room2_AC
			//
			else if (entityType == "Room2_AC")
			{
				Room2_AC* fan = new Room2_AC(imageSrc, positionSprite);
				fan->setProperties(obj.getProperties());
				eList.addEntity(fan, layer, false);
			}

			//
			// SpoderMan
			//
			else if(entityType == "Spider")
			{
				float x = float ( obj.getProperty("x").getValueInt() );
				float y = float ( obj.getProperty("y").getValueInt() );
				Spider * spider = new Spider(sf::Vector2f(x, y), position, sf::Vector2f(width, height));
				spider->setProperties(obj.getProperties());
				eList.addEntity(spider, Layer::Front, false);
				
			}


			//Room 2 PC
			else if(entityType == "PC_Position")
			{
				PC *pc = new PC(position);
				pc->setProperties(obj.getProperties());
				eList.addEntity(pc, Layer::Back, false);
			}

			else if(entityType == "SugarBowlTop")
			{
				SugarBowlTop *sbt = new SugarBowlTop(imageSrc, position, true);
				//sbt->setProperties(obj.getProperties());
				eList.addEntity(sbt, layer, false);
			}


			//
			// AntPath
			//
			else if (entityType == "AntPath")
			{
				vector<sf::Vector2f> sfPoints;
				for (MapPoint p : obj.getPolyline().getPoints())
				{
					//cout << p.x << "," << p.y << " ";
					sf::Vector2f sfPoint(float(p.x), float(p.y));
					sfPoint = sfPoint + position;
					sfPoints.push_back(sfPoint);
				}
				AntPath* antPath = new AntPath(sfPoints);
				eList.addEntity(antPath, Layer::NPC, false);
			}

			// spiderPath
			else if(entityType =="SpiderPath")
			{
				vector<sf::Vector2f> sfPoints;
				for (MapPoint p : obj.getPolyline().getPoints())
				{
					sf::Vector2f sfPoint(float(p.x), float(p.y));
					sfPoint = sfPoint + position;
					sfPoints.push_back(sfPoint);
				}
				//add to elist
				SpiderPath* spiderpath = new SpiderPath(sfPoints);
				eList.addEntity(spiderpath, Layer::NPC, false);

			}

			else if(entityType == "Spark_Position")
			{
				Sparks* sparks = new Sparks(position);
				sparks->setProperties(obj.getProperties());
				eList.addEntity(sparks, Layer::Front, false);
			}

			//BalkPort
			else if(entityType == "BalkPort")
			{
				BalkPort* balkport = new BalkPort(imageSrc, position, imageWidth, imageHeight);
				balkport->setProperties(obj.getProperties());
				eList.addEntity(balkport, Layer::Front, false);
			}


			//
			// Boiler
			//
			else if (entityType == "Boiler")
			{
				eList.addEntity(new Boiler(imageSrc, positionSprite), layer, false);
			}

			// 
			// Termometer
			//
			else if (entityType == "Termometer")
			{
				eList.addEntity(new Termometer(position), layer, false);
			}

			// 
			// Telefonsvarare
			//
			else if (entityType == "Telefonsvarare")
			{
				eList.addEntity(new Telefonsvarare(position), layer, false);
			}

			// 
			// Dust
			//
			else if (entityType == "Dust")
			{
				eList.addEntity(new Dust(rect), Layer::Front, false);
			}

			//
			// ServerRoomDoor
			//
			else if (entityType == "ServerRoomDoor")
			{
				eList.addEntity(new ServerRoomDoor(imageSrc, positionSprite), layer, false);
			}

			//
			// ServerRoomButton
			//
			else if (entityType == "ServerRoomButton")
			{
				eList.addEntity(new ServerRoomButton(position), layer, false);
			}

			//water for schakt 2
			else if(entityType == "Water")
			{
				WaterDrop* waterdrop = new WaterDrop(position, position.y + height, obj.getProperty("spawnTime").getValueFloat());
				waterdrop->setProperties(obj.getProperties());
				eList.addEntity(waterdrop, Layer::Front, false);
				//Water* water = new Water(position, width, height);
				//water->setProperties(obj.getProperties());
				//eList.addEntity(water, Layer::Front, false);

			}
			else if(entityType == "SCHAKT2_FAN")
			{
				SCHAKT2_FAN* schaktFan = new SCHAKT2_FAN(position);
				schaktFan->setProperties(obj.getProperties());
				eList.addEntity(schaktFan, layer, false);
			}


			else if(entityType == "EatingWasp")
			{
				EatingWasp* eatingwasp = new EatingWasp(obj.getProperty("image").getValueString(), position);
				eatingwasp->setProperties(obj.getProperties());
				eList.addEntity(eatingwasp, layer, false);
			}


			//
			// CameraScreen
			//
			else if (entityType == "CameraScreen")
			{
				
				string open = "Room 3/" + obj.getProperty("openTexture").getValueString();
				string active = "Room 3/" + obj.getProperty("activeTexture").getValueString();
				CameraScreen* cam = new CameraScreen(open, active, position);
				cam->setID(id);
				eList.addEntity(cam, layer, false);
				
			}

			else if(entityType == "spak")
			{
				lever* spak = new lever(position, imageSrc);
				spak->setProperties(obj.getProperties());
				eList.addEntity(spak, layer, false);
			}

			//
			// ServerRoomEyeScreen
			//
			else if (entityType == "ServerRoomEyeScreen")
			{
				eList.addEntity(new ServerRoomEyeScreen(position), layer, false);
			}

			//
			//	FadeSprite
			//
			if (entityType == "FadeSprite")
			{
				Entity* fadeSprite = new FadeSprite(imageSrc, positionSprite);
				fadeSprite->setID(id);
				eList.addEntity(fadeSprite, layer, false);
			}

			//
			// LightPoint
			//
			if (entityType == "LightPoint")
			{
				int r,g,b;
				if (obj.isProperty("r") && obj.isProperty("g") && obj.isProperty("b"))
				{
					r = obj.getProperty("r").getValueInt();
					g = obj.getProperty("g").getValueInt();
					b = obj.getProperty("b").getValueInt();
				}
				else
				{
					r = g = b = 255;
				}
				float radius = width/2;
				sf::Vector2f pos = sf::Vector2f( position.x + radius, position.y + radius);
				
				Entity* light = new Light(sf::Color(r,g,b,255), pos, radius, 360, 0, "zidLight", true, false);
				light->setID(id);
				EntityList::getEntityList().addEntity(light, Layer::Light, false);
			}

			//
			// Movie
			//
			if (entityType == "Movie")
			{
				string movieFile;
				if (obj.isProperty("movie"))
					movieFile = obj.getProperty("movie").getValueString();					
				else				
					movieFile = "";

				Entity* movie = new Movie(movieFile);

				EntityList::getEntityList().addEntity(movie, Layer::GUI, false);
			}
			


//			else if (entityType == "SecuMonitor")
//			{
//				sf::FloatRect rect;
//				rect.left = position.x;
//				rect.top = position.y;
//				rect.width = width;
//				rect.height = height;
//				SecuMonitor* secuMonitor = new SecuMonitor(rect);
//				secuMonitor->setProperties(obj.getProperties());				
//				secuMonitor->setID(id);
//				eList.addEntity(secuMonitor, Layer::Foreground, false);
//			}

			//Viewport Test

		}

		cout << endl;
	}		


}


Layer Level::getLayerFromString(string strLayer)
{
	if (strLayer == "Background")
		return Layer::Background;
	else if (strLayer == "Back")
		return Layer::Back;
	else if (strLayer == "Front")
		return Layer::Front;
	else if (strLayer == "Foreground")
		return Layer::Foreground;
	else if (strLayer == "Collision")
		return Layer::Collision;
	else if (strLayer == "Light")
		return Layer::Light;
	else if (strLayer == "Misc")
		return Layer::Misc;
	else if (strLayer == "NPC")
		return Layer::NPC;
	else if (strLayer == "Hivemind")
		return Layer::Hivemind;
	else if (strLayer == "GUI")
		return Layer::GUI;

	string ectStr = "Level::getLayerFromString - No Layer with that name: ";
	ectStr.append(strLayer);
	throw logic_error(ectStr);
	return Layer::Background;
}

void Level::init( sf::RenderWindow* window )
{
	sWindow = window;
}

void Level::showLoadingScreen()
{
	sf::Font font;
	font.loadFromFile("Resources/BEBAS___.ttf");

	sf::Text loadingText;
	loadingText.setFont(font);
	loadingText.setPosition(sWindow->getSize().x - 200.f, sWindow->getSize().y - 100.f);
	loadingText.setCharacterSize(40);
	loadingText.setString("LOADING");
	loadingText.setColor(sf::Color::White);

	sWindow->clear(sf::Color::Black);
	sWindow->setView(sWindow->getDefaultView());
	sWindow->draw(loadingText);
	sWindow->display();
}

