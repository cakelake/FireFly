#include "EntitySprite.h"

EntitySprite::EntitySprite(string textureID) 
: mSprite(Loading::getTexture(textureID))
, mActivate(true)
{
	// S�tter origin f�r spriten till mitten
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
	mActivate = true;
}

EntitySprite::EntitySprite(string textureID ,sf::Vector2f position) 
: mSprite(Loading::getTexture(textureID))
, mActivate(true)
{
	// S�tter origin f�r spriten till mitten
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

	setPosition(position);
	mActivate = true;
}

void EntitySprite::updateEntity(sf::Time dt) 
{
	
}
		
void EntitySprite::drawEntity(sf::RenderTarget& target, sf::RenderStates states) const
{	
	if(mActivate == true)
	{
		states.transform *= getTransform();
		target.draw(mSprite, states);
	}


}

void EntitySprite::sendMessage(Entity* entity, std::string message)
{
	if(message == "Activate")
	{
		mActivate = true;
	}
	if(message == "Deactivate")
	{
		mActivate = false;
	}


}
