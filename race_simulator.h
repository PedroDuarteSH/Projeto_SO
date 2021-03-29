#ifndef race_simulator
#define race_simulator  /* Include guard */
  
//C libraries

//User libraries
#include "shared_mem.h"
#include "functions.h"
#include "race_manager.h"

//User define variables
pid_t race_manager_process;
pid_t malfunction_manager_process;



//Structs

//Functions declarations
void gen_shared_memory();
void process_config_file(int *configs);
void race_manager_init(int incoming_shm_id);

//Usefull information
/* Config int[] format
    0 - T_units_second;
    1 - lap_distance;
    2 - lap_number;
    3 - number_of_teams (must be > 3);
    4 - max_cars_team;
    5 - T_breakdown_interval;
    6 - T_Box_min;
    7 - T_Box_Max;
    8 - Fuel_tank_capacity;
    */
#endif 