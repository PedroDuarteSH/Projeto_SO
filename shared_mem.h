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
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#define DEBUG
#define READ_BUFF 512
#define PIPENAME "CARS"


#define EMPTY -1
#define CREATED 1

//Race management
#define NOT_STARTED 0
#define STARTED 1
#define INTERRUPTED 2
#define TERMINATED 3

//Box status
#define FREE 0
#define BUSY 1
#define RESERVED 2

//car status
#define GAVE_UP 0
#define NOTSTARTED 1
#define BOX 2
#define SECURITY 3
#define RACE 4
#define FINISHED 5
#define MALFUNTION 6

//variable defines
#define CAR_COMMAND_SIZE 11
#define TRUE 1
#define FALSE 0


//String siz management
#define INPUT_LENGHT 200
#define STR_LENGHT 30




typedef struct config_struct{
    int  T_units_second;
    int lap_distance;
    int lap_number;
    int number_of_teams;
    int max_cars_team;
    int T_breakdown_interval;
    int T_Box_min;
    int T_Box_Max;
    int Fuel_tank_capacity;
}config_struct;

typedef struct race_struct{
    int status; //Started, ended, interruped
    int number_of_cars;
    int finished_cars;
    sem_t race_begin;
    sem_t cars_ready;
    sem_t change_status;
}race_struct;

typedef struct team_stuct{
    int team_number;
    char name[STR_LENGHT]; 
    int box_status;
    int number_team_cars;
    int comunication_pipe[2];
    sem_t write_pipe;
}team_stuct;

typedef struct car_struct{
    int ID;
    int number;
    int team_number;
    int state;
    float consumption;
    int speed;
    int reliability;
    float current_fuel;
    float distance;
    int completed_laps;
    
    int box_stops;
    int malfuntions_n;
    int finish_place;
}car_struct;


// structure for message queue
typedef struct malfunction{
    long carNumber;
} malfunction;

//Vars
//Main PID
pid_t main_pid;

//Shared memory id
int shm_id;

//Message Queue ID
int msq_id;

//Struct to save config files
config_struct *config;


//Log file management
FILE *log_file;
sem_t *log_semaphore;


//Shared memory locations
race_struct *race;
team_stuct *first_team;
car_struct *first_car;

//Processes PIDS
pid_t malfunction_manager_process;
pid_t race_manager_process;


#endif
