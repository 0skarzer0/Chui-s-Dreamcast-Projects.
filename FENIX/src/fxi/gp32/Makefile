DEFINE_GP32_LIBC=YES


FXE_TEXT =	"FENIX RUNTIME"
FXE_COMP =	"CHUI"
FXE_ICON =	../res/gp32icon.bmp


ARM_COMP = 	arm-elf


CC = $(ARM_COMP)-gcc
AR = $(ARM_COMP)-ar
AS = $(ARM_COMP)-as
ifndef DEFINE_GP32_LIBC
LD = 		$(ARM_COMP)-gcc
else
LD = 		$(ARM_COMP)-ld
endif
OBJCOPY = 	$(ARM_COMP)-objcopy
B2FXEC =	b2fxec
GPLINK =	gplink

EXEC_NAME =	fxi

MAPFILE	=	$(EXEC_NAME).map
TARGET_ELF =	$(EXEC_NAME).elf
TARGET_BIN =	$(EXEC_NAME).gxb
TARGET_FXE =	../../bin/fenix.rte
LDSCRIPT =	lnkscript
X_LDSCRIPT = 	arm-gp32bin.x
USER_INIT =	user_init.o


ifndef DEFINE_GP32_LIBC
.SFILES	=	crt0.s 
else
.SFILES =	crt0x_gp32.s 
endif


.CFILES	=	../src/main.c \
		../src/dcbr.c \
		../src/instance.c \
		../src/strings.c \
		../src/i_main.c \
		../src/i_debug.c \
		../src/i_func.c \
		../src/i_saveload.c \
		../src/g_main.c \
		../src/g_maps.c \
		../src/g_blit.c \
		../src/g_texts.c \
		../src/g_pal.c \
		../src/g_draw.c \
		../src/g_systexts.c \
		../src/g_console.c \
		../src/g_scroll.c \
		../src/g_flic.c \
		../src/g_mode7.c \
		../src/img_png.c \
		../src/img_pcx.c \
		../src/sound.c \
		../src/pathfind.c \
		../src/fnc_exports.c \
		../src/g_conversion.c \
		../src/g_font.c \
		../src/g_fpg.c \
		../src/g_blendop.c \
		../src/g_profiler.c \
		../../common/files.c \
		../../common/xctype.c \
		../../common/dirs.c \

ifndef DEFINE_GP32_LIBC
.CFILES+=	gpstart.c  \
		gpmain.c
else
.CFILES+=	main.c 
endif
		

.OFILES	=	$(.SFILES:.s=.o) $(.CFILES:.c=.o)


all: $(TARGET_FXE)

DEFS= -DGP32 -DTARGET_GP32 -DTARGET_linux -DFSL \

DEFS+= -DNO_CONSOLE -DNO_PROFILER 

DEFS+= -DUSE_FIXED_POINT

OPTFLAGS=-O3 -mtune=arm920 -march=armv4t -marm -mno-thumb-interwork -msoft-float -ffast-math -nostdlib -Wall -ffreestanding -fno-builtin -fno-exceptions -mstructure-size-boundary=8 -fomit-frame-pointer -fstrict-aliasing




ifndef DEFINE_GP32_LIBC

CFLAGS =	-Iinclude-gpsdk -I../../include -I../inc -I/opt/gp32/$(ARM_COMP)/include/SDL $(OPTFLAGS) $(DEFS)

LDFLAGS	=	-T$(LDSCRIPT) $(USER_INIT) -nostartfiles \
		-lgpos -lgpsound -lgpmem -lgpgraphic -lgpfont \
		-lSDL_image -lSDL_flic -lSDL_mixer -lSDL \
		-lpng -ljpeg -lz -lm -lgpstdlib -lgpstdio -Wno-thumb 
else

CFLAGS =	-Iinclude-libc -I../../include -I../inc -I/opt/gp32/$(ARM_COMP)/include/SDL $(OPTFLAGS) $(DEFS)

LDFLAGS	=	-Map $(MAPFILE) -nostartfiles --script $(X_LDSCRIPT) \
		-L/opt/gp32/arm-elf/lib -L/opt/gp32/lib/gcc/arm-elf/3.4.1
		
SYSTEMLIBS =	-lSDLx_image -lSDLx_mixer -lSDLx \
		-lpng_x -ljpeg_x -lz_x -lm -lx_gp32 -lgcc

DOLINK =	$(LD) $(LDFLAGS) $(.OFILES) $(SYSTEMLIBS) -o $(TARGET_ELF)
endif

clean:
	rm -f $(.OFILES) $(TARGET_FXE) $(TARGET_BIN) $(TARGET_ELF)

$(TARGET_FXE):	$(TARGET_BIN)
	$(B2FXEC) -t $(FXE_TEXT) -a $(FXE_COMP) -b $(FXE_ICON) $< $@


$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@


$(TARGET_ELF): $(.OFILES) $(DEPENDFILE) $(TARGET)
ifndef DEFINE_GP32_LIBC
	$(LD) -o $@ $(.OFILES) $(LDFLAGS)
else
	$(DOLINK)
endif


send: $(TARGET_FXE)
	$(GPLINK) -x put $(TARGET_FXE) /GPMM


run: $(TARGET_BIN)
	$(GPLINK) run $(TARGET_BIN)
