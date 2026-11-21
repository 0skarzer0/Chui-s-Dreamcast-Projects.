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

debug = true
target = linux

ifeq ($(target) ,linux)
CFLAGS += -Wall -DTARGET_linux
endif

ifeq ($(target) ,mac)
CFLAGS += -Wall -O2 -g
endif

# Usar debug=true para activar versión de depurado

ifeq ($(debug), true)
    CFLAGS += -DDEBUG -g -Wall
else
    CFLAGS += -O2 
endif

# Usar efence=true para activar la librería Electric Fence

ifeq ($(efence), true)
    LIBS += -lefence
endif

# Usar target=win32 para compilar el .EXE de Windows

ifeq ($(target), win32)
    EXE = .exe
    CFLAGS += -I/win32/include
else
    EXE = 
endif

HEADERS = ../include/files.h ../include/xctype.h ../include/pslang.h 
CFLAGS += -DTARGET_$(target) -I../include -I../libungif/lib

# Utilidades

all: ../bin/map$(EXE)

../bin/map$(EXE):	map.o
	$(CC) $^ -o $@ $(LDFLAGS) -L../libungif/lib/.libs -lungif -lX11 -lpng -lz $(LIBS)


map.o:	map.c $(HEADERS)

# Limpieza/empaquetado/etc.

clean:
	rm -f *.o core  ../bin/map

