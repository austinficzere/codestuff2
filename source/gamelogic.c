#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"

const int NUMB_LIVES = 3;
const int START_TIME = 300;
const int NUMB_MOVES = 100;
const int MAP_ROWS = 30;
const int MAP_COLS = 30;

struct gameState initGameState();
int isGameEnd(struct gameState *gs);


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

struct gameState updateGameState(struct gameState *gs, int button)
{

}


struct gameMap initGameMap()
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);
	map.rows = MAP_ROWS;
	map.cols = MAP_COLS;
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
