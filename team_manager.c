//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"


void change_state(car_struct *car, int state);
team_stuct *this_team;
pthread_t *cars;
int *box_requests;
pthread_mutex_t access_box = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t check_box = PTHREAD_MUTEX_INITIALIZER;

void team_manager_start(team_stuct *self){
    this_team = self;

#ifdef DEBUG
    print(concat("INITIATING TEAM CARS ARRAY ", this_team->name));
#endif

    car_struct **team_cars = find_team_cars();
    cars = (pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_create(&cars[i],NULL,car_init,team_cars[i]);
    }

    while(TRUE){


    }


    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //Wait for car threads to finish
    }

#ifdef DEBUG
    print(concat("Dead team: ", this_team->name));
#endif


    free(cars);
    free(team_cars);


    exit(0);
}


car_struct **find_team_cars(){
    car_struct ** team_cars = malloc(sizeof(car_struct) * this_team->number_team_cars);
    car_struct *temp_car = (car_struct *) (first_car);
    temp_car = temp_car + config->max_cars_team * this_team->team_number;
    for (int i = 0; i < this_team->number_team_cars; i++){
        team_cars[i] = (car_struct *) (temp_car);
        temp_car = (car_struct *) (temp_car+1);
    }
    return team_cars;
}

void *car_init(void * arg){
    car_struct *car = (car_struct *)(arg);
    char buffer [100];
#ifdef DEBUG
    snprintf (buffer, 100, "Team %s :Car number %d is WAITING FOR RACE",this_team->name, car->number);
    print(buffer);
#endif

    //Calculate needed fuel to 4 laps
    int distance = config->lap_distance * 4;
    float cons_4_laps = distance/car->speed * car->consumption;

    //Car ready to start
    sem_post(&race->cars_ready);
    
    //Wait for race to start
    sem_wait(&race->race_begin);


    change_state(car, RACE);
    
    while(TRUE){
        if(car->current_fuel < cons_4_laps/2){
            if(car->current_fuel < 0)break;
            change_state(car, SECURITY);
        }

        //Passing box
        if(car->distance >= config->lap_distance){
            if(car->completed_laps == config->lap_number)
                break;
            pthread_mutex_lock(&check_box);
            if(car->current_fuel < cons_4_laps && this_team->box_status == FREE){
                this_team->box_status = BUSY;
                pthread_mutex_unlock(&check_box);
                //Entrar na box
                
            }
            car->distance-=config->lap_distance;
            car->completed_laps++;
        }

        car->distance += car->speed;
        car->current_fuel -= car->consumption;

        if(msgrcv(msq_id,NULL, sizeof(malfunction) - sizeof(long), car->team_number * car->number,IPC_NOWAIT) == 0)
            change_state(car, SECURITY);

        sleep(1);
    }
    if(car->current_fuel < 0)
        change_state(car, GAVE_UP);
    else
        change_state(car, FINISHED);
    pthread_exit(NULL);
}


void change_state(car_struct *car, int state){
    car->state = state;
    if(state == SECURITY){
        pthread_mutex_lock(&check_box);
    }
   
    write(this_team->comunication_pipe[1], car, sizeof(car_struct *));
}
