#ifndef race_manager
#define race_manager  /* Include guard */

//variable defines
#define CAR_INPUT_SIZE 11
#define TRUE 1
#define FALSE 0

//Car management
#define INVALID_COMMAND 0
#define CANT_ADD_TEAM 1
#define CAR_ADDED 2

//String siz management
#define INPUT_LENGHT 200
#define SMALL_STR_LENGHT 10

//C libraries

//User libraries
#include "shared_mem.h"
#include "functions.h"
#include "team_manager.h"

//User define variables
int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;
team **teams;


//Structs

//Functions declarations
//race manager initiation
void race_manager_init(int incoming_shm_id);

//shared memory attach to process
void attach_update_race_shm(int incoming_shm_id);

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