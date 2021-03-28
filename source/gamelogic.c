#include <unistd.h>
#include <stdio.h>



struct Tile
{
	int valuePack;
	int isFrogOn;
}

struct gameMap
{
	Player frog;
	struct Tile **table;
	
}

struct gameState
{
	gameMap map;
	int score;
	int numbLives;
	int time;
	int movesLeft;
	int quit;
	int hasWon;
	int hasLost;
}

struct Tile **createTable(int rows, int cols)
{
	struct Tile **table = (struct Tile **) malloc(rows*sizeof(struct Tile *));
	for(int i = 0;i<rows;i++)
	{
		table[i] = (struct Tile *) malloc(cols * sizeof(struct Tile));
	}
	return table;

}



int isGameEnd(gameState* )
{
	if(gameState->numbLives == 0 || gameState->time == 0 || gameState -> movesLeft == 0 || hasWon || hasLost || quit){
		if(hasWon){
			// update score
		}
		return 1;	
	}

}
