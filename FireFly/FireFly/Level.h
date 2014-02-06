#pragma once
#include "EntityList.h"
#include "Player.h"
#include "Mal.h"
#include "Map.h"

class Level
{
public:
	static Level &getLevel();

	// Initiates level 0
	void startLevel0();
	// Initiates level 1
	void startLevel1();
private:
	Level();
	~Level();
	void operator=(const Level &level);
	static Level level;

private:
	// Spawn entities from a map file
	void loadMap(string filename);
	TexturesID	getTexIdFromString(string strTexId);
	Layer		getLayerFromString(string strLayer);
};

