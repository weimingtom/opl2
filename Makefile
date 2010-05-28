CC=gcc
CFLAGS+=-Wall -g
LIBS+=-lGL -lGLU -lglut -lopenal -lalut -llua

TARGET=opl2
OBJS=opl2.o opl2_tmb.o opl2_tsb.o #opl2_cam.o opl2_mdl.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)
