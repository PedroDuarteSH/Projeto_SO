#include "functions.h"
#include "shared_mem.h"
#include <time.h>

//Generates and attach to this process the shared memory struture
void gen_shared_memory(){
  //Generate global structure shared memory
  if ((shm_id = shmget(IPC_PRIVATE, sizeof(shr_memory), IPC_CREAT | 0777)) < 1){
    perror("Error in shmget with IPC_CREAT\n");
    exit(1);
  }
  shm_struct = shmat(shm_id, NULL, 0);

  //Generate config structure shared memory updating the shared memory struct
  if ((shm_struct->config_shmid = shmget(IPC_PRIVATE, sizeof(config), IPC_CREAT | 0777)) < 1){
    perror("Error in shmget with IPC_CREAT\n");
    exit(1);
  }
  config_struct = shmat(shm_struct->config_shmid, NULL, 0);

  if ((shm_struct->race_shmid = shmget(IPC_PRIVATE, sizeof(race), IPC_CREAT | 0777)) < 1){
    perror("Error in shmget with IPC_CREAT\n");
    exit(1);
  }
  race_struct = shmat(shm_struct->race_shmid, NULL, 0);


  //semaphore init
  sem_init(&race_struct->race_begin, 1, 0);
  sem_init(&race_struct->teams_ready, 1, 0);
}

//Config file gesture
int *read_config_file(){
    /* Config int[] format
    0 - T_units_second;
    1 - lap_distance;
    2 - lap_number;
    3 - number_of_teams (must be > 3);
    4 - max_cars_team;
    5 - T_breakdown_interval;
    6 - T_Box_min;
    7 - T_Box_Max;
    8 - Fuel_tank_capacity;
    */
    FILE *conf_file = fopen("config.txt", "r");

    if(conf_file == NULL)
        return NULL;
    
    
    int *configs = malloc(sizeof(int) * number_of_configs);
    int current_index = 0;

    char *line = malloc(sizeof(char) * line_lenght);
    char *temp_strtok;
    char *elem;
    while(fgets(line,line_lenght, conf_file) != NULL){
        strip(line);
        elem = strtok(line, ",");
        configs[current_index++] = strtol(elem, &temp_strtok, 10);
        if((elem = strtok(NULL, ",")) != NULL)
            configs[current_index++] = strtol(elem, &temp_strtok, 10);
    }
    if(configs[3] < 3)
        return NULL;
    return configs;
}

void process_config_file(int *configs){
  config_struct->T_units_second = configs[0];
  config_struct->lap_distance = configs[1];
  config_struct->lap_number = configs[2];
  config_struct->number_of_teams = configs[3];
  config_struct->max_cars_team = configs[4];
  config_struct->T_breakdown_interval = configs[5];
  config_struct->T_Box_min = configs[6];
  config_struct->T_Box_Max = configs[7];
  config_struct->Fuel_tank_capacity = configs[8];
}

void print_config_file(){
    printf("%d\n", config_struct->T_units_second);
    printf("%d\n", config_struct->lap_distance);
    printf("%d\n", config_struct->lap_number);
    printf("%d\n", config_struct->number_of_teams);
    printf("%d\n", config_struct->max_cars_team);
    printf("%d\n", config_struct->T_breakdown_interval);
    printf("%d\n", config_struct->T_Box_min);
    printf("%d\n", config_struct->T_Box_Max);
    printf("%d\n", config_struct->Fuel_tank_capacity);
}


//Log management
void init_log(){
  shm_struct->log_file = fopen("log.txt", "w");
  sem_init(&shm_struct->log_sem, 1, 1);
  global_init_log(shm_struct->log_file, shm_struct->log_sem);
}

FILE *log_file = NULL;
sem_t log_sem;
void global_init_log(FILE *input_log_file, sem_t input_log_sem){
    log_file =  input_log_file;
    log_sem = input_log_sem;
}

void print(char *result){
    char time_str[20];

    time_t timer = time(NULL);
    struct tm* tm_info = localtime(&timer);

    strftime(time_str, 20, "%H:%M:%S", tm_info);

    sem_wait(&log_sem);
    fprintf(log_file, "%s:%s\n",time_str,result);
    printf("%s:%s\n",time_str,result);
    fflush(log_file);
    fflush(stdout);
    sem_post(&log_sem);
}


//String auxiliar
void strip(char *phrase){
    phrase[strcspn(phrase, "\r")] = 0;
    phrase[strcspn(phrase, "\n")] = 0;
}

char * concat (char * s1, char * s2) {
	char * result = malloc(sizeof(char)*(strlen(s1)+strlen(s2)+1));
	strcpy(result,s1);
	strcat(result,s2);
	return result;
}

