#include "controller.h"
#include <pthread.h>
#include "global.h"
#include "gamelogic.h"
#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int MENU_STATE = 0;
const int PAUSE_STATE = 1;
const int GAME_STATE = 2;

int main(){
    int sTime = time(0);
    srand(sTime);
	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));
    cs -> isGameOn = 1;

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

    struct gameState gs = initGameState();
    struct gameState prevState = initGameState();
    prevState.gameStage = 5;
    prevState.map.frogX = 0;
    prevState.state = 10;

    initGFX();

    int currMenuState = 0;
    int currPauseState = 0;

    while(!gs.quit){
        wait(33333);
        if(gs.state == MENU_STATE)
        {
            drawMenuScreen(&prevState,&gs,currMenuState);
            currMenuState = updateMenuScreen(&gs, cs -> controllerButton, currMenuState);
        }
        else if (gs.state == PAUSE_STATE)
        {
            currPauseState = updatePauseScreen(&gs, cs -> controllerButton, currPauseState);
            drawPauseScreen(&prevState,&gs,currPauseState);
        }else{
            drawGameState(&prevState,&gs);
            setCurrToPrev(&prevState, &gs);
       	    updateGameState(&gs, cs -> controllerButton, sTime);
            spawnValuePacks(sTime, &gs);
        }
        // update gamestate

        // Change prev and current
    }

    return 1;
}
