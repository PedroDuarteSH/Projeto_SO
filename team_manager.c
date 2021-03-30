//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"


shr_memory *shm_struct;
team *this_team;
race *race_struct;
int *shm_indexes;
int shm_id;
pthread_t *cars;

void team_manager_start(int incoming_shm_id, int i){
    attach_update_team_shm(incoming_shm_id, i);
    team_manager_init();
    //gestaoBox()
    //gerirAbastecimentoDosCarros
}
void team_manager_init(){
    sleep(2);
    cars =(pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);
    
    //Colocar endereços de shared memory necessarios aos carros
    shm_indexes[0] = 1;
    
    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_create(&(cars[i]),NULL,car_init,&shm_indexes); //mudar o cars
    }
    print(concat("READY: CARS OF TEAM", this_team->name));

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //mudar o cars
    }
    exit(0);
}


void attach_update_team_shm(int incoming_shm_id, int i){
    shm_id = incoming_shm_id;
    shm_struct = shmat(shm_id, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);
    team **teams = shmat(race_struct->teams_shmid, NULL, 0);
    this_team = teams[i];
    shmdt(teams);
}