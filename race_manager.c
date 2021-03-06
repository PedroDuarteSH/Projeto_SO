//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager
#include "race_manager.h"

char car_states[7][30] = {"is giving up...",
                            "hasn't started!",
                            "entered the pits...",
                            "entered security mode!",
                            "is on Track!",
                            "Finished!",
                            "is now Malfuntioning..."};

void race_manager_init(){
    signal(SIGINT, finish_exit);
    signal(SIGUSR1, interrupt_race);

#ifdef DEBUG
    printf("Race Manager PID: %d\n", getpid());

    print("Starting race process manager...");
    print_config_file();
    fflush(stdout);
#endif

    clean_data();
    car_classification = 0;
    race->finished_cars = 0;
    create_pipes();
    while(TRUE){
        read_pipes();
        for (int i = 0; i < config->number_of_teams; i++) wait(NULL);
        print("RACE FINISHED!");
        kill(malfunction_manager_process, SIGUSR1);
        kill(main_pid, SIGTSTP);
        reset_race();
    }
    
    
}

void read_pipes(){
    char *command;
    char *savePtr = 0;
    while (TRUE){
        team_stuct *team_temp;
        car_struct *temp_car;
        sem_wait(&race->change_status);
        if(race->finished_cars == race->number_of_cars && (race->status == STARTED || race->status == INTERRUPTED)){
            race->status = TERMINATED;
            sem_post(&race->change_status);
            break;
        }else sem_post(&race->change_status);
                
        //Open Named Pipe
        named_pipe = open(PIPENAME, O_RDWR | O_NONBLOCK);

        FD_SET(named_pipe, &read_set);
        team_temp = (team_stuct *)(first_team);
        for (int i = 0; i < config->number_of_teams; i++){
            FD_SET(team_temp->comunication_pipe[0], &read_set);
            team_temp = (team_stuct *)(team_temp + 1);
        }
        if(select(named_pipe+1, &read_set, NULL, NULL, NULL) > 0){
            if(FD_ISSET(named_pipe, &read_set)){
                if((readed_chars = read(named_pipe, line, READ_BUFF)) == -1)
                    perror("Error reding from named pipe: ");
                line[readed_chars-1] = '\0';
                command = strtok_r(line, "\n", &savePtr);
                while(command != NULL){
                    sem_wait(&race->change_status);
                    if (race->status == STARTED){
                        print(RACE_STARTED_ERR);
                        sem_post(&race->change_status);
                    }
                    
                    else{
                        sem_post(&race->change_status);
                        print(process_command(line));
                    }
                    command = strtok_r(savePtr, "\n", &savePtr);
                }
            }
            team_temp = (team_stuct *)(first_team);
            for (int i = 0; i < config->number_of_teams; i++){
                if(FD_ISSET(team_temp->comunication_pipe[0], &read_set)){
                    if(read(team_temp->comunication_pipe[0], &temp_car, sizeof(car_struct *)) > 0){
                        snprintf(line, READ_BUFF, "Car %d from team %s %s",temp_car->number, team_temp->name, car_states[temp_car->state]);
                        sem_wait(&temp_car->car_check);
                        if(temp_car->state == FINISHED){
                            car_classification++;
                            temp_car->finish_place = car_classification;
                        }
                        sem_post(&temp_car->car_check);
                        print(line);
                    }
                }
                team_temp = (team_stuct *)(team_temp + 1);
            }
        }
        close(named_pipe);
    }
    
}


void create_pipes(){
    team_stuct * team_temp = (team_stuct *)(first_team);
    for (int i = 0; i < config->number_of_teams; i++){
        if(pipe(team_temp->comunication_pipe) == -1){
            #ifdef DEBUG
            print("Error creating temp pipe");
            #endif
		    clear_resources();
        }
        #ifdef DEBUG
        print("Unnamed Pipe created");
        #endif
        team_temp = (team_stuct *)(team_temp + 1);
    }
    
}

char* process_command(char *line){
    strip(line);
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    strcpy(command, line);
    strtok(command, " ");
    if (strcmp(line, START_RACE) == 0){
        free(command);
        if (verify_teams() == TRUE){
            start_race();
            
            return RACE_STARTING;
        }
        return START_ERROR;
    }
    else if (strcmp(command, ADD_CAR) == 0){
        free(command);
        return add_car(line);
    }
    else{
        free(command);
        return (concat(WRONG_COMMAND, line));
    }
}

