CC=gcc
CFLAGS+=-Wall -g
LIBS+=-lGL -lGLU -lglut -lopenal -lalut -llua

TARGET=opl2
OBJS=opl2.o opl2_tmb.o opl2_tsb.o opl2_pl2.o #opl2_tcm.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)
