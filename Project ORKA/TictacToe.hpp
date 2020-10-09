
#include "Basics.hpp"

struct Board {
	Short tiles[3][3];
	Bool gameOver = false;
	Short winner = 0;
	Short team = 1;

	void clear(){
		tiles[0][0] = 0;
		tiles[0][1] = 0;
		tiles[0][2] = 0;
		tiles[1][0] = 0;
		tiles[1][1] = 0;
		tiles[1][2] = 0;
		tiles[2][0] = 0;
		tiles[2][1] = 0;
		tiles[2][2] = 0;
	}
	void set(Short x, Short y, Short team) {
		if (!gameOver) {
			tiles[x][y] = team;
			if (team == 1) team = 2;
			else team = 1;
		}

	};
	void checkState() {
		//vertical
		if (tiles[0][0] == tiles[0][1] == tiles[0][2] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
		if (tiles[1][0] == tiles[1][1] == tiles[1][2] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
		if (tiles[2][0] == tiles[0][1] == tiles[2][2] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}

		//horizontal
		if (tiles[0][0] == tiles[1][0] == tiles[2][0] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
		if (tiles[0][1] == tiles[1][1] == tiles[2][1] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
		if (tiles[0][2] == tiles[1][2] == tiles[2][2] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}

		//diagonal
		if (tiles[0][0] == tiles[1][1] == tiles[2][2] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
		if (tiles[0][2] == tiles[1][1] == tiles[2][0] != 0) {
			gameOver = true;
			winner = tiles[0][0];
		}
	};
};