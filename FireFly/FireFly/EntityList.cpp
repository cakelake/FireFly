#include "EntityList.h"

EntityList EntityList::eL;

EntityList::EntityList()
{
	rLightMap.create(6000, 6000); //M�ste fixas s�Eden anpassas efter levelens storlek
	sLightMap.setPosition(0,6000);
	sLightMap.scale(1, -1);
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
	for (Entity* e : BackLayerList)
		window.draw(*e);
	for (Entity* e : NPCLayerList)
		window.draw(*e);
	for (Entity* e : FrontLayerList)
		window.draw(*e);
	for (Entity* e : ForegroundLayerList)
		window.draw(*e);

}

void EntityList::drawBackground(sf::RenderWindow& window)
{
	for (Entity* e : BackgroundLayerList)
		window.draw(*e);
}

void EntityList::drawBack(sf::RenderWindow& window)
{
	for (Entity* e : BackLayerList)
		window.draw(*e);
}

void EntityList::drawNPC(sf::RenderWindow& window)
{
	for (Entity* e : NPCLayerList)
		window.draw(*e);
}

void EntityList::drawFront(sf::RenderWindow& window)
{
	for (Entity* e : FrontLayerList)
		window.draw(*e);
}

void EntityList::drawLight(sf::RenderWindow& window)
{
	for (Entity* e : LightLayerList)
	{
		if((*e).getID() == "zidLight")
		{
			Entity* zid = EntityList::getEntityList().getEntity("Zid");
			e->setPosition(sf::Vector2f((*zid).getPosition().x, (*zid).getPosition().y));
		}
		//rLightMap.clear(sf::Color(15,15,20,240));
		rLightMap.clear(sf::Color(20,20,40,240));
		rLightMap.draw(*e);
		sLightMap.setTexture(rLightMap.getTexture());
		window.draw(sLightMap, sf::BlendMultiply);
	}
}

void EntityList::drawForeground(sf::RenderWindow& window)
{
	for (Entity* e : ForegroundLayerList)
		window.draw(*e);
}


void EntityList::addEntity(Entity *entity, Layer layer, bool runStart)
{
	listedEntities.push_back(entity);
	
	// Add the entity in a layer list to be used in the draw function
	if (layer == Layer::Background)
		BackgroundLayerList.push_back(entity);
	if (layer == Layer::Back)
		BackLayerList.push_back(entity);
	else if (layer == Layer::NPC)
		NPCLayerList.push_back(entity);
	else if (layer == Layer::Front)
		FrontLayerList.push_back(entity);
	else if (layer == Layer::Light)
		LightLayerList.push_back(entity);
	else if (layer == Layer::Foreground)
		ForegroundLayerList.push_back(entity);

	if (runStart)
		entity->start();
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
	// BackList
	i = BackLayerList.begin();
	while (i != BackLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			BackLayerList.erase(i++); 
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
	// LightLayerList
	i = LightLayerList.begin();
	while (i != LightLayerList.end())
	{
		bool isAlive = (*i)->getAliveStatus();
		if (!isAlive)
		{
			LightLayerList.erase(i++);
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
	listedEntities = entityList();

	BackgroundLayerList = entityList();
	BackLayerList = entityList();
	NPCLayerList = entityList();
	FrontLayerList = entityList();
	ForegroundLayerList = entityList();
	LightLayerList = entityList();
}

void EntityList::startList()
{
	for (Entity* e : listedEntities) 
	{
		e->start();
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

	return nullptr;
}

std::vector<Entity*> EntityList::getEntities(std::string id)
{
	std::vector<Entity*> list;
	for (Entity* e : listedEntities) 
	{
		if(e->getID() == id)
		{
			list.push_back(e);
		}
	}

	return list;
}
