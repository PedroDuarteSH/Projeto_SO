CC = gcc
OBJS = race_simulator.c race_simulator.h
PROG = race_simulator
CFLAGS = -g -Wall -pthread


all:	${PROG} clean

clean:	rm ${OBJS} *~ ${PROG}


${PROG}:	${OBJS}
					${CC} ${CFLAGS} ${OBJS} -o $@

.c.o:		${CC} ${CFLAGS} $< -c $@


%.o:	%.c
