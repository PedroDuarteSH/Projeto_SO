#include "malfunction_manager.h"

//Processo  responsável  por  gerar aleatoriamente as avarias dos carros, a partir da informação da sua fiabilidade
//Produz avarias randomicamente Comunica por Message queque
void malfunction_manager_init(){
    sleep(config->T_breakdown_interval);
    malfunction  send_msg;
    int probability = 0;
    car_struct * temp_car = first_car;
    while(1){
        for (int i = 0; i < config->number_of_teams * config->max_cars_team; i++){
            temp_car = (car_struct *)(temp_car + 1);
            if(temp_car->number == EMPTY){
                continue;
            }
            else{
                probability = rand() %100 +1;
                if(temp_car->reliability < probability){
                    print("TEVE UMA AVARIA");
                    send_msg.carNumber = temp_car->number * temp_car->team_number;
                    msgsnd(msq_id,&send_msg,sizeof(malfunction)-sizeof(long),0);
                }
            }
        }
       sleep(config->T_breakdown_interval);
    }
}