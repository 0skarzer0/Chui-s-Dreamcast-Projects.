/*
 *  Fenix - Videogame compiler/interpreter
 *  Current release       : FENIX - PROJECT 1.0 - R 0.9
 *  Last stable release   : 
 *  Project documentation : http://fenix.divsite.net
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  Copyright © 1999 José Luis Cebrián Pagüe
 *  Copyright © 2002 Fenix Team
 *
 */

/** @file
 *
 *  Contains the CONTROL:INPUTLINE class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define BLINK_TIME 500

/** Time where the last blinking started */
static int blink_start = 0;

/** Draw a inputline.
 *  This is a member function of the CONTROL:INPUTLINE class
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 * */

void gui_inputline_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	INPUTLINE * inputline   = (INPUTLINE *)c ;
	REGION * text_clip;
	int scroll = inputline->scroll;
	int xpos = gr_text_widthn (0, inputline->text, inputline->cursor);
	int total = gr_text_width (0, inputline->text);

	/* Choose an scroll position */
	if (c->focused && xpos < scroll)
		scroll = xpos - 16;
	if (scroll < 0)
		scroll = 0;
	if (c->width - 5 > total)
		scroll = 0;
	if (c->focused && xpos-scroll > c->width-8)
		scroll = xpos - (c->width-8);
	xpos -= scroll;

	/* Draw the inputline */
	gr_setcolor (color_highlightface) ;
	gr_hline (dest, clip, x+1, y+c->height-1, c->width-1);
	gr_vline (dest, clip, x+c->width-1, y+1, c->height-1);
	if (c->focused)
	{
		gr_setcolor (color_border) ;
		gr_hline (dest, clip, x, y, c->width);
		gr_vline (dest, clip, x, y, c->height);
	}
	else
	{
		gr_setcolor (color_shadow) ;
		gr_hline (dest, clip, x, y, c->width);
		gr_vline (dest, clip, x, y, c->height);
	}
	gr_setcolor (color_input);
	gr_box (dest, clip, x+1, y+1, c->width-2, c->height-2);

	x += 4;
	y += 4 ;
	text_clip = region_new (x-1, y-2, c->width-5, c->height-2);
	if (clip) region_union (text_clip, clip);

	/* Draw the selection */
	if (inputline->selend != inputline->selstart)
	{
		int b = gr_text_widthn (0, inputline->text, inputline->selstart);
		int e = gr_text_widthn (0, inputline->text, inputline->selend);

		if (b > e) { int c = b; b = e; e = c; }

		gr_setcolor (color_selection);
		gr_box (dest, text_clip, x+b-scroll, y-2, e-b, c->height-3);
	}

	/* Draw the text */
	gr_text_setcolor (color_text);
	gr_text_put (dest, text_clip, 0, x-scroll, y, inputline->text) ;

	/* Draw the caret */
	if (c->focused && ((gr_timer() - blink_start) % BLINK_TIME) <= 2*BLINK_TIME/3)
	{
		gr_setcolor (color_shadow);
		gr_vline (dest, text_clip, x+xpos, y-1, c->height-4);
		gr_hline (dest, text_clip, x+xpos-1, y-2, 3);
		gr_hline (dest, text_clip, x+xpos-1, y+c->height-6, 3);
	}

	region_destroy (text_clip);
	inputline->scroll = scroll;
}

/** Calculate the character position at a given x coordinate
 *
 *  @param input	Pointer to the inputline control
 *  @param x		X pixel coordinate 
 *  @return			Number of character at the given position
 */

int gui_inputline_charat (INPUTLINE * input, int x)
{
	int n, pos = 0;

	x += input->scroll;

	for (n = 0 ; input->text[n] ; n++)
	{
		pos += gr_text_widthn (0, input->text+n, 1);
		if (pos > x)
			return n;
	}
	return n;
}

/** Receive a mouse move event
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 */

