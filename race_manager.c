//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager
#include "race_manager.h"

team *create_team(char *team_name, int i);

void race_manager_init(){
    attach_update_race_shm();

#ifdef debug
    print_config_file();
#endif

    //avisar o semaforo que a corrida
    //ler o ficheiro / Receber namedPipe
    FILE *cars_file = fopen("cars.temp", "r");

    char *line = malloc(sizeof(char) * INPUT_LENGHT);
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    int game_started = FALSE;
    while (TRUE){
        fgets(line, INPUT_LENGHT, cars_file);
        if (game_started == FALSE){
            game_started = process_command(line);
            sleep(0);
        }
        else{


            break;
        }
    }
    free(command);
    free(line);
    //espera todas as equipas terminarem
    for (int i = 0; i < config_struct->number_of_teams; i++){
            wait(NULL);
    }
    //Espera que o setup das equipas esteja feito
    //Começa a Corrida
    //pid_t new_team;
    //Incia Carros
    /*
    for(int i = 0;i < 2;i++){ //mudar o 2
        new_team = fork();
        if(new_team == 0){
          team_manager();
       }
    }*/
}

void attach_update_race_shm(){
    //first 3 lines wasn't needed (already attached in father process)
    shm_struct = shmat(shm_id, NULL, 0);
    config_struct = shmat(shm_struct->config_shmid, NULL, 0);
    race_struct = shmat(shm_struct->race_shmid, NULL, 0);
    //iniciar equipas
    if ((race_struct->teams_shmid = shmget(IPC_PRIVATE, sizeof(team *) * config_struct->number_of_teams, IPC_CREAT | 0777)) < 1){
        print("Error in shmget with IPC_CREAT creating teams array");
        exit(1);
    }
    teams = shmat(race_struct->teams_shmid, NULL, 0);
    for (int i = 0; i < config_struct->number_of_teams; i++)
        teams[i] = NULL;
}

int process_command(char *line){
    strip(line);
    
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    strcpy(command, line);
    strtok(command, " ");
    if (strcmp(line, "START RACE!") == 0){
        if (verify_teams() == TRUE){
            start_race();
            return TRUE;
        }
        else print("CANNOT START, NOT ENOUGH TEAMS");
        
    }
    else if (strcmp(command, "ADDCAR") == 0){
        add_car(line);
    }
    else{
        
        print(concat("WRONG COMMAND => ", line));
        //return INVALID_COMMAND;
    }
    return FALSE;
}

int add_car(char *line){
    char **line_splited = malloc(sizeof(char *) * CAR_INPUT_SIZE);
    char *temp = concat("", line);
    if(verify_car_command(temp, line_splited) == FALSE){
        print(concat("WRONG COMMAND => ", line));
        return INVALID_COMMAND; //Wrong command
    }
    free(temp);
    team *t;
    if((t = find_team(line_splited[2])) == NULL){
        #ifdef debug
        print(concat("ERROR FINDING TEAM =>", line));
        #endif//Unable to find the team
        return CANT_ADD_TEAM;
    }
    int car_shmid;
    if ((car_shmid = shmget(IPC_PRIVATE, sizeof(car) * config_struct->max_cars_team, IPC_CREAT | 0777)) < 1){
        perror("Error in shmget with IPC_CREAT\n");
        exit(1);
    }
    //Attach team cars array
    car **team_cars = shmat(t->cars_shmid, NULL, 0);
    //Attach car
    car *c = shmat(car_shmid, NULL, 0);
    c->number = strtol(line_splited[4], &temp, 10);
    c->speed = strtol(line_splited[6], &temp, 10);
    c->consumption = strtof(line_splited[8], &temp);
    c->reliability = strtol(line_splited[10], &temp, 10);
    team_cars[t->number_team_cars++] = c;
    shmdt(team_cars);
    shmdt(c);
    
    print(concat("CAR ADDED SUCCESSFULLY => ", line));
    return CAR_ADDED;
    //Car added successfully
}

int verify_car_command(char *line, char ** line_splited){
    for (int i = 0; i < CAR_INPUT_SIZE; i++)
        line_splited[i] = NULL;
    //read command
    char* temp;
    char* saveptr = line;
    int index = 0;
    while((temp = strtok_r(saveptr, " ,\n", &saveptr))){
        line_splited[index] = concat("", temp);
        index++;
    }
    //verify if something is NULL
    if(line_splited[CAR_INPUT_SIZE-1] == NULL)
        return FALSE;
    return TRUE;
}

team *find_team(char *team_name){
    int i = 0;
    for (i = 0; i < config_struct->number_of_teams; i++){
        if(teams[i] == NULL){
            return(create_team((char *)team_name, (int)i));
        }
        else if(strcmp(teams[i]->name, team_name) == 0) return teams[i];   
    }
    return NULL;
}

team *create_team(char *team_name, int i){
    int team_id;
    if ((team_id = shmget(IPC_PRIVATE, sizeof(team), IPC_CREAT | 0777)) < 1){
        perror("Error in shmget with IPC_CREAT\n");
        exit(1);
    }
    teams[i] = shmat(team_id, NULL, 0);
    if ((teams[i]->cars_shmid = shmget(IPC_PRIVATE, sizeof(car*) * config_struct->max_cars_team, IPC_CREAT | 0777)) < 1){
            perror("Error in shmget with IPC_CREAT\n");
            exit(1);
        }
    strcpy(teams[i]->name, team_name);
    teams[i]->number_team_cars = 0;
    teams[i]->box_status = FREE;
    return (teams[i]);
}

void start_race(){
    sem_post(&race_struct->race_begin);
    print("STARTING RACE...");
    pid_t new_team;
    for(int i = 0;i < config_struct->number_of_teams; i++){
        new_team = fork();
        if(new_team == 0){
            print("Creating team...");
            //sem_init(&, 1, 0);
            team_manager_start(i);
        }
    }
}

int verify_teams(){
    for (int i = 0; i < config_struct->number_of_teams; i++){
        if (teams[i] == NULL)
            return FALSE;
    }
    return TRUE;
}

