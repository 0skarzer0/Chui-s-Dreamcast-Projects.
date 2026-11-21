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

OBJECTS = gui_button.o gui.o gui_checkbox.o gui_clipboard.o gui_colorchooser.o gui_container.o gui_control.o gui_dropdown.o gui_frame.o gui_imagedraw.o gui_imageeditor.o gui_imagepalette.o gui_images.o gui_imagetools.o gui_imageview.o gui_inputline.o gui_label.o gui_main.o gui_menu.o gui_palettepanel.o gui_radio.o gui_scrollable.o gui_scrollbar.o gui_table.o gui_taskbar.o gui_toolbar.o gui_tool.o gui_window.o ef_slice.o ef_depth.o gui_tabs.o
SOURCES = gui_button.c gui.c gui_checkbox.c gui_clipboard.c gui_colorchooser.c gui_container.c gui_control.c gui_dropdown.c gui_frame.c gui_imagedraw.c gui_imageeditor.c gui_imagepalette.c gui_images.c gui_imagetools.c gui_imageview.c gui_inputline.c gui_label.c gui_main.c gui_menu.c gui_palettepanel.c gui_radio.c gui_scrollable.c gui_scrollbar.c gui_table.c gui_taskbar.c gui_toolbar.c gui_tool.c gui_window.c ef_slice.c ef_depth.c gui_tabs.c
HEADERS = gui_button.h gui_checkbox.h gui_colorchooser.h gui_container.h gui_control.h gui_dropdown.h gui_frame.h gui.h gui_imageeditor.h gui_imageview.h gui_inputline.h gui_label.h gui_menu.h gui_palettepanel.h gui_radio.h gui_scrollable.h gui_scrollbar.h gui_table.h gui_taskbar.h gui_toolbar.h gui_tool.h gui_window.h gui_tabs.h

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
    LIBS += -lmingw32 -lpng -lz -L$(SDL)/lib -lSDL -mwindows
else
ifeq ($(target), BeOS)
    CFLAGS += `sdl-config --cflags` 
    LIBS += `sdl-config --libs` -lpng /boot/home/config/lib/libSDL_mixer.a -lpng -lz -ldl
else
ifeq ($(target), linuxPPC)
    CFLAGS += `sdl-config --cflags` -DTARGET_linux
    LIBS += `sdl-config --libs` -lpng -lm -lSDL_mixer -lvorbisfile -lvorbis
else
    CFLAGS += `sdl-config --cflags` 
    LIBS += `sdl-config --libs` -lpng -lm -lSDL_mixer -lvorbisfile -lvorbis
  endif
endif
endif

# Redefine implicit rules

%.o: %.c $(HEADERS)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

# BeOS DLL
#
ifeq ($(target),BeOS)
CFLAGS += -I../../include -I../../fxi/inc $$(sdl-config --cflags)

../../bin/lib/gui.so: $(OBJECTS)
	@(mkdir ../../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -nostart -Xlinker -soname=gui.so $(SOURCES)
else

#
# Linux DLL

all: gui.so

gui.so: $(OBJECTS)
	@(mkdir ../../bin/lib || true) 2> /dev/null
	gcc $(CFLAGS) -o $@ -shared $^ $(LIBS)

endif

