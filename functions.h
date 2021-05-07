//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef functions   /* Include guard */
#define functions

//C libraries
#include "shared_mem.h"
//User define variables
#define line_lenght 20
#define number_of_configs 9

#define LOG_SEM_NAME "log_semaphore"

//Functions declarations
void clear_resources();
void remove_shm();
void remove_msq();

//Config file gesture
int *read_config_file();
void print_config_file();
void process_config_file(int *configs);

//String auxiliar
char * concat (char *s1, char *s2);
void strip(char *phrase);

//Log_writing
void print(char *result);

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
