#include "Sparks.h"


Sparks::Sparks(sf::Vector2f position)
	: spark(Loading::getTexture("Schakt/elgnista_spritesheet.png"), 84, 84, 1, 8, 40) 
	,mNoise(Loading::getSound("Schakt/ElectricCracklingNoReverb.wav"), false)
{
	setPosition(position);
	mNoise.setPosition(position);
	mNoise.setLoop(true);
	mNoise.play();
	mNoise.getSound()->setMinDistance(175.f);
	spark.mLoopOnce = true;
}


void Sparks::updateEntity(sf::Time dt)
{
	if(spark.mLoopOnce == true && mSparkClock.getElapsedTime().asSeconds() > 1)
	{
		spark.oneLoop();
	}
	
	mSprite = spark.getCurrentSprite();
	
	if(spark.mLoopOnce == false)
	{
		mSparkClock.restart();
		spark.mLoopOnce = true;
	}
}

void Sparks::drawEntity(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(mSprite, states);
}
