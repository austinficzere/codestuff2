#include "controller.h"
#include <pthread.h>
#include "global.h"
#include "gamelogic.h"
#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    int sTime = time(0);
	struct ControllerStruct *cs;
	cs = malloc(sizeof(struct ControllerStruct));

	pthread_t controller_id;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&controller_id,&attr,controller_thread, (void *) cs);

    struct gameState gs = initGameState();
    struct gameState prevState = initGameState();
    prevState.map.frogX = 0;

    initGFX();

    while(1){
        drawGameState(&prevState,&gs);
        wait(100000);
        // update gamestate
        setCurrToPrev(&prevState, &gs);
        //updateGameState(&gs, cs -> controllerButton, sTime);
        if(isButtonPressed(cs -> controllerButton,4)){
			gs.map.frogY--;
		}
		if(isButtonPressed(cs -> controllerButton,5)){
			gs.map.frogY++;
		}
		if(isButtonPressed(cs -> controllerButton,6)){
			gs.map.frogX--;
		}
		if(isButtonPressed(cs -> controllerButton,7)){
			gs.map.frogX++;
		}
        // Change prev and current
    }

    return 0;
}