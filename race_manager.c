//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager

void race_manager(){
    pid_t new_team;
    //Incia Carros
    for(int i = 0;i < 2;i++){ //mudar o 2
        new_team = fork();
        if(new_team == 0){
          team_manager();
       }
    }

    //Espera que o setup das equipas esteja feito
    //Começa a Corrida
}