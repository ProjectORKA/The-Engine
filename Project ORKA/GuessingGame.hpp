#pragma once

//author: q80823
//16.1.22

//author: blendurian
//16.1.22

#include "Basics.hpp"
#include "Random.hpp"

void guessingGame() {
	randomizeSeed();
	const Int maxNumber = 1000;
	const Int target = randomInt(maxNumber);
	Int guess = 0;
	String name;
	Int guessCount = 0;

	std::cout << "Welcome to the number guessing game, whats your name?\n";
	std::cin >> name;

	std::cout << "We have generated a random number between 0 and " << maxNumber << ". Try and guess the number." <<
		std::endl;
	std::cin >> guess;

	while (target != guess) {
		if (target < guess) std::cout << "Its smaller than that.\n";
		if (target > guess) std::cout << "Its larger than that.\n";
		guessCount++;
		std::cin >> guess;
	}

	if (target == guess) std::cout << "Congratulations " << name << ", you guessed correctly!\n";
	std::cout << "Your score:" << guessCount << std::endl;
}
