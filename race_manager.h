//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef race_manager
#define race_manager  /* Include guard */

//User libraries
#include "shared_mem.h"
#include "functions.h"
#include "team_manager.h"

race * race_data;
//Functions declarations
//race manager initiation
void race_manager_init();

//shared memory attach to process
void attach_update_race_shm();

//Processes command recieved in file/namedpipe(later)
int process_command(char *line);
    //if add car
    int add_car(char *line);
        int verify_car_command(char *line, char ** line_splited);
        //If verify_car_command == true (Valid command), keeps adding
            team *find_team(char *team_name);//find the team with same name
            //if same name returns team Address
            //if can create team
                team *create_team(char *team_name, int i);
            //else return FALSE
    //if start race:
        int verify_teams();//Verifies if enough teams are created
        //if verify teams == true
            void start_race();
        



#endif
