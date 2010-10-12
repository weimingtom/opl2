MINGW = 1

OBJS += opl2.o opl2_int.o opl2_pl2.o opl2_tmb.o opl2_tsb.o opl2_tcm.o
OBJS += opl2_idx.o opl2_gl.o opl2_al.o opl2_lua.o

ifeq ($(PSP),1)

 PSPSDK = $(shell psp-config --pspsdk-path)
 PSPDEV = $(shell psp-config --pspdev-path)

 # PSP
 LIBS += -llua -lglut -lGLU -lGL -lALut -lOpenAL32
 LIBS += -lpspvfpu -lpsprtc -lpspaudio -lpsphprm -lm
 TARGET = opl2
 BUILD_PRX = 1
 CFLAGS += -Wall -g -G0

 include $(PSPSDK)/lib/build.mak

debug: $(TARGET).prx $(TARGET).elf
	psp-gdb -ex "target remote :10001" --args "$(TARGET).elf"

else

CC = gcc
CFLAGS += -Wall -g

 ifeq ($(MINGW),1)
  # MinGW
  LIBS += -llua -lglut32 -lfreeglut -lGLU32 -lOpenGL32 -lwinmm -lgdi32 -lALut -lOpenAL32
  CFLAGS += -DFREEGLUT_STATIC
  TARGET = opl2.exe
 else
  # *nix
  LIBS += -llua -lfreeglut -lGLU -lGL -lalut -lopenal
  TARGET = opl2
 endif

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean $(TARGET)

GEOM = -geometry 640x480+0+0
test: $(TARGET)
	$(TARGET) $(GEOM)
debug: $(TARGET)
	gdb -ex run --args $(TARGET) $(GEOM)

endif
