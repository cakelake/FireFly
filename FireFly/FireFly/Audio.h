#pragma once

#include "Loading.h"
#include <string>
#include <SFML/Audio.hpp>

class Audio
{
public:
	Audio(sf::SoundBuffer& buffer, bool global = false);
	~Audio();
	void play();
	void pause();
	void stop();
	void setPosition(sf::Vector2f position);
	void setLoop(bool statement);
	sf::Sound::Status getStatus();
	sf::Sound* getSound();


private:
	sf::Sound mSound;
};