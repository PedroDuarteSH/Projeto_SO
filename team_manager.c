//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "shared_mem.h"
/*
void team_manager(){
    for(int i = 0;i < 2;i++){ //mudar o 2
        id[i] = 1;
        pthread_create(&cars[i],NULL,cars,&id[i]); //mudar o cars
    }
    //Recebe menssagens pela queque e repara o carro se for preciso 
}*/

shr_memory *shm_struct;
team **teams;
race *race_struct;
int *id;
int shm_id;
pthread_t *cars;

void team_manager(int incoming_shm_id){
    attach_update_shm(incoming_shm_id);
    team_manager_init();
    //gestaoBox()
    //gerirAbastecimentoDosCarros
}
void team_manager_init(){
    sleep(2);
    
    for(int i = 0;i < teams[i]->number_team_cars;i++){
        cars[teams[i]->number_team_cars];
        id[i] = 1;
        pthread_create(&cars[i],NULL,cars,&id[i]); //mudar o cars
    }
}


void attach_update_shm(int incoming_shm_id){
    shm_id = incoming_shm_id;
    shm_struct = shmat(shm_id, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);
    teams = shmat(race_struct->teams_shmid, NULL, 0);
}