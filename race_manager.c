//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager

void race_manager(int shm_id){

    //Espera que o setup das equipas esteja feito
    //Começa a Corrida
    pid_t new_team;
    //Incia Carros
    for(int i = 0;i < 2;i++){ //mudar o 2
        new_team = fork();
        if(new_team == 0){
          team_manager();
       }
    }
}




void attach_update_shm(){
    shm_struct = shmat(shm_id, NULL, 0);
    config_struct = shmat(shm_struct->config_shmid, NULL, 0);
    
    //Attact race struct
    //Ininitiates

}