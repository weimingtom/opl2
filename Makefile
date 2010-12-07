PLATFORMS = nix win

OBJS += src/opl2.o src/opl2_al.o src/opl2_chr.o src/opl2_fnt.o src/opl2_gl.o
OBJS += src/opl2_idx.o src/opl2_int.o src/opl2_lua.o src/opl2_ogg.o src/opl2_pl2.o
OBJS += src/opl2_psd.o src/opl2_tcm.o src/opl2_tmb.o src/opl2_tsb.o src/opl2_vm.o

EXTRA_OBJS = tools/pl2ex.o tools/dumptmb.o
EXTRA_TARGETS = $(PL2EX) $(DUMPTMB) $(VMTEST)

INCDIR = include/

CFLAGS += -Wall -g $(addprefix -I,$(INCDIR))
LIBS += -lvorbisfile -lvorbis -logg -lm

WIN_LIBS += -llua -lmingw32 -lSDLmain -lSDL.dll
WIN_LIBS += -lGLU32 -lOpenGL32 -lwinmm -lgdi32 -lOpenAL32

NIX_LIBS += -llua5.1 -lSDL -lGLU -lGL -lopenal

ARGS = --window 640x480

ifeq ($(RELEASE),1)
 CFLAGS += -O2 -DNDEBUG
else
 ifeq ($(PROFILE),1)
  CFLAGS += -pg
  LIBS += -pg
 endif
endif

ifeq ($(PLATFORM),)

CCINFO := $(shell gcc -dumpmachine)

ifeq ($(findstring mingw,$(CCINFO)),mingw)
 PLATFORM=win
else
 PLATFORM=nix
endif

 ifeq ($(CCINFO),)

_default:
	@echo "Could not auto-detect platform."
	@echo "Try 'make platform' where platform is one of:"
	@echo "	$(PLATFORMS)"

nix:
	make PLATFORM=nix all
nix-clean:
	make PLATFORM=nix clean
nix-rebuild:
	make PLATFORM=nix rebuild
nix-test:
	make PLATFORM=nix test
nix-debug:
	make PLATFORM=nix debug
nix-release:
	make PLATFORM=nix RELEASE=1 rebuild

win:
	make PLATFORM=win all
win-clean:
	make PLATFORM=win clean
win-rebuild:
	make PLATFORM=win rebuild
win-test:
	make PLATFORM=win test
win-debug:
	make PLATFORM=win debug
win-release:
	make PLATFORM=win RELEASE=1 rebuild

 endif
endif

CC = gcc

OBJS += src/opl2_x86.o

ifeq ($(PLATFORM),win)

 WINDRES = windres.exe
 OBJS += res/opl2.res
 EXTRA_OBJS += res/opl2.rc
 #WINAPP = -mwindows

 LIBS += $(WIN_LIBS)
 TARGET = opl2.exe

 PL2EX = pl2ex.exe
 DUMPTMB = dumptmb.exe
 VMTEST = vmtest.exe

else

 LIBS += $(NIX_LIBS)
 TARGET = opl2

 PL2EX = pl2ex
 DUMPTMB = dumptmb
 VMTEST = vmtest

endif

all: $(TARGET) $(PL2EX) $(DUMPTMB)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(WINAPP)

clean:
	rm -f $(TARGET) $(EXTRA_TARGETS) $(OBJS) $(EXTRA_OBJS)

rebuild: clean all

test: $(TARGET)
	./$(TARGET) $(ARGS)
debug: $(TARGET)
	gdb -ex "break main" -ex run --args $(TARGET) $(ARGS)

profile:
	make PROFILE=1 rebuild
	./$(TARGET) $(ARGS)
	gprof $(TARGET) > profile.log

release:
	make RELEASE=1 rebuild

res/opl2.rc: res/opl2.ico
	@echo A ICON MOVEABLE PURE LOADONCALL DISCARDABLE $< > $@
res/opl2.res: res/opl2.rc
	$(WINDRES) --input-format=rc -o $@ $(RCINCS) $^ -O coff

$(VMTEST): src/opl2_vm.o src/opl2_x86.c
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ -lm -DVMTEST $(LIBS)

$(PL2EX): tools/pl2ex.o src/opl2_int.o src/opl2_pl2.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)

$(DUMPTMB): tools/dumptmb.o src/opl2_int.o src/opl2_tmb.o src/opl2_pl2.o src/opl2_vm.o src/opl2_x86.c src/opl2_idx.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: _default all $(PLATFORMS) clean rebuild test debug release profile
