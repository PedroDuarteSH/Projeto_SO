//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"


team_stuct *this_team;
pthread_t *cars;


void team_manager_start(team_stuct *self){
    this_team = self;

#ifdef DEBUG
    print(concat("INITIATING TEAM CARS ARRAY ", this_team->name));
#endif

    sem_post(&race->teams_ready);


    
    car_struct **team_cars = find_team_cars();
    cars = (pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);

    
    //Wait for race to start
    sem_wait(&race->race_begin);
    
    

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_create(&cars[i],NULL,car_init,team_cars[i]);
    }

    print(concat("READY: CARS OF TEAM ", this_team->name));

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //Wait for car threads to finish
    }
    print(concat("Dead team: ", this_team->name));
    free(cars);
    exit(0);
}


car_struct **find_team_cars(){
    car_struct ** team_cars = malloc(sizeof(car_struct) * this_team->number_team_cars);
    car_struct *temp_car = (car_struct *) (first_car);
    temp_car = temp_car + config->max_cars_team * this_team->team_number;
    for (int i = 0; i < this_team->number_team_cars; i++){
        printf("CAR %d - From team %s\n", temp_car->number, this_team->name);
        team_cars[i] = (car_struct *) (temp_car);
        temp_car = (car_struct *) (temp_car+1);
        
    }
    return team_cars;
}

void *car_init(void * arg){
    car_struct *c = (car_struct *)(arg);
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
