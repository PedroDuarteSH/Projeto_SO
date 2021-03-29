//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager
#include "race_manager.h"

//initiated in father process
int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;

void race_manager_init(int incoming_shm_id){
    attach_update_shm(incoming_shm_id);
    
    #ifdef debug
    print_config_file();
    #endif
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
    printf("%d", config_struct->Fuel_tank_capacity);
}

void attach_update_shm(int incoming_shm_id){

    //first 3 lines wasn't needed (already attached in father process)
    shm_id = incoming_shm_id;
    shm_struct = shmat(shm_id, NULL, 0);
    config_struct = shmat(shm_struct->config_shmid, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);

    race_struct->status = .1;

}