//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef team_manager
#define team_manager

#include "shared_mem.h"
#include "functions.h"

//functions
void team_manager_start(team_stuct *self);
car_struct **find_team_cars();
void *car_init(void * shm_ids);
#endif
