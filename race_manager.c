//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager

#include "race_manager.h"

void race_manager_init(int incoming_shm_id){
    int shm_id = incoming_shm_id;

    shr_memory *shm_struct = NULL;
    config *config_struct = NULL;

    attach_update_shm(shm_id, shm_struct, config_struct);
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

    #ifdef debug
    print_config_file(config_struct);
    #endif

}
void print_config_file(config *config_struct){
    printf("%d\n",config_struct->T_units_second);
    printf("%d\n", config_struct->lap_distance);
    printf("%d\n", config_struct->lap_number);
    printf("%d\n", config_struct->number_of_teams);
    printf("%d\n", config_struct->max_cars_team);
    printf("%d\n", config_struct->T_breakdown_interval);
    printf("%d\n", config_struct->T_Box_min);
    printf("%d\n", config_struct->T_Box_Max);
    printf("%d", config_struct->Fuel_tank_capacity);
}

void attach_update_shm(int shm_id,  shr_memory *shm_struct, config *config_struct ){
    shm_struct = shmat(shm_id, NULL, 0);
    config_struct = shmat(shm_struct->config_shmid, NULL, 0);
    
    //Attact race struct
    //Ininitiates

}