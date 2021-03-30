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

int main(){
  //read config file
  int *configs = NULL;
  configs = read_config_file();
  if (configs == NULL)
    printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
  
  //generate the shared memory
  gen_shared_memory();
  init_log();
  print("Initiated_program!");

  process_config_file(configs);
  //Updates the config struct with file configs
  sem_init(&race_struct->race_begin, 1, 0);
  race_manager_process = fork();

  if(race_manager_process == 0){
    print("Starting race process manager...");
    //RACE MANAGER PROCESS
    race_manager_init(shm_id);
    exit(0);
  }

  malfunction_manager_process = fork();
  if(malfunction_manager_process == 0){
        print("Created Malfuntion process");
        sem_wait(&race_struct->race_begin);
        print("Malfuntion process initiated");
        //MALFUNCTION PROCESS
        exit(0);
  }  

  wait(NULL);
  wait(NULL);
  print("Finished");
}
