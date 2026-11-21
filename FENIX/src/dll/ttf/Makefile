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
target = MAC

# BeOS DLL
#
ifeq ($(target),BeOS)
CFLAGS += -IFreetype/include -I../../include -I../../fxi/inc $$(sdl-config --cflags)
LDTTF = -L/boot/home/config/lib -lfreetype

../../bin/lib/ttf.so: ttf.c
	@(mkdir ../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -nostart -Xlinker -soname=ttf.so $(LDTTF) ttf.c 
endif

