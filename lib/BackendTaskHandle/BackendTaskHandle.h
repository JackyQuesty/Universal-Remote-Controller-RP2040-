#ifndef BACKENDTASKHANDLE_H
#define BACKENDTASKHANDLE_H

extern volatile bool PCF_INT_Flag;

void task_InputRead(void*);

void init_core0();

#endif