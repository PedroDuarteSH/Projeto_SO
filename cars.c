//Threads carros que podem estar em 5 estados(Corrida,Segurança, Box, Desistência, Terminado)
//Recebem mensagens do Gestor de Avarias,sobre avarias
void *car_init(void * shm_ids);
#include "shared_mem.h"

void *car_init(void * shm_ids){
    pthread_exit(NULL);
}