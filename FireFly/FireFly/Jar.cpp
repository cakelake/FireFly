#include "Jar.h"

Jar::Jar(TexturesID textureID, sf::Vector2f position)
: mRigidbody()
, mSprite(Loading::getLoading().getTexture(textureID))
{
	// S�tter origin f�r spriten till mitten
	sf::FloatRect bounds = mSprite.getLocalBounds();
	mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);


	// Skapar rectanglar f�r kollision f�r Burken
	float glassWidthAdjust = 50.f;
	float glassHeightAdjust = 50.f;

	bounds.left += glassWidthAdjust;
	bounds.top += glassHeightAdjust;
	bounds.width = bounds.width - 2*glassWidthAdjust;
	bounds.height = bounds.height - 2*glassHeightAdjust;
	
	float width = 20.f;
	std::vector<sf::FloatRect> rects;
	//Left
	rects.push_back(sf::FloatRect(-bounds.width/2, -bounds.height/2, width, bounds.height));
	//Top
	rects.push_back(sf::FloatRect(-bounds.width/2, -bounds.height/2, bounds.width, width));
	//Bottom
	rects.push_back(sf::FloatRect(-bounds.width/2, bounds.height/2-width, bounds.width, width));
	//Right
	rects.push_back(sf::FloatRect(bounds.width/2-width, -bounds.height/2, width, bounds.height));

	float density = 3.f;
	mRigidbody.AddDynRectBody(rects, position, density);
}


void Jar::updateEntity(sf::Time dt) 
{
	// Get the position and rotation from the rigidbody
	mRigidbody.update();				
	setPosition(mRigidbody.getPosition());
	setRotation(mRigidbody.getRotation());
}
		
void Jar::drawEntity(sf::RenderTarget& target, sf::RenderStates states) const
{	
	states.transform *= getTransform();
	target.draw(mSprite, states);

	// Rigidbody debug draw
	if (Globals::DEBUG_MODE)
		mRigidbody.drawDebug(target, states);
}