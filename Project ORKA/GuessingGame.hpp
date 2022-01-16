#pragma once

//author is mb

#include "Basics.hpp"
#include "Random.hpp"

void guessingGame() {
	randomizeSeed();

	Int Randomvariable = randomInt(1000);
	Int Somevariable;
	String Leaderboardname;
	Int GuessNumber = 0;

	std::cout << "Welcome to the number guessing game, please type your name:" << std::endl;
	std::cin >> Leaderboardname;

	std::cout << "Try and guess the number" << std::endl;
	std::cin >> Somevariable;

	while (Randomvariable != Somevariable) {

		if (Randomvariable < Somevariable) std::cout << "Smaller" << std::endl;
		if (Randomvariable > Somevariable) std::cout << "Larger" << std::endl;
		GuessNumber++;

		std::cin >> Somevariable;

	}

	if (Randomvariable == Somevariable) std::cout << "You guessed correctly, " << Leaderboardname << "!" << std::endl;
	std::cout << Leaderboardname << "'s Score:" << "-" << GuessNumber << std::endl;

}