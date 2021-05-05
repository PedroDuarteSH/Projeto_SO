//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"


team *this_team;
pthread_t *cars;


void team_manager_start(int team_number){
#ifdef DEBUG
    print("Searching for team");
#endif
    this_team = (team *)(race_struct + 1 + team_number);
#ifdef DEBUG
    print(concat("INITIATING TEAM CARS ARRAY ", this_team->name));
#endif

    car**team_cars = find_team_cars();
    
    cars = (pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);
    printf("Equipa %s\n", this_team->name);
    for(int i = 0; i < this_team->number_team_cars;i++){
        //pthread_create(&cars[i],NULL,car_init,c);
        printf("Carro %d da equipa %s pronto", team_cars[i]->number, this_team->name);
    }

    /*
    sem_post(&teams_ready);
    //Wait for race to start
    sem_wait(&race_begin);
    
    

    for(int i = 0;i < this_team->number_team_cars;i++){
        car * c = shmat(team_cars[i], NULL,0);
        pthread_create(&cars[i],NULL,car_init,c);
    }

    //print(concat("READY: CARS OF TEAM ", this_team->name));

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //Wait for car threads to finish
    }
    //print(concat("Dead team: ", this_team->name));*/
    free(cars);
    exit(0);
}


car **find_team_cars(){
    car ** team_cars = malloc(sizeof(car) * this_team->number_team_cars);
    team_cars[0] = (car *)this_team + (config_struct->number_of_teams - this_team->team_number) + this_team->team_number * config_struct->max_cars_team;
    
    for (int i = 1; i < this_team->number_team_cars; i++){
        team_cars[i] = (car *) team_cars[i-1] + 1;
    }
    return team_cars;
}

void *car_init(void * arg){
    car *c = (car *)(arg);
    char buffer [100];
    snprintf (buffer, 100, "Team %s :Car number %d is Raccing",this_team->name, c->number);
    print(buffer);
    //char * temp;
    //temp = itoa(c->number, temp, 10);
    sleep(rand()%5);
    snprintf (buffer, 100, "Team %s :Car number %d reached finish line ",this_team->name, c->number);
    print(buffer);
    pthread_exit(NULL);
}
