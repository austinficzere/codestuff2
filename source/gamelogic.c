#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"
#include <time.h>
#include "controller.h"

const int NUMB_LIVES = 3;
const int START_TIME = 300;
const int NUMB_MOVES = 100;
const int MAP_ROWS = 30;
const int MAP_COLS = 30;
const int H_OBJ = 15;

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
	
	if (isButtonPressed(button,START_BUTTON))
	{
		gs -> gameStage = -1;
		return;
	}

	if(button!=NONE_PRESSED){
		if (isButtonPressed(button,UP_BUTTON) && gs -> map.frogY>0)
		{
			gs -> map.orientation = 0;
			gs -> map.frogY--;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,DOWN_BUTTON) && gs -> map.frogY<MAP_ROWS-1)
		{
			gs -> map.orientation = 2;
			gs -> map.frogY++;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,LEFT_BUTTON) && gs -> map.frogX>0)
		{
			gs -> map.orientation = 3;
			gs -> map.frogX--;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,RIGHT_BUTTON) && gs -> map.frogX<MAP_COLS)
		{
			gs -> map.orientation = 1;
			gs -> map.frogX++;
			gs -> movesLeft--;
		}
	}

	if (gs -> map.frogY == 0)
	{
		if (gs -> gameStage == 3)
			{
				gs -> hasWon = 1;
				gs -> score = ((gs -> time) + (gs -> movesLeft) + (gs -> numbLives)) * 2;
			}
			else
			{
				gs -> gameStage++;
				gs -> map.frogY = MAP_ROWS - 1;
				gs -> movesLeft--;
				gs -> score++;
			}
	}

	gs -> time = time(0) - startTime;

}

void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr){
	prev -> frogX = curr -> frogX;
	prev -> frogY = curr -> frogY;
	prev -> orientation = curr -> orientation;

	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			//STRUCT 
			//FIX
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

void initHarmObjects(struct harmObject *hObjs){
	
}

struct gameMap initGameMap()
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);

	map.hObjs = malloc(H_OBJ * sizeof(struct harmObject));
	initHarmObjects(map.hObjs);

	map.rows = MAP_ROWS;
	map.cols = MAP_COLS;
	map.frogX = map.cols/2;
	map.frogY = map.rows-2;
	map.orientation = 0;
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
