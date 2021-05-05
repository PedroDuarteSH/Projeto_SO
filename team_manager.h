//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef team_manager
#define team_manager

#include "shared_mem.h"
#include "functions.h"

//functions
void team_manager_start(int i);
car ** find_team_cars();
void attach_update_team_shm(int i);
void *car_init(void * shm_ids);
#endif
