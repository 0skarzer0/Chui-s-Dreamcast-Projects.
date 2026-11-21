# Fenix - Compilador/intérprete de videojuegos
# Copyright (C) 1999 José Luis Cebrián Pagüe
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

OBJECTS = mixer.o ogg.o wav.o
SOURCES = mixer.c ogg.c wav.c
HEADERS = codec.h dumbogg.h ogg.h vorbisenc.h wav.h dumb.h mixer.h os_types.h vorbisfile.h

# Default options
ifndef target
error:
	@echo "ERROR: Debe compilar con make target=N"
	@echo "       (N puede ser MAC, linux o WIN32)"
endif

debug = true
SDL = /usr/local/include/SDL

CFLAGS += -DTARGET_$(target) 
CFLAGS += -I../../include -I../../fxi/inc

ifeq ($(debug), true)
    CFLAGS += -g
else
    CFLAGS += -O2
endif

ifeq ($(target), win32)
    CFLAGS += -I/win32/include -I$(SDL)/include -I$(SDL)/include/SDL -D_WINDOWS
    LIBS += -lmingw32 -lpng -lz -L$(SDL)/lib -lSDL -mwindows -ldumb
else
ifeq ($(target), BeOS)
    CFLAGS += `sdl-config --cflags` 
    LIBS += `sdl-config --libs` -lpng -lpng -lz -ldl -ldumb
else
    CFLAGS += `sdl-config --cflags` 
    LIBS += `sdl-config --libs` -lpng -lm -logg -lvorbisfile -lvorbis -ldumb
  endif
endif

# Redefine implicit rules

%.o: %.c $(HEADERS)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

# BeOS DLL

ifeq ($(target),BeOS)
CFLAGS += -I../../include -I../../fxi/inc $$(sdl-config --cflags)

../../bin/lib/gui.so: $(OBJECTS)
	@(mkdir ../../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -nostart -Xlinker -soname=mixer.so $(SOURCES)
else

#
# Linux DLL

all: mixer.so

mixer.so: $(OBJECTS)
	@(mkdir ../../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -shared $^ $(LIBS)

endif
