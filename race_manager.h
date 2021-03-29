#ifndef race_manager
#define race_manager  /* Include guard */
  
//C libraries

//User libraries
#include "shared_mem.h"
#include "functions.h"
//User define variables
//Structs

//Functions declarations
void attach_update_shm(int shm_id);
void print_config_file();
void race_manager_init(int incoming_shm_id);

#endif