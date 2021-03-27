#include <stdio.h>
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

