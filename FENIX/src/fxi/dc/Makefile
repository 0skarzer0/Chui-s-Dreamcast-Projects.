TARGET=fxi

INCS = -Idc/include -I$(KOS_BASE)/libc/include -I$(KOS_BASE)/include -I$(KOS_BASE)/kernel/arch/dreamcast/include -Iinc -Iinclude -Ifsl/../src -Ifsl/../src/SFont -Ifsl/../src/ML_Lib
DEFS= -DDREAMCAST -DTARGET_DREAMCAST -DTARGET_DC -DTARGET_LINUX -DTARGET_linux -DLINUX -DFSL -DDATA_PREFIX="/cd/" -DFILE_SEPARATOR="/" 
OPTFLAGS=-O2 -fomit-frame-pointer -Wno-switch
KOS_CFLAGS+=-D_arch_dreamcast $(DEFS) $(INCS) -Iinclude/ -I../inc -I../../include -I../src -I$(KOS_BASE)/addons/include -I$(KOS_BASE)/addons/include/SDL $(OPTFLAGS)


all: $(TARGET).elf

include $(KOS_BASE)/Makefile.rules


.CFILES =	dcmain.c \
		../src/main.c \
		../src/dcbr.c \
		../src/instance.c \
		../src/strings.c \
		../src/i_main.c \
		../src/i_debug.c \
		../src/i_func.c \
		../src/i_saveload.c \
		../src/i_copy.c \
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
		../src/f_joystick.c \
		../src/fgc.c \
		../src/fbm.c \
		../src/fpl.c \
		../src/f_sort.c \
		../src/f_bgload.c \
		../src/f_cd.c \
		../../common/files.c \
		../../common/xctype.c \
		../../common/dirs.c \
		../../common/regex.c \


OBJS =		$(.CFILES:.c=.o)


$(TARGET).elf: $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET).elf $(KOS_START) \
	$(OBJS)  $(OBJEXTRA) -lpng_128 -lz_123 -lSDL_mixer_126 -lSDL_image_124 -lSDL_129 $(OBJEXTRA) -lm -lstdc++ $(KOS_LIBS) 


$(TARGET).bin:
	$(KOS_OBJCOPY) -O binary $(TARGET).elf output.bin
	scramble $(TARGET).bin $(TARGET).bin
		

run: $(TARGET).elf
	$(KOS_LOADER) $(TARGET).elf

clean:
	rm -f $(OBJS) output.bin $(TARGET).elf romdisk.* example.iso