int gui_inputline_mousemove (CONTROL * c, int x, int y, int buttons)
{
	INPUTLINE * inputline = (INPUTLINE *)c;

	if (inputline->select)
	{
		inputline->selstart = inputline->selmouse;
		inputline->selend = gui_inputline_charat(inputline, x);
		inputline->cursor = inputline->selend;
		return 1;
	}
	return 0;
}

/** Receive a mouse button press event
 *  This is a member function of the CONTROL:INPUTLINE class
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 *  @param x		Mouse X coordinate relative to control position
 *  @param y		Mouse Y coordinate relative to control position
 *  @param b		Number of the inputline pressed or released
 *  @param pressed	1 if the inputline was pressed, 0 if it was released
 * */

int gui_inputline_mousebutton (CONTROL * c, int x, int y, int b, int pressed)
{
	INPUTLINE * inputline = (INPUTLINE*)c;

	if (pressed == 2)
	{
		inputline->cursor = strlen(inputline->text);
		inputline->selstart = 0;
		inputline->selend = inputline->cursor;
	}
	else if (pressed == 1)
	{
		inputline->selmouse = gui_inputline_charat(inputline, x);
		inputline->cursor = inputline->selmouse;
		inputline->selstart = inputline->cursor;
		inputline->selend = inputline->cursor;
		inputline->select = 1;
	}
	else
	{
		inputline->select = 0;
	}

	return 1;
}

/** The mouse leaves a inputline, change its pressed state.
 *  This is a member function of the CONTROL:INPUTLINE class
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 * */

int gui_inputline_mouseleave (CONTROL *c)
{
	return 1;
}

/** The user moves the focus to the inputline
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 *  @return			1 if the control is capable of focusing
 */

int gui_inputline_enter (CONTROL * c)
{
	INPUTLINE * input = (INPUTLINE *)c;

	input->selstart = 0;
	input->selend = strlen(input->text);
	input->cursor = input->selend;
	c->focused = 1;
	c->redraw = 1;
	return 1;
}

/** The user leaves the control
 *
 *  @param c		Pointer to the inputline.control member (at offset 0 of the inputline)
 */

int gui_inputline_leave (CONTROL * c)
{
	INPUTLINE * input = (INPUTLINE *)c;

	input->scroll = 0;
	input->selstart = 0;
	input->selend = 0;
	c->focused = 0;
	c->redraw = 1;
	return 1;
}

/** Inserts text in the input buffer
 *
 *  @param i			Pointer to the input object
 *  @param pos			Position to insert the text (negative to start from the end)
 *  @param text			Pointer to the caracters
 *  @param count		Number of characters to insert
 */

void gui_inputline_addtext (INPUTLINE * input, int pos, const char * text, int count)
{
	int len = strlen(input->text);

	if (pos < 0)
		pos = len+(pos+1);
	if (pos > len)
		pos = len;
	if (len+count+1 > input->size)
		count = input->size-(len+1);
	if (count < 1)
		return;

	memmove (input->text + pos + count, input->text + pos, len-pos+1);
	memmove (input->text + pos, text, count);

	if (input->cursor >= pos)
		input->cursor += count;
	if (input->selstart >= pos)
		input->selstart += count;
	if (input->selend >= pos)
		input->selend += count;

	input->control.actionflags |= ACTION_CHANGE;
}

/** Deletes some text from the input buffer
 * 
 *  @param i			Pointer to the input object
 *  @param pos			Position to insert the text (negative to start from the end)
 *  @param count		Number of characters to insert (0 to delete to EOL)
 */

