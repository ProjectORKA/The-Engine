#pragma once

// author: q80823
// 16.1.22

// author: blendurian
// 16.1.22

#include "Basics.hpp"
#include "Random.hpp"

struct GuessingGame
{
	void run()
	{
		randomizeSeed();
		Int           guess      = 0;
		Int           guessCount = 0;
		constexpr Int maxNumber  = 1000;
		String        name       = "Player";
		const Int     target     = randomIntFast(maxNumber);

		std::cout << "Welcome to the number guessing game, whats your name?\n";
		std::cin >> name;

		std::cout << "Hey " << name << ".\nI have generated a random number between 0 and " << maxNumber << ".\nCan you try and guess the number?\n";
		std::cin >> guess;

		while(target != guess)
		{
			if(target < guess) std::cout << "Its smaller than that.\n";
			if(target > guess) std::cout << "Its larger than that.\n";
			guessCount++;
			std::cin >> guess;
		}

		if(target == guess) std::cout << "Congratulations " << name << "!\nYou guessed correctly!\nYour score: " << guessCount << "\n";
		pause();
	}
};
