#include "Level.h"
#include "Loading.h"
#include "Box2dWorld.h"
#include "Camera.h"

#include "Zid.h"
#include "EntitySprite.h"
#include "StaticCollider.h"
#include "LevelBoundryCollider.h"


Level Level::level;

Level::Level()
{
}


Level::~Level()
{
}

Level &Level::getLevel()
{
	return level;
}

void Level::startLevel0()
{
	// Creates a box2d world
	Box2dWorld::newWorld(b2Vec2(0, -10.f));

	// Load the resources for this level
	Loading::getLoading().loadLevel0();

	// Level boundry
	sf::FloatRect levelBoundry(-2500, -1080, 5000, 2160);

	// Sets level boundry for the camera
	Camera::currentCamera().setBounds(levelBoundry);
	//Camera::currentCamera().setBounds(sf::FloatRect(0, 0, 5000, 2160));

	// Create the entities for this level 
	// Should come from a level file later
	/*
	Player *player = new Player(100, 100);
	EntityList::getEntityList().addEntity(player);
	Mal *mal = new Mal();
	EntityList::getEntityList().addEntity(mal);
	*/

	EntityList::getEntityList().addEntity(new EntitySprite(Textures::Background_Room1));

	EntityList::getEntityList().addEntity(new Zid());

	EntityList::getEntityList().addEntity(new StaticCollider(sf::Vector2f(0, 200.f)));
	EntityList::getEntityList().addEntity(new LevelBoundryCollider(levelBoundry));

	sf::FloatRect levelBoundry2(-1500, -580, 2000, 500);
	EntityList::getEntityList().addEntity(new LevelBoundryCollider(levelBoundry2));
}

