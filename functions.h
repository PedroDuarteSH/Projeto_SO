#ifndef functions   /* Include guard */
#define functions

//C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared_mem.h"
//User define variables
#define line_lenght 20
#define number_of_configs 9

int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;
//Structs

//Functions declarations
int *read_config_file();
void strip(char *phrase);
void global_init_log(FILE *input_log_file, sem_t input_log_sem);
void print(char *result);
char * concat (char * s1, char * s2);
void gen_shared_memory();
void process_config_file(int *configs);
void init_log();


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