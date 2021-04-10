#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"
#include <time.h>
#include "controller.h"
#include "global.h"
#include "gfx.h"

const int NUMB_LIVES = 3;
const int START_TIME = 300;
const int NUMB_MOVES = 300;
const int MAP_ROWS = 20;
const int MAP_COLS = 30;
const int H_OBJ = 15;

struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void spawnValuePacks(int sTime, struct gameState *gs);
void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr);
void updateHarmObjects(struct gameMap gm);
int collides(int left1, int right1, const struct imageStruct *img1, int left2, int right2, const struct imageStruct *img2);

int frogCollideHarm(struct gameMap gm);


struct Tile **createTable(int rows, int cols)
{
	struct Tile **table = (struct Tile **) malloc(rows*sizeof(struct Tile *));
	for(int i = 0;i<rows;i++)
	{
		table[i] = (struct Tile *) malloc(cols * sizeof(struct Tile));
	}
	return table;
}


struct Tile copyTile(struct Tile tc){
	struct Tile ret;
	ret.valuePack = tc.valuePack;
	ret.canStand = tc.canStand;

	return ret;
}


int isGameEnd(struct gameState *gs)
{
	if(gs->numbLives == 0 || gs->time == 0 || gs -> movesLeft == 0 || gs -> hasWon || gs -> hasLost || gs -> quit){
		return 1;	
	}
	return 0;
}

int updateMenuScreen(struct gameState *gs, int button, int menuState)
{
	if (menuState == 0 && isButtonPressed(button,A_BUTTON))
	{
		gs -> state = 2;
	}

	else if (menuState == 1 && isButtonPressed(button,A_BUTTON))
	{
		gs -> quit = 1;
	}

	else if (isButtonPressed(button,UP_BUTTON))
	{
		menuState = 0;
	}
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		menuState = 1;
	}
	return menuState;
}

int updatePauseScreen(struct gameState *gs, int button, int pauseState)
{
	if (pauseState == 0 && isButtonPressed(button,A_BUTTON))
	{
		*gs = initGameState();
	}

	else if (pauseState == 1 && isButtonPressed(button,A_BUTTON))
	{
		gs -> state = 0;
	}

	else if (isButtonPressed(button,UP_BUTTON))
	{
		pauseState = 0;
	}
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		pauseState = 1;
	}
	else if (isButtonPressed(button,START_BUTTON))
	{
		gs -> state = 2;
	}
	return pauseState;
}

void updateGameState(struct gameState *gs, int button, int startTime)
{
	if (isGameEnd(gs)){
		gs ->  quit = 1;
		printf("hello");
		return;
	}

	if(frogCollideHarm(gs -> map)){
		gs -> numbLives--;
		gs -> map.frogX = gs -> map.cols/2;
		gs -> map.frogY = gs -> map.rows-2;
	}

	// deal with quit, which we check if we are in stage -1, and they press A on correct button
	updateHarmObjects(gs -> map);
	if (isButtonPressed(button,START_BUTTON)){
		gs -> state = 1;
	}

	spawnValuePacks(startTime, gs);

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

	// Check collision with harm object
	if(gs -> map.table[gs -> map.frogY][gs -> map.frogX].valuePack != 0){
		int valueStand  = gs -> map.table[gs -> map.frogY][gs -> map.frogX].valuePack;
		// do stuff
		gs -> map.table[gs -> map.frogY][gs -> map.frogX].valuePack = 0;
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
				gs -> map.frogY = MAP_ROWS - 2;
				gs -> score++;
			}
	}else if(gs -> gameStage != 0 && gs -> map.frogY == MAP_ROWS - 1){
		gs -> gameStage--;
		gs -> map.frogY = 1;
	}

	gs -> time = time(0) - startTime;

}

void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr){
	prev -> frogX = curr -> frogX;
	prev -> frogY = curr -> frogY;
	prev -> orientation = curr -> orientation;

	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			prev -> table[i][j] = copyTile(curr -> table[i][j]);
		}
	}

	for(int i = 0;i<curr -> numbOfHarm;i++){
		prev -> hObjs[i].drawX = curr -> hObjs[i].drawX;
		prev -> hObjs[i].drawY = curr -> hObjs[i].drawY;
		prev -> hObjs[i].speed= curr -> hObjs[i].speed;
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
	prev -> state = curr -> state;

}

int randomNumb(int lowerBound, int upperBound){
	return (rand() % (upperBound-lowerBound+1)) +lowerBound;
}

void initHarmObjects(struct harmObject *hObjs, int numbOfHarm){
	for(int i = 0;i<numbOfHarm;i++){
		hObjs[i].img = &busImage;
		hObjs[i].speed = randomNumb(1,20);
		hObjs[i].drawY = randomNumb(100,SCREEN_Y-100);
		hObjs[i].orientation = (randomNumb(0,1) ? 0 : 2);
		if(hObjs[i].orientation == 2){
			hObjs[i].speed = -(hObjs[i].speed);
			hObjs[i].drawX = SCREEN_X-(hObjs[i].img -> width);
		}else{
			hObjs[i].drawX = 0;
		}
	}	
}

struct gameMap initGameMap()
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);

	map.hObjs = malloc(H_OBJ * sizeof(struct harmObject));
	initHarmObjects(map.hObjs,H_OBJ);
	map.numbOfHarm = H_OBJ;
		
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
	gs.state = 0;
	return gs;
}

void updateHarmObjects(struct gameMap gm){
	for(int i = 0;i<gm.numbOfHarm;i++){
		gm.hObjs[i].drawX = gm.hObjs[i].drawX + gm.hObjs[i].speed;
		if(gm.hObjs[i].drawX<0){
			gm.hObjs[i].drawX = SCREEN_X-(gm.hObjs[i].img -> width);
		}else if(gm.hObjs[i].drawX >= SCREEN_X){
			gm.hObjs[i].drawX = 0;
		}
	}
}

void  *harm_obj_thread(void *arg){
	struct harmObject *obj = (struct harmObject *)arg;
	while(1){
		obj -> drawX = obj -> drawX + obj -> speed;
		if(obj -> drawX<0){
			obj -> drawX = SCREEN_X-(obj -> img -> width);
		}else if(obj -> drawX >= SCREEN_X){
			obj -> drawX = 0;
		}
	}
}

int frogCollideHarm(struct gameMap gm){
	int xOff = tileToPixel(SCREEN_X, gm.cols, gm.frogX);
	int yOff = tileToPixel(SCREEN_Y, gm.rows, gm.frogY);
	for(int i = 0;i<gm.numbOfHarm;i++){
		if(collides(xOff,yOff,&frogImage32,gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].img)){
			return 1;
		}
	}
	return 0;
}

int collides(int x1, int y1, const struct imageStruct *img1, int x2, int y2, const struct imageStruct *img2){
	if (x1 < x2  + img2 -> width &&
   x1 + img1 -> width > x2 &&
   y1 < y2 + img2 -> height &&
   y1 + img1 -> height > y2) {
	   return 1;
	}
	return 0;
}

void spawnValuePacks(int sTime, struct gameState *gs)
{
	int cTime = time(0) - sTime;
	
	if (cTime > 5)
	{
		int i = randomNumb(3, MAP_ROWS-3);
		int j = randomNumb(0, MAP_COLS-1);
		gs -> map.table[i][j].valuePack = randomNumb(1, 4);
	}
}
