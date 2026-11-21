TARGET = noiz2sa.elf

KOS_CFLAGS+= -I/opt/dc/kos/kos-ports/include/SDL -I/opt/dc/kos/kos-ports/zlib-1.2.1 -I/opt/dc/kos/kos-ports/libpng-1.2.5 -Isrc -Isrc/bulletml -DDREAMCAST -DDATA_PREFIX="/rd/" -DFILE_SEPARATOR="/" -DBOOST_NO_EXCEPTIONS -DSCREEN_DIVISOR=2

all: $(TARGET)

include $(KOS_BASE)/Makefile.rules

OBJS =	src/noiz2sa.o \
	src/ship.o \
	src/shot.o \
	src/frag.o \
	src/bonus.o \
	src/foe.o \
	src/foecommand.o \
	src/barragemanager.o \
	src/attractmanager.o \
	src/background.o \
	src/letterrender.o \
	src/screen.o \
	src/clrtbl.o \
	src/vector.o \
	src/degutil.o \
	src/rand.o \
	src/soundmanager.o \
	src/bulletml/bulletmlparser.o \
	src/bulletml/bulletmlparser-tinyxml.o \
	src/bulletml/bulletmlrunner.o \
	src/bulletml/bulletmlrunnerimpl.o \
	src/bulletml/bulletmltree.o \
	src/bulletml/calc.o \
	src/bulletml/formula-variables.o \
	src/bulletml/tinyxml/tinyxml.o \
	src/bulletml/tinyxml/tinyxmlerror.o \
	src/bulletml/tinyxml/tinyxmlparser.o \
	romdisk.o

clean:
	rm -f $(OBJS) $(TARGET).elf  romdisk.o romdisk.img

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
	$(OBJS) -lpng -lz -lSDL_mixer -lSDL_image -lSDL $(OBJEXTRA) -lm -lstdc++ $(KOS_LIBS)

romdisk.o: romdisk.img
#	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o
	bin2odc romdisk.img romdisk.o

romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d data -v

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

