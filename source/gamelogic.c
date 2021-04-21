/* 
 * INFORMATION FOR TA
 * Class: CPSC 359
 * Names: Austin Ficzere(30093971) and Manbir Sandhu(30086574)
 * Description: The game logic of our game, updates the game screens, has logic for colliding objects, and 
 * initializes our structures.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gamelogic.h"
#include <time.h>
#include "controller.h"
#include "global.h"
#include "gfx.h"

// constants
const int NUMB_LIVES = 3;
const int START_TIME = 300;
const int NUMB_MOVES = 300;
const int MAP_ROWS = 20;
const int MAP_COLS = 30;
const int H_OBJ = 15;
const int NUMB_OF_STAGES = 4;
const int ITEM_SPAWN_DIFF = 3;

// defining functions/structures
struct gameState initGameState();
int isGameEnd(struct gameState *gs);
void spawnValuePacks(int sTime, struct gameState *gs);
void setCurrToPrevMap(struct gameMap *prev, struct gameMap *curr);
void updateHarmObjects(struct gameMap gm);
int collides(int left1, int right1, const struct imageStruct *img1, int left2, int right2, const struct imageStruct *img2);
int frogCollideHarm(struct gameMap gm, int frogX, int frogY);
int updateFrogStage(struct gameMap gm, int frogX, int frogY);

/*
@Params: 
        rows: number of rows
		cols: number of columns
@Returns: a pointer of pointers of Tile structures, which is essentially a 2D array
We make space for our 2D array of Tile structs and then return the 2D array.
*/
struct Tile **createTable(int rows, int cols)
{
	struct Tile **table = (struct Tile **) malloc(rows*sizeof(struct Tile *));
	for(int i = 0;i<rows;i++)
	{
		table[i] = (struct Tile *) malloc(cols * sizeof(struct Tile));
	}
	return table;
}

/*
@Params: 
        tc: a Tile struct we want to copy
@Returns: a new Tile struct with the same values as the Tile struct that was passed in
We copy a Tile struct and return the new copied Tile.
*/
struct Tile copyTile(struct Tile tc){
	struct Tile ret;
	ret.valuePack = tc.valuePack;
	ret.canStand = tc.canStand;

	return ret;
}

