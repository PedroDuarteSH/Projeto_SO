#ifndef shared_mem   /* Include guard */
#define shared_mem

#define debug

#define MAX_SIZE 30

//imports 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>



//structs
typedef struct shr_memory{
    int config_shmid; //Config struct
    int race_shmid; //Race struct
    sem_t log_sem; //Log Posix semaphore to control log input
    FILE *log_file; //Log File address to doesn't open file every time
}shr_memory;

typedef struct config{
    int  T_units_second;
    int lap_distance;
    int lap_number;
    int number_of_teams;
    int max_cars_team;
    int T_breakdown_interval;
    int T_Box_min;
    int T_Box_Max;
    int Fuel_tank_capacity;
}config;

typedef struct race{
    int status; //Started, ended, interruped, 
    int teams_shmid; //Array in shared memory with team structs address
}race;

typedef struct team{
    int name[MAX_SIZE]; 
    int box_status;
    const int number_cars;
    int cars_shmid;    //Array in shared memory with team structs address
}team;

typedef struct car{
    int state;
    int consumption;
    int speed;
    int done_trips;
    int current_fuel;
}car;

//Public declared functions

#endif