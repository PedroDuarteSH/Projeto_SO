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

    while(fgets(line, INPUT_LENGHT, cars_file) != NULL){
        //sem_getvalue -> SABER VALOR DO SEMAFORO
        strip(line);
        strcpy(command, line);
        command = strtok(command, " ");

        if(strcmp(command, "START RACE!") == 0){
            if(verify_teams() == TRUE){

            }

            
            
            
            break;
        }
        else if(strcmp(command, "ADDCAR") ==0){ 
            
            sleep(1);
        }
        else
            print(strcat("WRONG COMMAND => ", line));        
    }
    free(command);
    print("Race Started");
    sem_post(&race_struct->race_begin);
    
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
    printf("%d\n",config_struct->T_units_second);
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
    if ((race_struct->teams_shmid = shmget(IPC_PRIVATE, sizeof(team *) * config_struct->number_of_teams, IPC_CREAT | 0777)) < 1){
        print("Error in shmget with IPC_CREAT creating teams array");
        exit(1);
    }

    teams = shmat(race_struct->teams_shmid, NULL, 0);
    for (int i = 0; i < config_struct->number_of_teams; i++)
        teams[i] = NULL;
    
    
}

team * find_key(){




}


car *ADD_CAR(char *line){



}

int verify_teams(){
    for (int i = 0; i < config_struct->number_of_teams; i++){
        if(teams[i] == NULL)
                    return FALSE;
    return TRUE;
}