/*
@Params: 
        gs: pointer to the gamestate
@Returns: 1 if the game is over and 0 otherwise
Checks if the game is over, so either the lives/time/movesLeft are 0, or we have won/lost/quit.
*/
int isGameEnd(struct gameState *gs)
{
	if(gs -> hasWon)
		return 1;
	else if(gs->numbLives == 0 || gs->time == 0 || gs -> movesLeft == 0 || gs -> hasLost || gs -> quit){
		gs -> hasLost = 1;
		return 1;	
	}
	return 0;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		menuState: an int that tells us which button on the screen is selected
@Returns: the new menuState if it was changed or the old one if it was not changed
Updates the menu screen and checks if we have switched the button selected or if the user has pressed one of the
buttons.
*/
int updateEndScreen(struct gameState *gs, int button){
	if(button != NONE_PRESSED){
		*gs = initGameState();
	}
}

int updateMenuScreen(struct gameState *gs, int button, int menuState)
{
	// if we are selecting play and they press A
	if (menuState == 0 && isButtonPressed(button,A_BUTTON))
	{
		gs -> state = 2;
	}
	// if we are selecting quit and they press A
	else if (menuState == 1 && isButtonPressed(button,A_BUTTON))
	{
		gs -> quit = 1;
	}
	// if they press up we select the top button
	else if (isButtonPressed(button,UP_BUTTON))
	{
		menuState = 0;
	}
	// if they press down we select the bottom button
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		menuState = 1;
	}
	return menuState;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		pauseState: an int that tells us which button on the screen is selected
@Returns: the new pauseState if it was changed or the old one if it was not changed
Updates the pause screen and checks if we have switched the button selected or if the user has pressed one of the
buttons.
*/
int updatePauseScreen(struct gameState *gs, int button, int pauseState)
{
	// if they press restart button, we reset the game
	if ((pauseState == 0 || pauseState == 1) && isButtonPressed(button,A_BUTTON))
	{
		*gs = initGameState();
		if(pauseState == 0){
			gs -> state = 2;
		}
	}
	// if they press up we select the top button
	else if (isButtonPressed(button,UP_BUTTON))
	{
		pauseState = 0;
	}
	// if they press down we select the bottom button
	else if (isButtonPressed(button,DOWN_BUTTON))
	{
		pauseState = 1;
	}
	// if they press start we resume the game
	else if (isButtonPressed(button,START_BUTTON))
	{
		gs -> state = 2;
	}
	return pauseState;
}

/*
@Params: 
        gs: pointer to the gamestate
		button: the button binary num that tells us which buttons are pressed
		startTime: the time the game started
@Returns: none
Updates the game frog when a button is pressed, checks for collision, checks if the game ended, spawns
value packs, updates the harm objects, and updates the game state values.
*/
void updateGameState(struct gameState *gs, int button, int startTime)
{
	// check if game is over
	if (isGameEnd(gs)){
		gs -> state = 3;
	}

	if(frogCollideHarm(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY) && gs -> gameStage != 3){
		gs -> numbLives--;
		gs -> frogX = MAP_COLS/2;
		gs -> frogY = MAP_ROWS-2;
	}

	if(!(frogCollideHarm(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY)) && gs -> gameStage == 3 && gs -> frogY < MAP_ROWS - 2 && gs -> frogY > 2){
		gs -> numbLives--;
		gs -> frogX = MAP_COLS/2;
		gs -> frogY = MAP_ROWS-2;
	}else if(gs -> gameStage == 3){
		int updateX = updateFrogStage(gs -> map[gs -> gameStage], gs -> frogX, gs -> frogY);
		gs -> frogX = updateX;
	}


	// update harm objects
	updateHarmObjects(gs -> map[gs -> gameStage]);

	// if they press start the pause screen opens
	if (isButtonPressed(button,START_BUTTON)){
		gs -> state = 1;
	}

	// spawn value packs
	spawnValuePacks(startTime, gs);

	// checking for d pad button presses
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

	// if we reach the end of screen
	if (gs -> frogY == 0)
	{
		// check if we are in last stage, if we are they win
		if (gs -> gameStage == 3)
			{
				gs -> hasWon = 1;
				gs -> score = ((gs -> time) + (gs -> movesLeft) + (gs -> numbLives)) * 2;
			}
			// otherwise go to next stage
			else
			{
				gs -> gameStage++;
				gs -> frogY = MAP_ROWS - 2;
				gs -> score++;
			}
	// check if the go back and want to return to previous stage
	}else if(gs -> gameStage != 0 && gs -> frogY == MAP_ROWS -1){
		gs -> gameStage--;
		gs -> frogY = 1;
	}

	// update time
	gs -> time = time(0) - startTime;

}

int updateFrogStage(struct gameMap gm, int frogX, int frogY){
	int xOff = tileToPixel(SCREEN_X, gm.cols, frogX);
	int yOff = tileToPixel(SCREEN_Y, gm.rows, frogY);
	for(int i = 0;i<gm.numbOfHarm;i++){
		if(collides(xOff,yOff,&frogImage32,gm.hObjs[i].drawX,gm.hObjs[i].drawY,gm.hObjs[i].img) && !collides(xOff,yOff,&frogImage32,gm.hObjs[i].drawX + gm.hObjs[i].speed,gm.hObjs[i].drawY,gm.hObjs[i].img) ){
			if(gm.hObjs[i].speed<0){
				frogX--;
			}else{
				frogX++;
			}
		}
	}
	return frogX;
}

/*
@Params: 
        prev: a pointer to the previous game map
		curr: a pointer to the current game map
@Returns: none
We set the current game map to previous one so that we can access it.
*/
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

/*
@Params: 
        prev: a pointer to the previous game state
		curr: a pointer to the current game state
@Returns: none
We set the current game state to previous one so that we can access it.
*/
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

/*
@Params: 
        lowerBound: the lower bound of the random num
		upperBound: th upper bound of the random num
@Returns: the random number within the range
Finds a random number in a given range.
*/
int randomNumb(int lowerBound, int upperBound){
	return (rand() % (upperBound-lowerBound+1)) +lowerBound;
}

/*
@Params: 
        hObjs: a pointer to the harmObject struct
		numbOfHarm: the number of harm objects we want
		gameStage: the current stage of the game
@Returns: none
Initializes the harm objects on the screen to random spots.
*/
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
		low = 5;
		high = 6;
	}
	int count = 2;
	for(int i = 0;i<numbOfHarm;i++){
		hObjs[i].img = hObjImg.imgs[randomNumb(low,high)];
		if(gameStage == 3){
			hObjs[i].speed = randomNumb(1,5);
		}else{
			hObjs[i].speed = randomNumb(1,20);
		}
		if(gameStage == 3){
			hObjs[i].drawY = tileToPixel(SCREEN_Y,MAP_ROWS,count);
			count++;
			if(count == 18)
				count = 2;
		}else{
			hObjs[i].drawY = randomNumb(100,SCREEN_Y-110);
		}
		hObjs[i].orientation = (randomNumb(0,1) ? 0 : -1);
		if(hObjs[i].orientation == -1){
			hObjs[i].speed = -(hObjs[i].speed);
		}
		hObjs[i].drawX = randomNumb(0,SCREEN_X);
	}	
}

