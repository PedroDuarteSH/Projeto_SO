CC = gcc
OBJS = race_simulator.c race_simulator.h shared_mem.h race_manager.c race_manager.h functions.c functions.h team_manager.c cars.c
PROG = race_simulator
CFLAGS = -Wall -pthread -g


all:	${PROG} clean

clean:	rm ${OBJS} *~ ${PROG}


${PROG}:	${OBJS}
					${CC} ${CFLAGS} ${OBJS} -o $@

.c.o:		${CC} ${CFLAGS} $< -c $@


%.o:	%.c
