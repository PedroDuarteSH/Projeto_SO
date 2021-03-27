#ifndef race_simulator   /* Include guard */
#define race_simulator

//C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//User define variables
#define line_lenght 20
#define number_of_configs 9
//Structs

//Functions declarations
int *read_config_file();
void strip(char *phrase);

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