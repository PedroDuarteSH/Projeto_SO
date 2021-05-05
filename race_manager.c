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
#endif

    char line[READ_BUFF];
    int named_pipe, readed_chars;
    if ((named_pipe = open(PIPENAME, O_RDONLY)) < 0){
        print("Cannot open pipe for reading in race_manager");
        clear_resources();
    }

    //IMPLEMENTAR SELECT
    while (TRUE){
        if((readed_chars = read(named_pipe, line, READ_BUFF)) == -1)
            perror("Error reding from named pipe: ");
        line[readed_chars-1] = '\0';
        if (race_struct->status == NOT_STARTED){
            race_struct->status = process_command(line);
        }
        else{

        }
    }
    free(line);
    //espera todas as equipas terminarem
    for (int i = 0; i < config_struct->number_of_teams; i++) wait(NULL);

}

int process_command(char *line){
    strip(line);
    char *command = malloc(sizeof(char) * INPUT_LENGHT);
    strcpy(command, line);
    strtok(command, " ");
    if (strcmp(line, "START RACE!") == 0){
        if (verify_teams() == TRUE){
            start_race();
            free(command);
            return TRUE;
        }
        else print("CANNOT START, NOT ENOUGH TEAMS");
    }
    else if (strcmp(command, "ADDCAR") == 0){
        add_car(line);
    }
    else{
        print(concat("WRONG COMMAND => ", line));
    }
    free(command);
    
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
    
    team * car_team;
    if((car_team = find_team(line_splited[2])) == NULL){
        #ifdef debug
        print(concat("ERROR FINDING TEAM =>", line));
        #endif
        return CANT_ADD_TEAM;
    }

    car * car_to_add = find_car_pos(car_team);

    car_to_add->number = strtol(line_splited[4], &temp, 10);
    car_to_add->speed = strtol(line_splited[6], &temp, 10);
    car_to_add->consumption = strtof(line_splited[8], &temp);
    car_to_add->reliability = strtol(line_splited[10], &temp, 10);

    //Create Unnamed Car Pipe
    if(pipe(car_to_add->comunication_pipe) == -1){
		print("Error creating temp pipe");
		clear_resources();
	}
    free(line_splited);
    car_team->number_team_cars++;
    print(concat("CAR ADDED SUCCESSFULLY => ", line));
    return CAR_ADDED;
}

car * find_car_pos(team *car_team){
    return (car *)(race_struct + 1 + config_struct->number_of_teams + 1 + car_team->team_number * config_struct->max_cars_team + car_team->number_team_cars);
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
    team *temp_team = (team *)(race_struct + 1);
    for (int i = 0; i < config_struct->number_of_teams; i++){
        if(temp_team->initiated == EMPTY){
            return(create_team(temp_team, team_name,  i));
        }
        else if(strcmp(temp_team->name, team_name) == 0) return temp_team;   
        team *temp_team = (team *)(temp_team + 1);
    }
    return NULL;
}

team *create_team(team* team_struct, char *team_name, int team_number){
    strcpy(team_struct->name, team_name); 
    team_struct->number_team_cars = 0;
    team_struct->box_status = FREE;
    team_struct->team_number = team_number;
    if(fork() == 0){
        print(concat("CREATING TEAM !", team_name));
        team_manager_start(team_number);
    }

    team_struct->initiated = CREATED;
    return team_struct;
}

void start_race(){
    for (int i = 0; i < config_struct->number_of_teams; i++){
        sem_wait(&race_struct->teams_ready);
    }
    print("STARTING RACE...");
    for (int i = 0; i < config_struct->number_of_teams + 1; i++)
        sem_post(&race_struct->race_begin);    
}

int verify_teams(){
    team *temp_team = (team *)(race_struct + 1);
    for (int i = 0; i < config_struct->number_of_teams; i++){
        if (temp_team->initiated == EMPTY)
            return FALSE;
        team *temp_team = (team *)(temp_team + 1);
    }
    return TRUE;
}

