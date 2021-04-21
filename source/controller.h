#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

// define functions
void *controller_thread(void *);
int isButtonPressed(int button, int i);
void wait(int time);

// define constants
const int NONE_PRESSED;
const int B_BUTTON;
const int Y_BUTTON;
const int SELECT_BUTTON;
const int START_BUTTON;
const int UP_BUTTON;
const int DOWN_BUTTON;
const int LEFT_BUTTON;
const int RIGHT_BUTTON;
const int A_BUTTON;
const int X_BUTTON;
const int LEFT_BUMPER;
const int RIGHT_BUMPER;

#endif
