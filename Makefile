include objects.mak

CC = gcc
CFLAGS += -Wall -g

LIBS += -llua -lglut -lGLU -lGL -lalut -lopenal -lvorbis -lvorbisfile
TARGET = opl2

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean $(TARGET)

ARGS = -window 640x480
test: $(TARGET)
	$(TARGET) $(ARGS)
debug: $(TARGET)
	gdb -ex run --args $(TARGET) $(ARGS)

