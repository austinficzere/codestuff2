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
const int NUMB_OF_STAGES = 4;
const int ITEM_SPAWN_DIFF = 3;

struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void spawnValuePacks(int sTime, struct gameState *gs);
void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr);
void updateHarmObjects(struct gameMap gm);
int collides(int left1, int right1, const struct imageStruct *img1, int left2, int right2, const struct imageStruct *img2);

int frogCollideHarm(struct gameMap gm, int frogX, int frogY);


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
	if ((pauseState == 0 || pauseState == 1) && isButtonPressed(button,A_BUTTON))
	{
		*gs = initGameState();
		if(pauseState == 0){
			gs -> state = 2;
		}
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

	if(frogCollideHarm(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY)){
		gs -> numbLives--;
		gs -> frogX = MAP_COLS/2;
		gs -> frogY = MAP_ROWS-2;
	}

	// deal with quit, which we check if we are in stage -1, and they press A on correct button
	updateHarmObjects(gs -> map[gs -> gameStage]);
	if (isButtonPressed(button,START_BUTTON)){
		gs -> state = 1;
	}

	spawnValuePacks(startTime, gs);

	if(button!=NONE_PRESSED){
		if (isButtonPressed(button,UP_BUTTON) && gs -> frogY>0)
		{
			gs -> orientation = 0;
			gs -> frogY--;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,DOWN_BUTTON) && gs -> frogY<MAP_ROWS-1)
		{
			gs -> orientation = 2;
			gs -> frogY++;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,LEFT_BUTTON) && gs -> frogX>0)
		{
			gs -> orientation = 3;
			gs -> frogX--;
			gs -> movesLeft--;
		}
		if (isButtonPressed(button,RIGHT_BUTTON) && gs -> frogX<MAP_COLS)
		{
			gs -> orientation = 1;
			gs -> frogX++;
			gs -> movesLeft--;
		}
	}

	// Check collision with harm object
	if(gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack != 0){
		int valueStand  = gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack;
		// do stuff
		if (valueStand == 1)
		{
			gs -> numbLives++;
		}
		else if (valueStand == 2)
		{
			gs -> score++;
		}
		else if (valueStand == 3)
		{
			gs -> time = (gs -> time) + 5;
		}
		else if (valueStand == 4)
		{
			gs -> movesLeft = (gs -> movesLeft) + 5;
		}

		gs -> map[gs -> gameStage].table[gs -> frogY][gs -> frogX].valuePack = 0;
	}

	if (gs -> frogY == 0)
	{
		if (gs -> gameStage == 3)
			{
				gs -> hasWon = 1;
				gs -> score = ((gs -> time) + (gs -> movesLeft) + (gs -> numbLives)) * 2;
			}
			else
			{
				gs -> gameStage++;
				gs -> frogY = MAP_ROWS - 2;
				gs -> score++;
			}
	}else if(gs -> gameStage != 0 && gs -> frogY == MAP_ROWS -1){
		gs -> gameStage--;
		gs -> frogY = 1;
	}

	gs -> time = time(0) - startTime;

}

void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr){
	for(int i = 0;i<MAP_ROWS;i++){
		for(int j = 0;j<MAP_COLS;j++){
			prev -> table[i][j] = copyTile(curr -> table[i][j]);
		}
	}

	for(int i = 0;i<curr -> numbOfHarm;i++){
		prev -> hObjs[i].img = curr -> hObjs[i].img;
		prev -> hObjs[i].drawX = curr -> hObjs[i].drawX;
		prev -> hObjs[i].drawY = curr -> hObjs[i].drawY;
		prev -> hObjs[i].speed= curr -> hObjs[i].speed;
	}
}

void setCurrToPrev(struct gameState *prev, struct gameState *curr){

	for(int i = 0;i<NUMB_OF_STAGES;i++){
		setCurrToPrevMap(&(prev -> map[i]), &(curr -> map[i]));
	}
	prev -> frogX = curr -> frogX;
	prev -> frogY = curr -> frogY;
	prev -> orientation = curr -> orientation;
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

void initHarmObjects(struct harmObject *hObjs, int numbOfHarm, int gameStage){
	int low;
	int high;
	if(gameStage == 0){
		low = 0;
		high = 1;
	}else if(gameStage == 1){
		low = 2;
		high = 3;
	}else if(gameStage == 2){
		low = 4;
		high = 4;
	}else{
		low = 1;
		high = 1;
	}

	for(int i = 0;i<numbOfHarm;i++){
		hObjs[i].img = hObjImg.imgs[randomNumb(low,high)];
		hObjs[i].speed = randomNumb(1,20);
		hObjs[i].drawY = randomNumb(100,SCREEN_Y-100);
		hObjs[i].orientation = (randomNumb(0,1) ? 0 : -1);
		if(hObjs[i].orientation == -1){
			hObjs[i].speed = -(hObjs[i].speed);
		}
		hObjs[i].drawX = randomNumb(0,SCREEN_X);
	}	
}

struct gameMap initGameMap(int gameStage)
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);

	map.hObjs = malloc(H_OBJ * sizeof(struct harmObject));
	initHarmObjects(map.hObjs,H_OBJ, gameStage);
	map.numbOfHarm = H_OBJ;
	map.lastItemSpawn = time(0);
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
	for(int i = 0;i<NUMB_OF_STAGES;i++){
		gs.map[i] = initGameMap(i);
	}
	gs.frogX = MAP_ROWS/2;
	gs.frogY = MAP_ROWS-2;
	gs.orientation = 0;
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
		if(gm.hObjs[i].drawX + gm.hObjs[i].img -> width<0){
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

int frogCollideHarm(struct gameMap gm, int frogX, int frogY){
	int xOff = tileToPixel(SCREEN_X, gm.cols, frogX);
	int yOff = tileToPixel(SCREEN_Y, gm.rows, frogY);
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
	int spawnDiff = time(0) - gs -> map[gs -> gameStage].lastItemSpawn;
	
	if (cTime > 30 && spawnDiff > ITEM_SPAWN_DIFF)
	{
		int i = randomNumb(3, MAP_ROWS-3);
		int j = randomNumb(0, MAP_COLS-1);
		gs -> map[gs -> gameStage].table[i][j].valuePack = randomNumb(1, 4);
		gs -> map[gs -> gameStage].lastItemSpawn = time(0);
	}
}
