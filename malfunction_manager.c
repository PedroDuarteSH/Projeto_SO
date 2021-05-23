#include "malfunction_manager.h"


int volatile break_loop;
void b_loop(int signum){
    break_loop++;

}

//Processo  responsável  por  gerar aleatoriamente as avarias dos carros, a partir da informação da sua fiabilidade
//Produz avarias randomicamente Comunica por Message queque
void malfunction_manager_init(){
    signal(SIGINT, clear_resources);
    signal(SIGUSR1, b_loop);
    malfunction send_msg;
    int probability = 0;
    while(TRUE){
        print("MALFUNTION WAITNG RACE TO START!");
        sem_wait(&race->race_begin);
        break_loop = 0;
        while(1){
            usleep((config->T_breakdown_interval) * 1000000 / config->T_units_second);
            if(break_loop > 0)
                break;
            car_struct * temp_car = first_car;
            for (int i = 0; i < config->number_of_teams * config->max_cars_team; i++){
                sem_wait(&temp_car->car_check);
                if(temp_car->number != EMPTY && temp_car->state != GAVE_UP && temp_car->state != FINISHED){
                    sem_post(&temp_car->car_check);
                    probability = rand() % 100 + 1;
                    if(temp_car->reliability < probability){
                        send_msg.carNumber = temp_car->ID;
                        msgsnd(msq_id,&send_msg,sizeof(malfunction)-sizeof(long),0);
                    }
                }else sem_post(&temp_car->car_check);
                temp_car = (car_struct *)(temp_car + 1);
            }
        }
    }
    
}