//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#ifndef shared_mem   /* Include guard */
#define shared_mem

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
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


#define DEBUG


#define PIPENAME "CARS"

#define MAX_SIZE 30

#define EMPTY 0
#define CREATED 1

//Race management
#define NOT_STARTED 0
#define STARTED 1
#define INTERRUPTED 2


//Box status
#define FREE 0
#define BUSY 1
#define RESERVED 2

//variable defines
#define CAR_INPUT_SIZE 11
#define TRUE 1
#define FALSE 0

//Car management
#define INVALID_COMMAND 0
#define CANT_ADD_TEAM 1
#define CAR_ADDED 2

//String siz management
#define INPUT_LENGHT 200
#define SMALL_STR_LENGHT 10




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
    int status; //Started, ended, interruped
    sem_t race_begin;
    sem_t teams_ready;
}race;

typedef struct team{
    int initiated;
    int team_number;
    char name[MAX_SIZE]; 
    int box_status;
    int number_team_cars;
    sem_t modify_team;
}team;

typedef struct car{
    int number;
    int team_number;
    int state;
    float consumption;
    int speed;
    int reliability;
    int current_fuel;
    int comunication_pipe[2];
}car;

//Vars
//Main PID
pid_t main_pid;

//Shared memory id
int shm_id;


config *config_struct;





//Log file management
FILE *log_file;
sem_t *log_semaphore;


race *race_struct;

#endif
