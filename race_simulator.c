#include "race_simulator.h"
//Main file
//Processo  responsável  por  iniciar  o sistema e os restantes processos do simulador.

/* TO DO
● Cria	Named	Pipe de	comunicação	com	o	Gestor	de	Corrida
● Inicia	o	processo	Gestor	de	Corrida
● Inicia	o	processo	Gestor	de	Avarias
● Captura	o	sinal	SIGTSTP e	imprime	estatísticas
● Captura	o	sinal	SIGINT	para	terminar a	corrida	e	o	programa.	Ao	receber	este	sinal,	
deve	 aguardar	 que	 todos	 os	 carros	 cruzem	 a	meta	 (mesmo	 que	 não	 seja	 a	 sua	
última	volta)	- os	carros	que	se	encontram	na	box	no	momento	da	instrução	devem	
terminar.	 Após	 todos	 os	 carros	 concluírem	 a	 corrida,	 deverá	 imprimir	 as	
estatísticas	do	jogo	e	terminar/libertar/remover	todos	os	recursos	utilizados.*/

/* DONE
● Lê	configurações	do	Ficheiro	de	Configuração (ver	exemplo	fornecido)
*/

int shm_id;
shr_memory *shm_struct;
config *config_struct;
race *race_struct;
int main(){
  //read config file
  int *configs = NULL;
  configs = read_config_file();
  if (configs == NULL)
    printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");

  process_config_file(configs);
  //generate the shared memory
  gen_shared_memory();

  //Updates the config struct with file configs
  sem_init(&race_struct->race_begin, 1, 0);
  race_manager_process = fork();
  if (race_manager_process == 0){
    //RACE MANAGER PROCESS
    race_manager_init(shm_id);
    exit(0);
  }
  /*
    malfunction_manager_process = fork();
    if(malfunction_manager_process == 0){
        //MALFUNCTION PROCESS
        malfunction_manager();
        exit(0);
    }*/

  
  
  
  
  wait(NULL);
  printf("Finished");
}

//Generates and attach to this process the shared memory struture
void gen_shared_memory()
{
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
