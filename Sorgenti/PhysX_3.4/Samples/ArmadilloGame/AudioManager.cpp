#include "AudioManager.h"

AudioManager::AudioManager()
{
    loadMusicSuccess = musicBuffer.loadFromFile("../../Media/PhysX/3.4/Samples/MrGear.wav");
	musicSound.setBuffer(musicBuffer);

    loadSFXSuccess = sfxBuffer.loadFromFile("../../Media/PhysX/3.4/Samples/VictorySFX.wav");
	sfxSound.setBuffer(sfxBuffer);
}

AudioManager::~AudioManager()
{
}

// Gestione riproduzione background music
int AudioManager::PlayMusic()
{
	if (!loadMusicSuccess) return -1;
	if (musicSound.getStatus() != sf::SoundSource::Status::Playing) {
		musicSound.setLoop(true);
		musicSound.play();
	}
	return 0;
}

int AudioManager::StopMusic() {
	if (!loadMusicSuccess) return -1;
	if (musicSound.getStatus() == sf::SoundSource::Status::Playing) {
		musicSound.stop();
	}
	return 0;
}

// Gestione riproduzione SFX fine gioco
int AudioManager::PlaySFX() {
    if (!loadSFXSuccess) return -1;
	if (sfxSound.getStatus() != sf::SoundSource::Status::Playing) {
		sfxSound.play();
	}
	return 0;
}