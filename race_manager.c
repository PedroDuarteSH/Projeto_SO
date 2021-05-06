//Joao Fernandes 2019220273
//Pedro Henriques 2019217793

//Começa a corrida e acaba
//Mostra classificação Final e as equipas ainda em jogo
//Cria processos Team_manager
#include "race_manager.h"
#define READ_BUFF 512


void race_manager_init(){

#ifdef DEBUG
    print_config_file();
    fflush(stdout);
#endif
    
    char line[READ_BUFF];
    int named_pipe, readed_chars, car_state;
    int max_number_cars = config->number_of_teams * config->max_cars_team;    

    create_pipes(max_number_cars);
    //Read Pipes
    fd_set read_set;
    while (TRUE){
        //Open Named Pipe
        if ((named_pipe = open(PIPENAME, O_RDONLY | O_NONBLOCK)) < 0){
            print("Cannot open pipe for reading in race_manager");
            clear_resources();
        }
        FD_ZERO(&read_set);
        FD_SET(named_pipe, &read_set);
        car * car_imp = (car *) (race_struct + 1 + config->number_of_teams);
        for (int i = 0; i < max_number_cars; i++){
            FD_SET(car_imp->comunication_pipe[0], &read_set);
            car * car_imp = (car *) (car_imp + 1);
        }

        if(select(named_pipe+1, &read_set, NULL, NULL, NULL) > 0){
            if(FD_ISSET(named_pipe, &read_set)){
                if((readed_chars = read(named_pipe, line, READ_BUFF)) == -1)
                    perror("Error reding from named pipe: ");
                line[readed_chars-1] = '\0';
                if (race_struct->status == NOT_STARTED) print(process_command(line));
                else print(RACE_STARTED_ERR);
            }
            car * car_imp = (car *) (race_struct + 1 + config->number_of_teams);
            for (int i = 0; i < max_number_cars; i++){
                if(FD_ISSET(car_imp->comunication_pipe[0], &read_set)){
                    if(read(named_pipe, &car_state, READ_BUFF) == -1)
                        perror("Error reding from named pipe: ");
                    
                }
            }
            
        }


    }
        


    
    free(line);
    //espera todas as equipas terminarem
    for (int i = 0; i < config->number_of_teams; i++) wait(NULL);

}

void create_pipes(int max_number_cars){
    car * car_imp = (car *) (race_struct + 1 + config->number_of_teams);
    for (int i = 0; i < max_number_cars; i++){
        if(pipe(car_imp->comunication_pipe) == -1){
            print("Error creating temp pipe");
		    clear_resources();
        }
#ifdef DEBUG
        print("Unnamed Pipe created");
#endif
        car * car_imp = (car *) (car_imp + 1);
    }
    
}

char* process_command(char *line){
    strip(line);
    print(concat("PROCESS COMMAND => ", line));
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    strcpy(command, line);
    strtok(command, " ");
    if (strcmp(line, START_RACE) == 0){
        if (verify_teams() == TRUE){
            start_race();
            free(command);
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
    team * car_team;
    if((car_team = find_team(line_splited[2])) == NULL) return concat(CANT_ADD_TEAM, line);
    
    //Process car
    int parse_error = 0;
    int car_number = strtol(line_splited[4], &temp, 10);
    if (temp == line_splited[4])parse_error++;
    car *car_to_add = find_car_pos(car_team, car_number);
    if(car_to_add == NULL)
        return CAR_NUMBER_EXISTS;
    //Initiate cars data
    car_to_add->team_number = car_team->team_number;
    car_to_add->state = NOTSTARTED;
    car_to_add->box_stops = 0;
    car_to_add->current_fuel = config->Fuel_tank_capacity;
    car_to_add->number = car_number;
    car_to_add->speed = strtol(line_splited[6], &temp, 10);
    if (temp == line_splited[6])parse_error++;
    car_to_add->consumption = strtof(line_splited[8], &temp);
    if (temp == line_splited[8])parse_error++;
    car_to_add->reliability = strtol(line_splited[10], &temp, 10);
    if (temp == line_splited[10])parse_error++;

    if(parse_error != 0){
        car_to_add->number = EMPTY;
        if(car_team->number_team_cars == 0)
            car_team->initiated = EMPTY;
        return concat(WRONG_COMMAND, line);
    }
    
    car_team->number_team_cars++;
    
    free(line_splited);
   
    return concat(CAR_ADDED, line);
}

car * find_car_pos(team *car_team, int number){
    car * temp_car = (car *) (race_struct + 1 + config->number_of_teams + car_team->team_number * config->max_cars_team);
    for (int i = 0; i < config->max_cars_team; i++){
        if(temp_car->number == number)
            return NULL;
        else if(temp_car->number == EMPTY)
            return temp_car;
        temp_car++;
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
    //verify if something is NULL
    if(line_splited[CAR_COMMAND_SIZE-1] == NULL)
        return FALSE;
    return TRUE;
}

team *find_team(char *team_name){
    team *temp_team = (team *)(race_struct + 1);
    for (int i = 0; i < config->number_of_teams; i++){
        if(temp_team->initiated == EMPTY) return(create_team(temp_team, team_name,  i));
        else if(strcmp(temp_team->name, team_name) == 0) return temp_team;   
        temp_team = (team *)(temp_team + 1);
    }
    return NULL;
}

team *create_team(team* team_struct, char *team_name, int team_number){
    strcpy(team_struct->name, team_name); 
    team_struct->number_team_cars = 0;
    team_struct->box_status = FREE;
    team_struct->team_number = team_number;
    team_struct->initiated = CREATED;
    return team_struct;
}

void start_race(){
    //Change status to ignore input commands
    race_struct->status = STARTED;
    //Create team processes
    team *temp = (team *)(race_struct + 1);
    for (int i = 0; i < config->number_of_teams; i++){
        if(fork() == 0){
            print(concat("TEAM CREATED => ", temp->name));
            team_manager_start(temp);
        }
        temp = temp + 1;
    }

    //Wait for all teams ready (Change to car threads?????)
    for (int i = 0; i < config->number_of_teams; i++){
        sem_wait(&race_struct->teams_ready);
    }
    //Inform cars and malfunction process that race has started
    for (int i = 0; i < config->number_of_teams + 1; i++)
        sem_post(&race_struct->race_begin);    
}

int verify_teams(){
    team *temp_team = (team *)(race_struct + 1);
    for (int i = 0; i < config->number_of_teams; i++){
        if (temp_team->initiated == EMPTY)
            return FALSE;
        temp_team = (team *)(temp_team + 1);
    }
    return TRUE;
}

