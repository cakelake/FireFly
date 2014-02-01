#ifndef ZID_H
#define ZID_H

#include "Entity.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>



class Zid : public Entity
{
public:
	Zid();

private:
	virtual void	updateEntity(sf::Time dt);	
	virtual void	drawEntity(sf::RenderTarget& target, sf::RenderStates states) const;

	void movement();

private:
	sf::Sprite		mSprite;
	Rigidbody		mRigidbody;
	
};

#endif