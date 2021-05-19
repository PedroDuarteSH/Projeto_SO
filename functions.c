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

  //Destroy named semaphores
  if(getpid() == main_pid){
    //Esperar pelos processos filho (Corrida e Malfunction)
    waitpid(race_manager_process, NULL, 0);
    waitpid(malfunction_manager_process, NULL, 0);
    print_statistics(SIGTSTP);
    
    print("SIMULATOR CLOSING");
    //Eliminar memória partilhada
    shmdt(race);
    if(shm_id >= 0)
      remove_shm();
    if(msq_id >= 0)
      remove_msq();
    
    //Destroy named pipe and semaphore
    unlink(PIPENAME);
    sem_unlink(LOG_SEM_NAME);
  }
  else shmdt(race);
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

void print_statistics(int signum){
  char *output;
  output = concat("", "\n\tStatistics!\n");
  if(race->status == NOT_STARTED){
    output = concat(output, "\tCars are not racing");
    print(output);
    free(output);
    return;
  }
  
  int t_malfunc_num = 0, t_box_stops = 0;
  
  car_struct *car_worst = first_car;
  car_struct **best_5_cars = malloc(sizeof(car_struct * ) * 5);
  int used;
  for (int i = 0; i < 5; i++)
    best_5_cars[i] = NULL;
  
  car_struct *temp;
  for (int i = 0; i < 6; i++){
    for (int k = 0; i < race->finished_cars && k < 5; k++){
      temp = first_car;
      for (int j = 0; j < config->number_of_teams * config->max_cars_team; j++){
        if(temp->finish_place == (i+1))
          best_5_cars[i++] = temp;
        temp = temp + 1;
      }
    }
    temp = first_car;
    for (int j = 0; j < config->number_of_teams * config->max_cars_team; j++){
      used = 0;
      if(temp->number != EMPTY){
        if(i == 5){
          t_malfunc_num += temp->malfuntions_n;
          t_box_stops += temp->box_stops;
        }
        for (int k = 0; k < i; k++)
          if(temp == best_5_cars[k]){
            used = 1;
          }
        if(used == 0){
          if(i<5){
            if(best_5_cars[i] == NULL || temp->completed_laps > best_5_cars[i]->completed_laps || (temp->completed_laps == best_5_cars[i]->completed_laps && temp->distance > best_5_cars[i]->distance)){
              best_5_cars[i] = temp;
            }
          }
          else
            if(temp->completed_laps < car_worst->completed_laps || (temp->completed_laps == car_worst->completed_laps && temp->distance < car_worst->distance))
              car_worst = temp;
        }
      }
      temp = temp + 1;
    }
  }
  
  char line[READ_BUFF];
  team_stuct *t;
  
  output = concat(output, "\tFirst placed cars:\n");

  for (int i = 0; i < 5; i++){
    t = first_team + best_5_cars[i]->team_number; 
    snprintf(line, READ_BUFF, "\t\t%dª Position: [TEAM %s] Car %d of team %d -> Elapsed Laps: %d | Elapsed Lap Distance: %f | Box accesses: %d \n", (i+1), t->name, best_5_cars[i]->number, (t->team_number+1), best_5_cars[i]->completed_laps, best_5_cars[i]->distance, best_5_cars[i]->box_stops);
    output = concat(output, line);
  }

  output = concat(output, "\tLast placed car:\n");
  t = first_team + car_worst->team_number; 
  snprintf(line, READ_BUFF, "\t\t%dª Position: [TEAM %s] Car %d of team %d -> Elapsed Laps: %d | Elapsed Lap Distance: %f |Box accesses: %d \n", race->number_of_cars, t->name, car_worst->number, (t->team_number+1), car_worst->completed_laps, car_worst->distance, car_worst->box_stops);
  output = concat(output, line);
  
  int racing_cars = race->number_of_cars-race->finished_cars;
  output = concat(output, "\tOther Data:\n");
  snprintf(line, READ_BUFF, "\t\tStops at the boxes to refuel: %d\n\t\tMalfunctions total: %d\n\t\tRacing Cars: %d",t_box_stops, t_malfunc_num, racing_cars);
  output = concat(output, line);
  print(output);
  free(best_5_cars);
}