void gui_inputline_deltext (INPUTLINE * input, int pos, int count)
{
	int len = strlen(input->text);

	if (count < 0)
	{
		pos += count;
		count = -count;
	}
	if (pos < 0)
	{
		count += pos;
		pos = 0;
	}
	if (pos > len)
		pos = len;
	if (count == 0 || pos + count >= len)
		count = len-pos;
	if (count < 1)
		return;

	memmove (input->text + pos, input->text + pos + count, len-(pos+count)+1);

	if (input->cursor >= pos)
	{
		input->cursor -= count;
		if (input->cursor < pos)
			input->cursor = pos;
	}
	if (input->selstart >= pos)
	{
		input->selstart -= count;
		if (input->selstart < pos)
			input->selstart = pos;
	}
	if (input->selend >= pos)
	{
		input->selend -= count;
		if (input->selend < pos)
			input->selend = pos;
	}

	input->control.actionflags |= ACTION_CHANGE;
}

/** The user presses a key
 *
 *  @param c			Pointer to the inputline.control member (at offset 0 of the inputline)
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_inputline_key (CONTROL * c, int scancode, int character)
{
	INPUTLINE * inputline = (INPUTLINE *)c;

	int    cursor     = inputline->cursor;
	int    old_cursor = inputline->cursor;
	char * text       = inputline->text;
	char   ch         = (char)character;

	if (gr_key(KEY_RALT))
		scancode &= ~KEYMOD_CONTROL;

	blink_start = gr_timer();

	switch (scancode & ~KEYMOD_SHIFT)
	{
		case KEY_RIGHT | KEYMOD_CONTROL:
			while (text[cursor] && text[cursor] != ' ') cursor++;
			while (text[cursor] && text[cursor] == ' ') cursor++;
			inputline->cursor = cursor;
			break;
		case KEY_LEFT | KEYMOD_CONTROL:
			if (cursor > 0 && text[cursor] != ' ' && text[cursor-1] == ' ') cursor--;
			while (cursor > 0 && text[cursor] == ' ') cursor--;
			while (cursor > 0 && text[cursor] != ' ') cursor--;
			if (text[cursor] == ' ') cursor++;
			inputline->cursor = cursor;
			break;
		case KEY_LEFT:
			inputline->cursor--;
			break;
		case KEY_RIGHT:
			inputline->cursor++;
			break;
		case KEY_HOME:
			inputline->cursor = 0;
			break;
		case KEY_END:
			inputline->cursor = strlen(inputline->text);
			break;
		case KEY_BACKSPACE:
			if (inputline->selstart != inputline->selend)
				gui_inputline_deltext (inputline, inputline->selstart, 
						inputline->selend - inputline->selstart);
			else if (inputline->cursor > 0)
				gui_inputline_deltext (inputline, inputline->cursor-1, 1);
			break;
		case KEY_BACKSPACE | KEYMOD_CONTROL:
			gui_inputline_deltext (inputline, 0, inputline->cursor);
			break;

		/* Seleccionar todo */
		case KEY_A | KEYMOD_CONTROL:
		case KEY_E | KEYMOD_CONTROL:
			inputline->selstart = 0;
			inputline->selend = strlen(inputline->text);
			inputline->cursor = old_cursor = inputline->selend;
			break;

#ifdef WIN32

		/* Windows Clipboard Support */
		case KEY_C | KEYMOD_CONTROL:
		case KEY_X | KEYMOD_CONTROL:
			if (inputline->selstart != inputline->selend && OpenClipboard (NULL))
			{
				char * ptr;
				int size = inputline->selend - inputline->selstart;
				HGLOBAL handle;

				if (size < 0) size = -size;
				EmptyClipboard ();
				handle = GlobalAlloc (GMEM_MOVEABLE, size+1);
				if (!handle) break;
				ptr = GlobalLock(handle);
				if (!ptr) break;
				if (inputline->selstart > inputline->selend)
					memcpy (ptr, inputline->text + inputline->selend, size);
				else
					memcpy (ptr, inputline->text + inputline->selstart, size);
				ptr[size] = 0;
				GlobalUnlock(handle);

				SetClipboardData (CF_TEXT, handle);

				if ((scancode & ~KEYMOD_SHIFT) == (KEY_X | KEYMOD_CONTROL))
					gui_inputline_deltext (inputline, inputline->selstart, 
							inputline->selend - inputline->selstart);

				CloseClipboard();
			}
			break;

		case KEY_V | KEYMOD_CONTROL:
			if (inputline->selstart != inputline->selend)
				gui_inputline_deltext (inputline, inputline->selstart, 
						inputline->selend - inputline->selstart);

			if (OpenClipboard(NULL))
			{
				HGLOBAL handle = GetClipboardData(CF_TEXT);
				char * ptr = GlobalLock(handle);
				gui_inputline_addtext (inputline, inputline->cursor, ptr, strlen(ptr));
				GlobalUnlock(ptr);
				CloseClipboard();
			}
			break;

