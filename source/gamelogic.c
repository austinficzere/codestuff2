#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"
#include <time.h>

const int NUMB_LIVES = 3;
const int START_TIME = 300;
const int NUMB_MOVES = 100;
const int MAP_ROWS = 30;
const int MAP_COLS = 30;

struct gameState initGameState();
int isGameEnd(struct gameState *gs);

void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr);


struct Tile **createTable(int rows, int cols)
{
	struct Tile **table = (struct Tile **) malloc(rows*sizeof(struct Tile *));
	for(int i = 0;i<rows;i++)
	{
		table[i] = (struct Tile *) malloc(cols * sizeof(struct Tile));
	}
	return table;

}

int isGameEnd(struct gameState *gs)
{
	if(gs->numbLives == 0 || gs->time == 0 || gs -> movesLeft == 0 || gs -> hasWon || gs -> hasLost || gs -> quit){
		if(gs -> hasWon){
			// update score
		}
		return 1;	
	}
	return 0;
}

void updateGameState(struct gameState *gs, int button, int startTime)
{
	if (gs -> numbLives == 0)
	{
		gs -> hasLost = 1;
		return;
	}

	// deal with quit, which we check if we are in stage -1, and they press A on correct button

	int i = 3;
	while (i < 8)
	{
		if ((button & (1<<i)))
		{
			gs -> gameStage = -1;
			break;
		}

		if ((button & (1<<i)))
		{
			if (gs -> map.frogY == 0)
			{
				if (gs -> gameStage == 3)
				{
					gs -> hasWon = 1;
					gs -> score = ((gs -> time) + (gs -> movesLeft) + (gs -> numbLives)) * 2;
					break;
				}
				else
				{
					gs -> gameStage++;
					gs -> map.frogY = MAP_ROWS - 1;
					gs -> movesLeft--;
					gs -> time = time(0) - startTime;
					gs -> score++;
					break;
				}
			}
			else
			{
				if (i == 4)
				{
					gs -> map.frogY--;
					gs -> score++;
				}
				if (i == 5)
				{
					gs -> map.frogY++;
				}
				if (i == 6)
				{
					gs -> map.frogX--;
				}
				if (i == 7)
				{
					gs -> map.frogX++;
				}
				gs -> movesLeft--;
				gs -> time = time(0) - startTime;
				break;
			}
		}
	}
}

void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr){
	prev -> frogX = curr -> frogX;
	prev -> frogY = curr -> frogY;
	prev -> orientation = curr -> orientation;

	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			prev -> table[i][j] = curr -> table[i][j];
		}
	}
}

void setCurrToPrev(struct gameState *prev, struct gameState *curr){

	setCurrToPrevMap(&(prev -> map), &(curr -> map));
	prev -> score = curr -> score;
	prev -> numbLives = curr -> numbLives;
	prev -> time = curr -> time;
	prev -> movesLeft = curr -> movesLeft;
	prev -> quit = curr -> quit;
	prev -> hasWon = curr -> hasWon;
	prev -> hasLost = curr -> hasLost;
	prev -> gameStage = curr -> gameStage;

}

struct gameMap initGameMap()
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);
	map.rows = MAP_ROWS;
	map.cols = MAP_COLS;
	map.frogX = map.cols/2;
	map.frogY = map.rows-2;
	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			map.table[i][j].valuePack = 0;
		}
	}

	return map;
}

struct gameState initGameState()
{
	struct gameState gs;
	gs.map = initGameMap();
	gs.score = 0;
	gs.numbLives = NUMB_LIVES;
	gs.time = START_TIME;
	gs.movesLeft = NUMB_MOVES;
	gs.quit = 0;
	gs.hasWon = 0;
	gs.hasLost = 0;
	gs.gameStage = 0;
	return gs;
}