char* add_car(char *line){
    char **line_splited = malloc(sizeof(char *) * CAR_COMMAND_SIZE);
    char *temp = concat("", line);
    if(verify_car_command(temp, line_splited) == FALSE) return concat(WRONG_COMMAND, line);
    free(temp);
 
    //Verify parsing errors
    int parse_error = 0;
    
    int car_number = strtol(line_splited[4], &temp, 10);
    if (temp == line_splited[4])parse_error++;
    
    int speed = strtol(line_splited[6], &temp, 10);
    if (temp == line_splited[6])parse_error++;
    
    float consumption = strtof(line_splited[8], &temp);
    if (temp == line_splited[8])parse_error++;

    int reliability = strtol(line_splited[10], &temp, 10);
    if (temp == line_splited[10])parse_error++;

    if(parse_error != 0) return concat(WRONG_COMMAND, line);
    
    //Find/Init team and car
    team_stuct *car_team;
    if((car_team = find_team(line_splited[2])) == NULL)return concat(CANT_ADD_TEAM, line);
   
    car_struct *car_to_add = find_car_pos(car_team, car_number);
    if(car_to_add == NULL){
        if(car_team->number_team_cars == 0)
            car_team->team_number = EMPTY;
        return concat(CAR_NUMBER_EXISTS, line);
    }
        
    //Initiate cars data
    car_to_add->team_number = car_team->team_number;
    car_to_add->number = car_number;
    car_to_add->speed = speed;
    car_to_add->consumption = consumption;
    car_to_add->reliability = reliability;
    

    car_team->number_team_cars++;
    
    free(line_splited);
    race->number_of_cars++;
    return concat(CAR_ADDED, line);
}

car_struct *find_car_pos(team_stuct *car_team, int number){
    car_struct *temp_car = (car_struct *) first_car;
    temp_car = temp_car + (config->max_cars_team * car_team->team_number);
    for (int i = 0; i < config->max_cars_team; i++){
        if(temp_car->number == number){
            return NULL;
        }        
        else if(temp_car->number == EMPTY)    
            return temp_car;
        
        temp_car = (car_struct *)(temp_car+1);
    }
    return NULL;
}

int verify_car_command(char *line, char ** line_splited){
    for (int i = 0; i < CAR_COMMAND_SIZE; i++)
        line_splited[i] = NULL;
    //read command
    char* temp;
    int index = 0;
    while((temp = strtok_r(line, " ,\n", &line)) && index < 11){
        line_splited[index] = concat("", temp);
        index++;
    }
    if(line_splited[CAR_COMMAND_SIZE-1] == NULL)
        return FALSE;
    return TRUE;
}

team_stuct *find_team(char *team_name){
    team_stuct *temp_team = first_team;
    for (int i = 0; i < config->number_of_teams; i++){
        if(temp_team->team_number == EMPTY) return(create_team(temp_team, team_name,  i));
        else if(strcmp(temp_team->name, team_name) == 0) return temp_team;   
        temp_team = (team_stuct *)(temp_team + 1);
    }
    return NULL;
}

team_stuct *create_team(team_stuct *team_struct, char *team_name, int team_number){ 
    strcpy(team_struct->name, team_name); 
    team_struct->number_team_cars = 0;
    team_struct->box_status = FREE;
    team_struct->team_number = team_number;
    return team_struct;
}

void start_race(){
    race->finished_cars = 0;
    //Change status to ignore input commands
    sem_wait(&race->change_status);
    race->status = STARTED;
    sem_post(&race->change_status);
    //Create team processes
    team_stuct *temp_team = first_team;
    for (int i = 0; i < config->number_of_teams; i++){
        if(fork() == 0){
#ifdef DEBUG
            print(concat("TEAM CREATED => ", temp_team->name));
#endif
            team_manager_start(temp_team);
        }
        temp_team = (team_stuct *)(temp_team + 1);
    }

    //Wait for all teams ready (Change to car threads?????)
    temp_team = first_team;
    for (int i = 0; i < config->number_of_teams; i++){
        for (int j = 0; j < temp_team->number_team_cars; j++) 
            sem_wait(&race->cars_ready);
        temp_team = (team_stuct *)(temp_team + 1);
    }
    //Inform cars and malfunction process that race has started
    sem_post(&race->race_begin);
    temp_team = first_team;
    //Inform cars and malfunction process that race has started

    for (int i = 0; i < config->number_of_teams; i++){
        for (int j = 0; j < temp_team->number_team_cars; j++)
            sem_post(&race->race_begin);
        temp_team = (team_stuct *)(temp_team + 1);
    }
}

int verify_teams(){
    team_stuct *temp_team = (team_stuct *) first_team;
    for (int i = 0; i < config->number_of_teams; i++){
        if (temp_team->team_number == EMPTY)
            return FALSE;
        temp_team = (team_stuct *) (temp_team + 1);
    }
    return TRUE;
}

void finish_exit(int signum){
    if(race->status != NOT_STARTED){
        interrupt_race(signum);
        read_pipes();
        for (int i = 0; i < config->number_of_teams; i++) wait(NULL);
    }
    close_pipes();
    clear_resources(SIGINT);
    exit(0);
}

void interrupt_race(int signum){
    print("RECIEVED RACE INTERRUPTION");
    sem_wait(&race->change_status);
    if(race->status == STARTED){
        race->status = INTERRUPTED;
    }
    sem_post(&race->change_status);
}

void reset_race(){
    close_pipes();
    car_classification = 0;
    create_pipes();
    FD_ZERO(&read_set);
}

void close_pipes(){
    team_stuct * team_temp = (team_stuct *)(first_team);
    for (int i = 0; i < config->number_of_teams; i++){
        close(team_temp->comunication_pipe[0]);
        close(team_temp->comunication_pipe[1]);
        team_temp = (team_stuct *)(team_temp + 1);
    }
}