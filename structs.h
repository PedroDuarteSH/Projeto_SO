#ifndef structs   /* Include guard */
#define structs

typedef struct shr_memory{
    int config_shmid; //Config struct
    int race_shmid; //Race struct
    int log_sem_shmid; //Log Posix semaphore to control log input
    int log_file_shmid; //Log File address to doesn't open file every time
}shr_memory;

typedef struct memory{
    config *config; //Config struct
    race *race_data; //Race struct
    sem_t log_sem; //Log Posix semaphore to control log input
    FILE *log_file; //Log File address to doesn't open file every time

}memory;


typedef struct race{
    int status; //Started, interruped, 



}race;

typedef struct team{
    int status;

}team;

#endif