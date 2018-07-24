#pragma once
#include <SFML/Audio.hpp>

// Classe per gestione dell'audio basato su SFML
class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	int PlayMusic();
	int StopMusic();
	int PlaySFX();

private:
	sf::SoundBuffer musicBuffer;
	sf::Sound musicSound;
	sf::SoundBuffer sfxBuffer;
	sf::Sound sfxSound;
	bool loadMusicSuccess;
    bool loadSFXSuccess;
};

