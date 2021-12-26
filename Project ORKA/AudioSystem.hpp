
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

struct Sound {

};

struct AudioSystem {

	Vector<Sound> sound;

	void playSound(Index soundID) {};
	void importMP3(Path path) {};
	void importWav(Path path) {};
};