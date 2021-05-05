//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#include "race_simulator.h"



int main(){
  //Ignora o sinal SIG_INT até fazer todas as inicializações
  signal(SIGINT, SIG_IGN);
  
  //Save Process Pid to clean it
  main_pid = getpid();
  //Make shm_id as -1 to know that is not created
  shm_id = -1;

  //read config file
  int *configs = NULL;
  configs = read_config_file();
  if (configs == NULL) printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
  process_config_file(configs);
  
  //generate the shared memory and control mechanisms
  init_program();

  free(configs);
  init_log();

  //Create Named Pipe
  create_named_pipe(PIPENAME);


  //Updates the config struct with file configs
  pid_t race_manager_process = fork();
  if(race_manager_process == 0){
    print("Starting race process manager...");
    signal(SIGINT, clear_resources);
    //RACE MANAGER PROCESS
    race_manager_init();
    exit(0);
  }
  pid_t malfunction_manager_process = fork();
  if(malfunction_manager_process == 0){
    signal(SIGINT, clear_resources);
    print("Created Malfuntion process");
    sem_wait(&race_struct->race_begin);
    print("Malfuntion process initiated");
    //MALFUNCTION PROCESS
    exit(0);
  }

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
  int shared_mem_size = sizeof(race) + sizeof(team) * config_struct->number_of_teams + sizeof(car) * config_struct->number_of_teams * config_struct->max_cars_team;
  if ((shm_id = shmget(IPC_PRIVATE, shared_mem_size, IPC_CREAT | 0777)) < 1){
    perror("Error in shmget with IPC_CREAT\n");
    exit(1);
  }

  if((race_struct = (race *) shmat(shm_id, NULL, 0)) == (race *)-1){
      print("Error attaching shared memory in race_manager process");
      exit(0);
  }

  race_struct->status = NOT_STARTED;
  team *temp_team = (team *)(race_struct + 1);
  for (int i = 0; i < config_struct->number_of_teams; i++){
    temp_team->initiated = EMPTY;
    team *temp_team = (team *)(temp_team + 1);
  }
  car *current_car = (car *)temp_team;
  for (int i = 0; i < config_struct->number_of_teams * config_struct->max_cars_team; i++){
    current_car->comunication_pipe[0] = EMPTY;
    current_car->comunication_pipe[1] = EMPTY;
    car *current_car = (car *)(current_car + 1);
  }

  //Race Semaphores Init
  sem_init(&race_struct->race_begin, 1, 0);
  sem_init(&race_struct->teams_ready, 1, 0);
}

//Log management
void init_log(){
  log_file = fopen("log.txt", "w");
  log_semaphore = sem_open(LOG_SEM_NAME, O_CREAT | O_EXCL, 0777, 1);
}

//Create Named pipe
void create_named_pipe(char *name){
  if ((mkfifo(name, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)){
    print("CANNOT CREATE NAMED PIPE -> EXITING\n");
    exit(0);
  }
}

void print_statistics(){


}

