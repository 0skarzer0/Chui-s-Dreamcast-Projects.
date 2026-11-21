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

# Default options
ifndef target
error:
	@echo "ERROR: Debe compilar con make target=N"
	@echo "       (N puede ser MAC, linux o WIN32)"
endif

debug = true
SDL = /usr/local/include/SDL

CFLAGS += -DTARGET_$(target) 
CFLAGS += -I../include -I../fxi/inc

ifeq ($(target), win32)
    TARGET = ../../bin/fxi.exe
    CFLAGS += -I/win32/include -I$(SDL)/include -I$(SDL)/include/SDL -D_WINDOWS
    LIBS += -lmingw32 -lpng -lz -L$(SDL)/lib -lSDL -mwindows
else
ifeq ($(target), BeOS)
    TARGET = ../../bin/fxi
    CFLAGS += `sdl-config --cflags` 
    LIBS += `sdl-config --libs` -lpng /boot/home/config/lib/libSDL_mixer.a -lpng -lz -ldl
else
    TARGET = ../../bin/fxi
    CFLAGS += `sdl-config --cflags` -L../FreeType/include
    LIBS += `sdl-config --libs` -lpng -lm -lSDL_mixer -lvorbisfile -lvorbis
  endif
endif
# BeOS DLL
#
ifeq ($(target),BeOS)
CFLAGS += -I../include -I../fxi/inc $$(sdl-config --cflags)

../bin/lib/hello.so: hello.c
	@(mkdir ../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -nostart -Xlinker -soname=hello.so hello.c 
else

#
# Linux DLL

all: hello.so hello2.so

hello.so: hello.c
	@(mkdir ../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -shared $<

hello2.so: hello2.c
	@(mkdir ../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -shared $<
endif

