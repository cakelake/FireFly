#pragma once

#include "Entity.h"
#include "Animation.h"
#include "Audio.h"

class Myra : public Entity
{
public:
	Myra(vector<sf::Vector2f> path, vector<float> lengths, vector<sf::Vector2f> directions);

private:
	virtual void updateEntity(sf::Time dt);	
	virtual void drawEntity(sf::RenderTarget& target, sf::RenderStates states) const;

	void setProcPosition(float p);

private:
	sf::Sprite mSprite;
	vector<sf::Vector2f> mPath;
	vector<float> mLengths;
	vector<sf::Vector2f> mDirections;
	float mPos;
};