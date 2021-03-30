#ifndef team_manager
#define team_manager

#include "shared_mem.h"
#include "functions.h"

//functions
void team_manager_start(int i);
void team_manager_init();
void attach_update_team_shm(int i);
void *car_init(void * shm_ids);
#endif