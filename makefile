CC = gcc
OBJS = race_simulator.c race_manager.c functions.c team_manager.c
PROG = race_simulator
CFLAGS = -Wall -pthread -g


all:	${PROG} clean

clean:	rm ${OBJS} *~ ${PROG}


${PROG}:	${OBJS}
					${CC} ${CFLAGS} ${OBJS} -o $@

.c.o:		${CC} ${CFLAGS} $< -c $@


%.o:	%.c
