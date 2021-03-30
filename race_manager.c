//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager
#include "race_manager.h"

//initiated in father process
int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;
team **teams;

#define TRUE 1
#define FALSE 0

//temporary
#define INPUT_LENGHT 200
#define SMALL_STR_LENGHT 10
void race_manager_init(int incoming_shm_id){
    attach_update_shm(incoming_shm_id);

#ifdef debug
    print_config_file();
#endif

    //avisar o semaforo que a corrida
    //ler o ficheiro / Receber namedPipe
    FILE *cars_file = fopen("cars.temp", "r");

    char *line = malloc(sizeof(char) * INPUT_LENGHT);
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    int game_started;
    while (TRUE){
        line[0] = 0;
        sem_getvalue(&race_struct->race_begin, &game_started);
        fgets(line, INPUT_LENGHT, cars_file);
        if (game_started == FALSE){

            process_command(line);
        }
        else{
            if(line[0] != 0) //invalid command
                print("Rejected, race already started!");
            break;
        }
    }
    free(command);
    
    

    //Espera que o setup das equipas esteja feito
    //Começa a Corrida
    //pid_t new_team;
    //Incia Carros
    /*
    for(int i = 0;i < 2;i++){ //mudar o 2
        new_team = fork();
        if(new_team == 0){
          team_manager();
       }
    }*/
}

void print_config_file(){
    printf("%d\n", config_struct->T_units_second);
    printf("%d\n", config_struct->lap_distance);
    printf("%d\n", config_struct->lap_number);
    printf("%d\n", config_struct->number_of_teams);
    printf("%d\n", config_struct->max_cars_team);
    printf("%d\n", config_struct->T_breakdown_interval);
    printf("%d\n", config_struct->T_Box_min);
    printf("%d\n", config_struct->T_Box_Max);
    printf("%d\n", config_struct->Fuel_tank_capacity);
}

void attach_update_shm(int incoming_shm_id){
    //first 3 lines wasn't needed (already attached in father process)
    shm_id = incoming_shm_id;
    shm_struct = shmat(shm_id, NULL, 0);
    config_struct = shmat(shm_struct->config_shmid, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);
    //iniciar equipas
    if ((race_struct->teams_shmid = shmget(IPC_PRIVATE, sizeof(team *) * config_struct->number_of_teams, IPC_CREAT | 0777)) < 1)
    {
        print("Error in shmget with IPC_CREAT creating teams array");
        exit(1);
    }

    teams = shmat(race_struct->teams_shmid, NULL, 0);
    for (int i = 0; i < config_struct->number_of_teams; i++)
        teams[i] = NULL;
}

void start_race(){

}

team *find_team(char *team_name){
    int i = 0;
    for (i = 0; i < config_struct->number_of_teams; i++){
        if(teams[i] == NULL){
            int team_id;
            if ((team_id = shmget(IPC_PRIVATE, sizeof(team), IPC_CREAT | 0777)) < 1){
                perror("Error in shmget with IPC_CREAT\n");
                exit(1);
            }
            teams[i] = shmat(team_id, NULL, 0);
            if ((teams[i]->cars_shmid = shmget(IPC_PRIVATE, sizeof(car*) * config_struct->max_cars_team, IPC_CREAT | 0777)) < 1){
                perror("Error in shmget with IPC_CREAT\n");
                exit(1);
            }
            strcpy(teams[i]->name, team_name); 
            teams[i]->number_team_cars = 0;
            return teams[i];
        }
        else if(strcmp(teams[i]->name, team_name) == 0){
            print("Found team");
            return teams[i];
        }
            
    }
}

car *add_car(char *line){
    char *team_name = malloc(sizeof(char) * SMALL_STR_LENGHT);
    char *temp;
    strtok(line, ", \n");//First 
    strtok(NULL, ", \n");//team
    team_name = strtok(NULL, ", ");
    team *t = find_team(team_name);
    int car_shmid;
    if ((car_shmid = shmget(IPC_PRIVATE, sizeof(car) * config_struct->max_cars_team, IPC_CREAT | 0777)) < 1){
        perror("Error in shmget with IPC_CREAT\n");
        exit(1);
    }
    car *c = shmat(car_shmid, NULL, 0);
    strtok(NULL, ", \n");
    c->number = strtol(strtok(NULL, ", \n"), &temp, 10);
    strtok(NULL, ", \n");
    c->speed = strtol(strtok(NULL, ", \n"), &temp, 10);
    strtok(NULL, ", \n");
    c->consumption = strtoll(strtok(NULL, ", \n"), &temp, 10);
    strtok(NULL, ", \n");
    c->reliability = strtol(strtok(NULL, ", \n"), &temp, 10);
    t->number_team_cars++;
}

int verify_teams(){
    for (int i = 0; i < config_struct->number_of_teams; i++)
        if (teams[i] == NULL)
            return TRUE;
    return FALSE;
}


void process_command(char *line){
    strip(line);
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    strcpy(command, line);
    strtok(command, " ");
    if (strcmp(line, "START RACE!") == 0){
        if (verify_teams() == TRUE){
            start_race();
            sem_post(&race_struct->race_begin);
        }
        else print("CANNOT START, NOT ENOUGH TEAMS");
    }
    else if (strcmp(command, "ADDCAR") == 0){
        add_car(line);
        sleep(1);
    }
    else print(strcat("WRONG COMMAND => ", line));
}

