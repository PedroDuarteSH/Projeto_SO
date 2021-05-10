#include "malfunction_manager.h"

//Processo  responsável  por  gerar aleatoriamente as avarias dos carros, a partir da informação da sua fiabilidade
//Produz avarias randomicamente Comunica por Message queque
void malfunction_manager_init(){
    malfunction send_msg;
    int probability = 0;
    while(1){
        usleep((config->T_breakdown_interval) * 1000000 / config->T_units_second);
        car_struct * temp_car = first_car;
        for (int i = 0; i < config->number_of_teams * config->max_cars_team; i++){
            if(temp_car->number != EMPTY && temp_car->state != GAVE_UP && temp_car->state != FINISHED){
                probability = rand() % 100 +1;
                if(temp_car->reliability < probability){
                    send_msg.carNumber = temp_car->ID;
                    msgsnd(msq_id,&send_msg,sizeof(malfunction)-sizeof(long),0);
                }
            }
            temp_car = (car_struct *)(temp_car + 1);
        }
    }
}