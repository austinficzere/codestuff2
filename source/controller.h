#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

void *controller_thread(void *);
int isButtonPressed(int button, int i);
void wait(int time);

const int NONE_PRESSED;

#endif
