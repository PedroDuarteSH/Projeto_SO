#include "race_simulator.h"

//Main file
//Processo  responsável  por  iniciar  o sistema e os restantes processos do simulador.
/* TO DO
● Lê	configurações	do	Ficheiro	de	Configuração (ver	exemplo	fornecido)
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

int main() {
    int *configs = read_config_file();
    if(configs == NULL)
        printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
    
}

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


void strip(char *phrase){
    phrase[strcspn(phrase, "\r")] = 0;
    phrase[strcspn(phrase, "\n")] = 0;
}