/*
@Params: 
        gameStage: the stage of the game
@Returns: the initial gameMap struct
Intiializing the gameMap.
*/
struct gameMap initGameMap(int gameStage)
{
	struct gameMap map;
	map.table = createTable(MAP_ROWS,MAP_COLS);

	if(gameStage != 3){
		map.hObjs = malloc(H_OBJ * sizeof(struct harmObject));
		initHarmObjects(map.hObjs,H_OBJ, gameStage);
		map.numbOfHarm = H_OBJ;
	} else{
		map.hObjs = malloc(18 * sizeof(struct harmObject));
		initHarmObjects(map.hObjs,18, gameStage);
		map.numbOfHarm = 18;
	}
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

/*
@Params: none
@Returns: the initial gameState struct
Intiializing the gameState.
*/
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

/*
@Params: 
        gm: the gameMap struct
@Returns: none
Updates the posisitions of the harm objects on the screen.
*/
void updateHarmObjects(struct gameMap gm){
	for(int i = 0;i<gm.numbOfHarm;i++){
		gm.hObjs[i].drawX = gm.hObjs[i].drawX + gm.hObjs[i].speed;
		int width = gm.hObjs[i].img -> width;
		if(gm.hObjs[i].drawX + width<0){
			gm.hObjs[i].drawX = SCREEN_X-(gm.hObjs[i].img -> width);

		}else if(gm.hObjs[i].drawX >= SCREEN_X){
			gm.hObjs[i].drawX = 0;
		}
	}
}

/*
@Params: 
        arg: pointer to the argument we pass to the thread
@Returns: none
The thread that acts as a harm object.
*/
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

/*
@Params: 
        gm: the gameMap of the game
		frogX: the x position of the frog on the screen
		frogY: the y position of the frog on the screen
@Returns: 1 if the frog has collided with something and 0 otherwise
Checks if the frog has collided with a harm object.
*/
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

/*
@Params: 
        x1: x value of image 1
		y1: y value of image 1
		img1: pointer to image 1
		x2: x value of image 2
		y2: y value of image 2
		img2: pointer to image 2
@Returns: 1 if the image has collided with another image and 0 otherwise
Checks if two images have collided on the screen.
*/
int collides(int x1, int y1, const struct imageStruct *img1, int x2, int y2, const struct imageStruct *img2){
	if (x1 < x2  + img2 -> width &&
   x1 + img1 -> width > x2 &&
   y1 < y2 + img2 -> height &&
   y1 + img1 -> height > y2) {
	   return 1;
	}
	return 0;
}

/*
@Params: 
        sTime: the start time of the game
		gs: the gameState struct
@Returns: none
Spawns value packs in the game if we have waited 30 seconds to random spots on the screen.
*/
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
