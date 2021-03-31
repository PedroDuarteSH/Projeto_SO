//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"


team *this_team;
car **team_cars;
pthread_t *cars;

void team_manager_start(int i){
    attach_update_team_shm(i);
    team_manager_init();
    //gestaoBox()
    //gerirAbastecimentoDosCarros
}
void team_manager_init(){
    print(concat("INITIATING TEAM CARS ARRAY ", this_team->name));
    cars =(pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);
    //shm_indexes = malloc(sizeof(int) * 1);
    //Colocar endereços de shared memory necessarios aos carros
    int race_starting;
    sem_post(&race_struct->teams_ready);

    //Wait for race to start
    sem_wait(&race_struct->race_begin);

    car ** team_cars = shmat(this_team->cars_shmid, NULL, 0);
    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_create(&cars[i],NULL,car_init,&i); //mudar o cars
    }

    //print(concat("READY: CARS OF TEAM ", this_team->name));

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //Wait for car threads to finish
    }
    //print(concat("Dead team: ", this_team->name));
    exit(0);
}


void attach_update_team_shm(int i){
    shm_struct = shmat(shm_id, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);
    team **teams = shmat(race_struct->teams_shmid, NULL, 0);
    this_team = teams[i];
    shmdt(teams);
}


void *car_init(void * arg){
    int i = *(int *)(arg);
    print("HERE");
    car * c = team_cars[i];
    char *string = "";
    print(concat("CAR RACCING", string));
    sleep(3);
    pthread_exit(NULL);
}