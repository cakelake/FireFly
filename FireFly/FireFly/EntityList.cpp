#include "EntityList.h"

EntityList EntityList::eL;

EntityList::EntityList()
{
}

EntityList::~EntityList()
{
}

EntityList &EntityList::getEntityList()
{
	return eL;
}

void EntityList::update(sf::Time dt)
{
	for (Entity* e : listedEntities) 
	{
		e->update(dt);
	}
}

void EntityList::draw(sf::RenderWindow& window)
{
	// Draw in correct order with the layer lists
	for (Entity* e : BackgroundLayerList)
		window.draw(*e);
	for (Entity* e : NPCLayerList)
		window.draw(*e);
	for (Entity* e : FrontLayerList)
		window.draw(*e);
	for (Entity* e : ForegroundLayerList)
		window.draw(*e);

}

void EntityList::addEntity(Entity *entity, Layer layer)
{
	listedEntities.push_back(entity);

	// Add the entity in a layer list to be used in the draw function
	if (layer == Layer::Background)
		BackgroundLayerList.push_back(entity);
	else if (layer == Layer::NPC)
		NPCLayerList.push_back(entity);
	else if (layer == Layer::Front)
		FrontLayerList.push_back(entity);
	else if (layer == Layer::Foreground)
		ForegroundLayerList.push_back(entity);
}

void EntityList::updateList()
{
	// Removes entities from the layer lists if dead
	// BackgroundList
	entityList::iterator i = BackgroundLayerList.begin();
	while (i != BackgroundLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			BackgroundLayerList.erase(i++); 
		}
		else
		{
			++i;
		}
	}
	// NPCList
	i = NPCLayerList.begin();
	while (i != NPCLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			NPCLayerList.erase(i++);  
		}
		else
		{
			++i;
		}
	}
	// FrontLayerList
	i = FrontLayerList.begin();
	while (i != FrontLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			FrontLayerList.erase(i++); 
		}
		else
		{
			++i;
		}
	}
	// ForegroundLayerList
	i = ForegroundLayerList.begin();
	while (i != ForegroundLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			ForegroundLayerList.erase(i++);
		}
		else
		{
			++i;
		}
	}


	// Removes and deletes entities from the main list listedEntities if dead
	for(entityList::iterator i = listedEntities.begin(); i != listedEntities.end(); i++)
	{
		if((*i)->getAliveStatus() == true)  // if entity is alive it will be added to tempEntities
		{
			tempEntities.push_back((*i));
		}
		else
		{
			delete (*i);  //decreases chance of memory leaks.
		}
	}
	listedEntities.clear();		
	listedEntities = tempEntities;
	tempEntities.clear();
}

void EntityList::emptyList()
{
	for(entityList::iterator i = listedEntities.begin(); i != listedEntities.end(); i++)
	{
		delete (*i);
	}
}

Entity* EntityList::getEntity(std::string id)
{
	for (Entity* e : listedEntities) 
	{
		if(e->getID() == id)
		{
			return e;
		}

	}
}
