//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#include "functions.h"
#include "shared_mem.h"
#include <time.h>

void clear_resources(int signum){
  //Destroy unnamed semaphores
  if(sem_destroy(&race->cars_ready) == -1){
    #ifdef DEBUG
    print(concat("Error destroying cars_begin semaphore: ", strerror(errno)));
    #endif
  }
 
  if(sem_destroy(&race->race_begin) == -1){
    #ifdef DEBUG
    print(concat("Error destroying race_begin semaphore: ", strerror(errno)));
    #endif  
  }

  //Detach shared memory from processes
  shmdt(race);
  
  //Destroy named semaphores
  if(getpid() == main_pid){
    //Esperar pelos processos filho (Corrida e Malfunction)
    print("SIMULATOR CLOSING");
    waitpid(race_manager_process, NULL, 0); 
    waitpid(malfunction_manager_process, NULL, 0);
    //Eliminar memória partilhada
    if(shm_id >= 0)
      remove_shm();
    if(msq_id >= 0)
      remove_msq();
    
    //Destroy named pipe and semaphore
    unlink(PIPENAME);
    sem_unlink(LOG_SEM_NAME);
  }
  sem_close(log_semaphore);
  fclose(log_file);
  exit(0);
}
void remove_shm(){
  if(shmctl(shm_id, IPC_RMID, NULL) == -1){
#ifdef DEBUG
    print(concat ("Error removing shared memory: ", strerror(errno)));
#endif
  }
}

void remove_msq(){
  if(msgctl(msq_id, IPC_RMID, NULL) == -1){
#ifdef DEBUG
    print(concat ("Error removing message queue: ", strerror(errno)));
#endif
  }
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
    free(line);
    fclose(conf_file);
    return configs;
}

void process_config_file(int *configs){
  config = malloc(sizeof(config_struct));
  config->T_units_second = configs[0];
  config->lap_distance = configs[1];
  config->lap_number = configs[2];
  config->number_of_teams = configs[3];
  config->max_cars_team = configs[4];
  config->T_breakdown_interval = configs[5];
  config->T_Box_min = configs[6];
  config->T_Box_Max = configs[7];
  config->Fuel_tank_capacity = configs[8];
}

//Debug
void print_config_file(){
    printf("%d\n", config->T_units_second);
    printf("%d\n", config->lap_distance);
    printf("%d\n", config->lap_number);
    printf("%d\n", config->number_of_teams);
    printf("%d\n", config->max_cars_team);
    printf("%d\n", config->T_breakdown_interval);
    printf("%d\n", config->T_Box_min);
    printf("%d\n", config->T_Box_Max);
    printf("%d\n", config->Fuel_tank_capacity);
}



void print(char *result){
    char time_str[20];

    time_t timer = time(NULL);
    struct tm* tm_info = localtime(&timer);

    strftime(time_str, 20, "%H:%M:%S", tm_info);

    sem_wait(log_semaphore);
    fprintf(log_file, "%s:%s\n",time_str,result);
    printf("%s:%s\n",time_str,result);
    fflush(log_file);
    fflush(stdout);
    sem_post(log_semaphore);
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

