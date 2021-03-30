#ifndef team_manager
#define team_manager

#include "shared_mem.h"
#include "cars.c"
#include "functions.h"

//functions
void team_manager_start(int incoming_shm_id, int i);
void team_manager_init();
void attach_update_team_shm(int incoming_shm_id, int i);
#endif