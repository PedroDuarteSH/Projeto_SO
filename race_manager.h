#ifndef race_manager
#define race_manager  /* Include guard */
  
//C libraries

//User libraries
#include "shared_mem.h"

//User define variables
int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;
//Structs

//Functions declarations
void attach_update_shm();


#endif 