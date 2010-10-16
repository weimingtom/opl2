PLATFORMS = nix unix linux win dos mingw psp pspsdk

OBJS += opl2.o opl2_pl2.o opl2_tmb.o opl2_tsb.o opl2_tcm.o opl2_psd.o opl2_ogg.o
OBJS += opl2_int.o opl2_idx.o opl2_gl.o opl2_al.o opl2_vm.o opl2_lua.o

CFLAGS += -Wall -g
LIBS += -llua -lvorbisfile -lvorbis

ARGS = -window -geometry 640x480+0+0

ifeq ($(RELEASE),1)
 CFLAGS += -O2 -DNDEBUG
else
 ifeq ($(PROFILE),1)
  CFLAGS += -pg
  LIBS += -pg
 endif
endif 

default:
	@echo Try "make platform" where platform is one of:
	@echo 	$(PLATFORMS)

nix unix linux:
	make PLATFORM=nix all
nix-clean:
	make PLATFORM=nix clean
nix-rebuild:
	make PLATFORM=nix rebuild
nix-test:
	make PLATFORM=nix test
	
win dos mingw:
	make PLATFORM=win all	
win-clean:
	make PLATFORM=win clean
win-rebuild:
	make PLATFORM=win rebuild
win-test:
	make PLATFORM=win test
	
psp pspsdk:
	make PLATFORM=psp all
psp-clean:
	make PLATFORM=psp clean
psp-rebuild:
	make PLATFORM=psp rebuild
psp-debug:
	make PLATFORM=psp debug

ifeq ($(PLATFORM),psp)

 PSPDEV = $(shell psp-config --pspdev-path)
 PSPSDK = $(shell psp-config --pspsdk-path)

 LIBS += -lglut -lGLU -lGL -lALut -lOpenAL32 -logg
 LIBS += -lpspvfpu -lpsprtc -lpspaudio -lpsphprm -lm

 TARGET = opl2
 BUILD_PRX = 1
 CFLAGS += -G0

 include $(PSPSDK)/lib/build.mak

debug: $(TARGET).prx $(TARGET).elf
	psp-gdb -ex "target remote :10001" --args "$(TARGET).elf"

else

 CC = gcc

 ifeq ($(WITH_SSE),1)
  CFLAGS += -msse -DWITH_SSE
 endif
 ifeq ($(WITH_SSE2),1)
  CFLAGS += -msse2 -DWITH_SSE2
 endif

 ifeq ($(PLATFORM),win)

  WINDRES = windres.exe
  OBJS += opl2.res
  WINAPP = -mwindows

  LIBS += -lfreeglut -lGLU32 -lOpenGL32 -lwinmm -lgdi32 -lALut -lOpenAL32
  CFLAGS += -DFREEGLUT_STATIC
  TARGET = opl2.exe
  PL2EX = pl2ex.exe

opl2.rc:
	@echo A ICON MOVEABLE PURE LOADONCALL DISCARDABLE opl2.ico > $@
opl2.res: opl2.rc
	$(WINDRES) --input-format=rc -o $@ $(RCINCS) $^ -O coff

 else

  LIBS += -lglut -lGLU -lGL -lalut -lopenal
  TARGET = opl2
  PL2EX = pl2ex

 endif

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(WINAPP)

clean:
	rm -f $(TARGET) $(OBJS)

rebuild: clean all

test: $(TARGET)
	./$(TARGET) $(ARGS)
debug: $(TARGET)
	gdb -ex run --args $(TARGET) $(ARGS)

$(PL2EX): pl2ex.o opl2_int.o opl2_pl2.o
	$(CC) -o $@ $^ $(LIBS)

all: $(TARGET) $(PL2EX)

endif
	
.PHONY: default all $(PLATFORMS) clean rebuild test debug
