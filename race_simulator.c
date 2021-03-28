#include "race_simulator.h"
#include "functions.c"


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

int main() {
    int *configs = read_config_file();
    if(configs == NULL)
        printf("Error reading file or invalid number of teams\ncheck if your file is config.txt or the number of teams (line 3) is bigger than 3!");
}
