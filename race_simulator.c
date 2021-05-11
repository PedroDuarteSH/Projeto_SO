//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#include "race_simulator.h"

int main(){
  //Ignora o sinal SIG_INT até fazer todas as inicializações
  signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);

  //Save Process Pid to clean it
  main_pid = getpid();
  //Make IDS to know that is not created
  shm_id = -1;
  msq_id = -1;

  //read config file
  int *configs = NULL;
  configs = read_config_file();
  if (configs == NULL) printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
  process_config_file(configs);
  free(configs);
  
  init_log();
 
  //generate the shared memory and control mechanisms
  init_program();

  
  
 
  //Create Named Pipe
  create_named_pipe(PIPENAME);
  //Create MSQ
  create_msq();

  malfunction_manager_process = fork();
  if(malfunction_manager_process == 0) malfunction_manager_init();
  
  race_manager_process = fork();
  if(race_manager_process == 0) race_manager_init();
  
  
  
  //Gestão de sinais
  signal(SIGINT, clear_resources);
  signal(SIGTSTP, print_statistics);
  while(1){
    pause();
  }
}

//Generates and attach the shared memory struture
void init_program(){
  //Generate global structure shared memory
  int shared_mem_size = sizeof(race) + (sizeof(team_stuct) * config->number_of_teams) + (sizeof(car_struct) * config->number_of_teams * config->max_cars_team);
  if ((shm_id = shmget(IPC_PRIVATE, shared_mem_size, IPC_CREAT | IPC_EXCL | 0700)) < 1){
    print("Error in shmget with IPC_CREAT\n");
    exit(1);
  }

  if((race = (race_struct *) shmat(shm_id, NULL, 0)) == (race_struct *)-1){
      print("Error attaching shared memory in race_manager process");
      exit(0);
  }

  //Define first of each type for easy consulting
  first_team = (team_stuct *)(race + 1);
  first_car = (car_struct *)(first_team + config->number_of_teams);
  
  clean_data();
  //Race Semaphores Init
  sem_init(&race->race_begin, 1, 0);
  sem_init(&race->cars_ready, 1, 0);
}

void clean_data(){
  race->status = NOT_STARTED;
  race->number_of_cars = 0;
  race->finished_cars = 0;
  team_stuct *temp_team = first_team;
  for (int i = 0; i < config->number_of_teams; i++){
    temp_team->team_number = EMPTY;
    temp_team = (team_stuct *)(temp_team + 1);
  }
  car_struct * temp_car = first_car;
  for (int i = 0; i < config->number_of_teams * config->max_cars_team; i++){
    temp_car->number = EMPTY;
    temp_car->ID = i+1;
    temp_car = (car_struct *)(temp_car + 1);
  }
}

//Create Named pipe
void create_named_pipe(char *name){
  unlink(name);
  if ((mkfifo(name, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)){
    print("CANNOT CREATE NAMED PIPE -> EXITING\n");
    remove_shm();
    exit(0);
  }
}


void create_msq(){
  //Create message queue
  if((msq_id = msgget(IPC_PRIVATE, IPC_CREAT|0777)) == -1){
    print("Error creating message queue");
    exit(0);
  }
}


//Log management
void init_log(){
  //Delete semaphore if is open
  sem_unlink(LOG_SEM_NAME);
  log_file = fopen("log.txt", "w");
  log_semaphore = sem_open(LOG_SEM_NAME, O_CREAT | O_EXCL, 0777, 1);
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
  car_struct *temp;
  car_struct *car_worst = first_car;
  car_struct **best_5_cars = malloc(sizeof(car_struct * ) * 5);
  int used;
  for (int i = 0; i < 5; i++){
    best_5_cars[i] = NULL;
  }
  
  for (int i = 0; i < 6; i++){
    for (; i < race->finished_cars && i < 5; i++){
      temp = first_car;
      for (int j = 0; j < config->number_of_teams * config->max_cars_team; j++){
        if(temp->finish_place == (i+1))
          best_5_cars[i] = temp;
        temp = temp + 1;
      }
      
    }
    temp = first_car;
    for (int j = 0; j < config->number_of_teams * config->max_cars_team; j++){
      used = 0;
      if(temp->number != EMPTY){
        if(i == 0){
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
  team_stuct *temp_team;
  output = concat(output, "\tFirst placed cars:\n");
  for (int i = 0; i < 5; i++){
    temp_team = first_team + best_5_cars[i]->team_number; 
    snprintf(line, READ_BUFF, "\t\t%dª Position: [TEAM %s] Car %d of team %d -> Elapsed Laps: %d | Elapsed Lap Distance: %f | Box accesses: %d \n", (i+1), temp_team->name, best_5_cars[i]->number, (temp_team->team_number+1), best_5_cars[i]->completed_laps, best_5_cars[i]->distance, best_5_cars[i]->box_stops);
    output = concat(output, line);
  }
  output = concat(output, "\tLast placed car:\n");
  temp_team = first_team + car_worst->team_number; 
  snprintf(line, READ_BUFF, "\t\t%dª Position: [TEAM %s] Car %d of team %d -> Elapsed Laps: %d | Elapsed Lap Distance: %f |Box accesses: %d \n", race->number_of_cars, temp_team->name, car_worst->number, (temp_team->team_number+1), car_worst->completed_laps, car_worst->distance, car_worst->box_stops);
  output = concat(output, line);
  
  int racing_cars = race->number_of_cars-race->finished_cars;
  output = concat(output, "\tOther Data:\n");
  snprintf(line, READ_BUFF, "\t\tStops at the boxes to refuel: %d\n\t\tMalfunctions total: %d\n\t\tRacing Cars: %d",t_box_stops, t_malfunc_num, racing_cars);

  output = concat(output, line);
  
  print(output);
  free(best_5_cars);
}
