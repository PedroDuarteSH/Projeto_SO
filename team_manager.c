//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Processo responsável pela gestão da box e dos carros da equipa. É ainda responsável pela reparação dos carros da equipa e por atestar o depósito de combustível. Existe um por equipa.
//Fazer a Gestão da Box (APENAS 1) e Atualiza-a (Livre, Ocupada, reservada)
//Fazer a Gestão dos Carros da equipa
//Responsavel por reparações e atestar depósitos
#include "team_manager.h"

void Car_access_Box(car_struct *this_car);
void change_state(car_struct *car, int state);
team_stuct *this_team;
pthread_t *cars;
//int *box_requests;

pthread_mutex_t check_box = PTHREAD_MUTEX_INITIALIZER;


void team_manager_start(team_stuct *self){
    this_team = self;
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, SIG_IGN);
    
    close_otherPipes();
    
#ifdef DEBUG
    print(concat("INITIATING TEAM CARS ARRAY ", this_team->name));
#endif

    car_struct **team_cars = find_team_cars();
    cars = (pthread_t *) malloc(sizeof(pthread_t) * this_team->number_team_cars);

    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_create(&cars[i],NULL,car_init,team_cars[i]);
    }


    for(int i = 0;i < this_team->number_team_cars;i++){
        pthread_join(cars[i],NULL); //Wait for car threads to finish
    }

#ifdef DEBUG
    print(concat("Dead team: ", this_team->name));
#endif


    free(cars);
    free(team_cars);
    pthread_mutex_destroy(&check_box);  
    exit(0);
}

void close_otherPipes(){
    team_stuct *temp_team = first_team;
    for (int i = 0; i < config->number_of_teams; i++){
        if(temp_team != this_team){
            close(temp_team->comunication_pipe[0]);
            close(temp_team->comunication_pipe[1]);
        }
        else{
            close(temp_team->comunication_pipe[0]);
        }
        temp_team = temp_team + 1;
    }
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
    struct timeval last;
    struct timeval current;
    float elapsed;
    car_struct *car = (car_struct *)(arg);
    
    car->state = NOTSTARTED;
    car->box_stops = 0;
    car->malfuntions_n = 0;
    car->current_fuel = (float) config->Fuel_tank_capacity;
    car->finish_place = 0;
    car->completed_laps = 0;

#ifdef DEBUG
    char buffer [100];
    snprintf (buffer, 100, "Team %s :Car number %d is WAITING FOR RACE",this_team->name, car->number);
    print(buffer);
#endif

    //Calculate needed fuel to 4 laps
    int distance_4_laps = config->lap_distance * 4;
    float cons_4_laps = distance_4_laps/car->speed * car->consumption;
    //Car ready to start
    sem_post(&race->cars_ready);
    
    //Wait for race to start
    sem_wait(&race->race_begin);

    gettimeofday(&last, 0);
    
    change_state(car, RACE);
    malfunction mal;
    while(TRUE){
        if(car->current_fuel < 0)break;
       
        if(car->current_fuel < cons_4_laps/2 && car->state == RACE)
            change_state(car, SECURITY);
        
        //Passing box
        if(car->distance >= (float) config->lap_distance){
            car->distance-=config->lap_distance;
            car->completed_laps++;
            sem_wait(&race->change_status);
            if(race->status == INTERRUPTED){
                sem_post(&race->change_status);
                car->distance = 0;
                change_state(car, GAVE_UP);
                pthread_exit(NULL);
            }else sem_post(&race->change_status);
                
            if(car->completed_laps == config->lap_number){
                car->distance = 0;
                break;
            }
            
                
            pthread_mutex_lock(&check_box);
            if((car->state == SECURITY || car->state != MALFUNTION) && this_team->box_status != BUSY){
                Car_access_Box(car);
                gettimeofday(&last, 0);
            }
            else if(car->current_fuel < cons_4_laps && this_team->box_status == FREE){
                Car_access_Box(car);
                gettimeofday(&last, 0); 
            }
                
            else pthread_mutex_unlock(&check_box);
        }

        if(msgrcv(msq_id, &mal, sizeof(malfunction) - sizeof(long), car->ID,IPC_NOWAIT) == 0)
            if(car->state != MALFUNTION){
                change_state(car, MALFUNTION);
                sem_wait(&car->car_check);
                car->malfuntions_n++;
                sem_post(&car->car_check);
            }
        gettimeofday(&current, 0);
        elapsed = timedifference_msec(last, current) / 100 * config->T_units_second; 
        last = current;
        if(car->state == SECURITY || car->state == MALFUNTION){
            sem_wait(&car->car_check);
            car->distance += car->speed * 0.3 * elapsed;
            car->current_fuel -= car->consumption * 0.4 * elapsed;
            sem_post(&car->car_check);
        }
        else{
            sem_wait(&car->car_check);
            car->distance += car->speed * elapsed;
            car->current_fuel -= car->consumption * elapsed;
            sem_post(&car->car_check);
        }
        //0.1s sleep to not overload cpu
        usleep(100000);
    }
    if(car->current_fuel < 0)
        change_state(car, GAVE_UP);
    else
        change_state(car, FINISHED);
    pthread_exit(NULL);
}


void change_state(car_struct *car, int state){
    sem_wait(&car->car_check);
    car->state = state;
    if(car->state == SECURITY || car->state == MALFUNTION){
        pthread_mutex_lock(&check_box);
        if(this_team->box_status == FREE)
            this_team->box_status = RESERVED;
        pthread_mutex_unlock(&check_box);
    } 
    sem_post(&car->car_check);  
    sem_wait(&this_team->write_pipe);
    write(this_team->comunication_pipe[1], &car, sizeof(car_struct *));
}

void Car_access_Box(car_struct *this_car){
    this_team->box_status = BUSY;
    pthread_mutex_unlock(&check_box);
    change_state(this_car, BOX);

    //Re-Fuel Car
    this_car->box_stops++;
    this_car->current_fuel = config->Fuel_tank_capacity;
    usleep(2 * 1000000 /config->T_units_second);
    //Repair car if needed
    if(this_car->state == MALFUNTION){
        float repair_time = rand() % (config->T_Box_Max - config->T_Box_min) + config->T_Box_min;
        usleep(repair_time * 1000000 /config->T_units_second);
    }
    change_state(this_car, RACE);
    pthread_mutex_lock(&check_box);
    this_team->box_status = FREE;
    pthread_mutex_unlock(&check_box);
}


