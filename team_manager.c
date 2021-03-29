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

int shm_id;
void team_manager_init(int incoming_shm_id){
    sleep(2);
}
void team_manager_add_car(){

}