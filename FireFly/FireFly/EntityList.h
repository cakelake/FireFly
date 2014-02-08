#pragma once
#include <list>
#include "Entity.h"

enum class Layer 
{
	Background,
	NPC,
	Front,
	Foreground,
	Collision,
	Misc,
};

class EntityList
{
public:
	static EntityList &getEntityList(); //singleton
	typedef std::list <Entity*> entityList;

	void update(sf::Time dt);	// Updates all entities
	void draw(sf::RenderWindow& window);	// Draws all entities
	
	void addEntity(Entity *entity, Layer layer, bool runStart = true);	//adds new entities
	void updateList();	//deletes "dead" entities
	void emptyList();
	void startList(); // Runs once after levelload
	Entity* getEntity(std::string id);
	std::vector<Entity*> getEntities(std::string id);

private:
	EntityList();
	~EntityList();
	void operator=(const EntityList &eL);	//singleton
	static EntityList eL;	//singleton
	typedef std::list <Entity*> tempList; //stores "alive" entities
	tempList tempEntities;	//used for deleting killed entities

	entityList listedEntities;

	entityList BackgroundLayerList;
	entityList NPCLayerList;
	entityList FrontLayerList;
	entityList ForegroundLayerList;

};

