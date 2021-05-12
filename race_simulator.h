//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef race_simulator
#define race_simulator  /* Include guard */

//User libraries
#include "shared_mem.h"
#include "functions.h"
#include "race_manager.h"
#include "malfunction_manager.h"

void init_program();
void init_log();
void create_named_pipe(char *name);
void create_msq();
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