#endif

		case KEY_DELETE:
			if (inputline->selstart != inputline->selend)
				gui_inputline_deltext (inputline, inputline->selstart, 
						inputline->selend - inputline->selstart);
			else
				gui_inputline_deltext (inputline, inputline->cursor, 1);
			break;

		case KEY_DELETE | KEYMOD_CONTROL:
			gui_inputline_deltext (inputline, inputline->cursor, 0);
			break;

		default:
			if (character > 31 && !(scancode & KEYMOD_CONTROL) && !gr_key(KEY_LALT))
			{
				if (inputline->selstart != inputline->selend)
				{
					gui_inputline_deltext (inputline, inputline->selstart, 
							inputline->selend - inputline->selstart);
					inputline->cursor = inputline->selstart;
				}
				gui_inputline_addtext (inputline, inputline->cursor, &ch, 1);
				inputline->lastcode = 0;
				c->redraw = 1;
				return 1;
			}
			return 0;
	}

	if (inputline->cursor < 0)
		inputline->cursor = 0;
	if (inputline->cursor > (int)strlen(inputline->text))
	    inputline->cursor = strlen(inputline->text);

	/* Select text with SHIFT and cursor moves */
	if (inputline->cursor != old_cursor )
	{
		c->redraw = 1;

		if (scancode & KEYMOD_SHIFT)
		{
			if (!(inputline->lastcode & KEYMOD_SHIFT))
				inputline->selstart = old_cursor;
			inputline->selend = inputline->cursor;
		}
		else
		{
			inputline->selend = inputline->selstart = inputline->cursor;
		}
	}
	inputline->lastcode = scancode;
	return 1;
}

/** Create a new inputline control 
 *
 *  @param x 		Left pixel coordinate in the window
 *  @param y 		Top pixel coordinate in the window
 *  @param text 	Label of the inputline
 *  @return 		Pointer to the control or 0 if not enough memory
 * */

CONTROL * gui_inputline_new (int width, char * text, int size)
{
	INPUTLINE * inputline ;

	/* Alloc memory for the struct */
	inputline = (INPUTLINE *) malloc(sizeof(INPUTLINE));
	if (inputline == NULL)
		return NULL;
	gui_control_init (&inputline->control);

	/* Fill the control control struct data members */
	inputline->control.bytes = sizeof(INPUTLINE);
	inputline->control.width = 8 + width*8 ;
	inputline->control.height = 14;
	inputline->control.innerdrag = 1;
	inputline->control.hresizable = 1;
	inputline->control.min_width = 16;
	inputline->control.pointer = POINTER_IBEAM;

	/* Fill the control control struct member functions */
	inputline->control.draw = gui_inputline_draw;
	inputline->control.mouseleave = gui_inputline_mouseleave;
	inputline->control.mousebutton = gui_inputline_mousebutton;
	inputline->control.mousemove = gui_inputline_mousemove;
	inputline->control.enter = gui_inputline_enter;
	inputline->control.leave = gui_inputline_leave;
	inputline->control.key = gui_inputline_key;

	/* Fill the rest of data members */
	inputline->text = text;
	inputline->size = size;
	inputline->cursor = 0;
	inputline->selstart = 0;
	inputline->selend = 0;
	inputline->scroll = 0;
	inputline->lastcode = 0;
	inputline->select = 0;

	return &inputline->control;
}

