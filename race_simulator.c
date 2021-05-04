//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

#include "race_simulator.h"



int main(){
  //Gestão de sinais
  signal(SIGINT, clear_resources);
  //signal(SIGTSTP, print_statistics);
  //Save Process Pid to clean it
  start_pid = getpid();
  //Make shm_id as -1 to know that is not created
  shm_id = -1;


  //read config file
  int *configs = NULL;
  configs = read_config_file();
  if (configs == NULL) printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
  process_config_file(configs);
  
  //generate the shared memory and control mechanisms
  init_program(configs);
  init_log();


  print("Initiated_program!\n");
  free(configs);

  //Updates the config struct with file configs
  pid_t race_manager_process = fork();
  if(race_manager_process == 0){
    print("Starting race process manager...");
    //RACE MANAGER PROCESS
    race_manager_init();
    exit(0);
  }
  pid_t malfunction_manager_process = fork();
  if(malfunction_manager_process == 0){
    print("Created Malfuntion process");
    sem_wait(&race_begin);
    print("Malfuntion process initiated");
    //MALFUNCTION PROCESS
    exit(0);
  }

  //Create Named Pipe
  create_named_pipe(PIPENAME);


  print("Finished\n");
  
  clean();

  print("Limpo\n");
}

void create_named_pipe(char *name){
  if ((mkfifo(PIPENAME, O_CREAT|O_EXCL|0600)<0) && (errno != EEXIST)){
    perror("Cannot create named pipe: ");
    exit(0);
  }
}