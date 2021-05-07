//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef race_manager
#define race_manager  /* Include guard */

//User libraries
#include "shared_mem.h"
#include "functions.h"
#include "team_manager.h"

//Possible commands recieved
#define START_RACE "START RACE!"
#define ADD_CAR "ADDCAR"

//Command Helper
#define WRONG_COMMAND "WRONG COMMAND => "
#define CANT_ADD_TEAM "ERROR FINDING|CREATING TEAM - NO MORE SLOTS => "
#define CAR_NUMBER_EXISTS "CAR NUMBER ALREADY EXISTS OR NO MORE TEAM SLOTS=> "
#define CAR_ADDED "CAR ADDED SUCCESSFULLY => "
#define RACE_STARTING "RACE IS GOING TO START!"
#define START_ERROR "CANNOT START, NOT ENOUGH TEAMS"
#define RACE_STARTED_ERR "CANNOT INPUT MORE COMMANDS, RACE ALREADY STARTED"

//Functions declarations
//race manager initiation
void race_manager_init();
void create_pipes();
void clean_data();
//shared memory attach to process
void attach_update_race_shm();

//Processes command recieved in file/namedpipe(later)
char* process_command(char *line);
    //if add car
    char* add_car(char *line);
        int verify_car_command(char *line, char ** line_splited);
        //If verify_car_command == true (Valid command), keeps adding
            team_stuct *find_team(char *team_name);//find the team with same name
            //if same name returns team Address
            //if can create team
                team_stuct *create_team(team_stuct* team_struct, char *team_name, int team_number);
            //else return FALSE
            car_struct * find_car_pos(team_stuct *car_team, int number);
        //if start race:
        int verify_teams();//Verifies if enough teams are created
        //if verify teams == true
            void start_race();
        



#endif
