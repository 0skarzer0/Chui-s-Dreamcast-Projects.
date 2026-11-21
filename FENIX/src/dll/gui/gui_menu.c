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
 *  Contains the CONTROL:MENU class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"


#define MENU_PADDING_LEFT	12
#define MENU_PADDING_RIGHT	12

void gui_menu_closesubmenu (MENU * menu);
void gui_menu_opensubmenu (MENU * menu, int option);

/** Given a menu, return the next option with the given key as & code
 *
 *	@param menu		Pointer to the menu object
 *	@param first	Current option (searchs starts with the next one)
 *	@param c		Character to find
 *	@returns		Number of option or -1 if none found
 */

int gui_menu_optionkey (MENU * menu, int first, char c)
{
	int i;

	if (menu->count < 1)
		return -1;
	if (first < 0)
		first = menu->count-1;

	i = first+1;
	if (i == menu->count)
		i = 0;

	for (;;)
	{
		const char * ptr = menu->item[i].text;
		while (*ptr)
		{
			if (*ptr == '&' && toupper(ptr[1]) == toupper(c))
				return i;
			ptr++;
		}

		if (i == first)
			break;

		i++;
		if (i == menu->count)
			i = 0;
	}

	/* No matches found with the & prefix, return the first menu option
	 * beginning with the given letter, if any */

	i++;
	if (i == menu->count)
		i = 0;

	for (;;)
	{
		const char * ptr = menu->item[i].text;
		if (toupper(*ptr) == toupper(c))
			return i;

		if (i == first)
			break;

		i++;
		if (i == menu->count)
			i = 0;
	}

	/* No matches found */

	return -1;
}

/** Draw the shadow of a control. The shadow is currently hard-coded as 3 pixels wide.
 *	Note that the shadow is traslucent: the desktop below the control MUST be drawn
 *	under the shadow area before calling this function.
 *
 *	@param c		Control pointer
 *	@param dest		Destination bitmap
 *	@param clip		Clipping region (not required)
 */

void gui_control_draw_shadow (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	/* Draw a shadow */
	gr_setcolor (0);
	gr_setalpha (128);
	gr_hline    (dest, clip, x+6, y+c->height,   c->width-6);
	gr_vline    (dest, clip, x+c->width, y+6, c->height-5);
	gr_setalpha (90);
	gr_hline    (dest, clip, x+5, y+c->height,   1);
	gr_hline    (dest, clip, x+5, y+c->height+1, c->width-4);
	gr_vline    (dest, clip, x+c->width, y+5, 1);
	gr_vline    (dest, clip, x+c->width+1,	y+5, c->height-3);
	gr_setalpha (32);
	gr_hline    (dest, clip, x+4, y+c->height,   1);
	gr_hline    (dest, clip, x+4, y+c->height+1, 1);
	gr_hline    (dest, clip, x+4, y+c->height+2, c->width-2);
	gr_vline    (dest, clip, x+c->width, y+4, 1);
	gr_vline    (dest, clip, x+c->width+1, y+4, 1);
	gr_vline    (dest, clip, x+c->width+2,	y+4, c->height-1);
	gr_setalpha (255);
}

/** Draw a menu
 *  This is a member function of the CONTROL:MENU class
 *
 *  @param c		Pointer to the object
 *  @param dest		Destination drawing bitmap
 *  @param x		Left destination position in the bitmap
 *  @param y		Top destination position in the bitmap
 *  @param clip		Clipping region to apply or 0 if none
 **/

void gui_menu_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	MENU * menu = (MENU *)c;
	int option;
	int width = menu->control.width;
	int height = menu->control.height;

	gui_control_draw_shadow (c, dest, x, y, clip);

	/* Handle item selection */
	if (menu->selectedtime > 0 && menu->selectedtime <= gr_timer()-150)
	{
		int option = menu->selected;

		if (menu->item[option].type == MENUITEM_SUBMENU)
			gui_menu_opensubmenu(menu, option);
		else
			gui_menu_closesubmenu(menu);
		menu->selectedtime = 0;
	}

	/* Draw a border */
	gr_setcolor (color_highlight);
	gr_hline (dest, clip, x, y, width);
	gr_vline (dest, clip, x, y, height);
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x, y+height-1, width);
	gr_vline (dest, clip, x+width-1, y, height);
	width -= 2, height -= 2;
	x++, y++;
	gr_setcolor (color_face);
	gr_box (dest, clip, x, y, width, height);
	width -= 4, height -= 4;
	x += 2, y += 2;

	/* Draw the options */
	for (option = 0 ; option < menu->count ; option++)
	{
		/* Paint the selected option */
		if (menu->item[option].enabled && menu->selected == option)
		{
			gr_setcolor (color_highlightface);
			gr_box (dest, clip, x, y, width, menu->item[option].height);
			gr_setcolor (!menu->pressed ? color_highlight : color_shadow);
			gr_hline (dest, clip, x, y, width);
			gr_vline (dest, clip, x, y, menu->item[option].height);
			gr_setcolor (!menu->pressed ? color_shadow : color_highlight);
			gr_hline (dest, clip, x, y+menu->item[option].height-1, width);
			gr_vline (dest, clip, x+width-1, y, menu->item[option].height);
		}

		if (menu->pressed && menu->item[option].enabled && menu->selected == option)
			(*menu->item[option].draw)(&menu->item[option], dest, clip, x+1, y+1, menu->control.width);
		else
			(*menu->item[option].draw)(&menu->item[option], dest, clip, x, y, menu->control.width);
		y += menu->item[option].height;
	}
}

/**	Draws a menu separator
 *
 *	@param item		Pointer to this menu item
 *	@param dest		Pointer to the destination graphic
 *	@param clip		Pointer to the clipping region
 *	@param x		X Screen of the top-left drawing point
 *	@param y		Y Screen of the top-left drawing point
 *	@param width	Width of the menu that contains the separator (without borders)
 **/

static void draw_separator (MENUITEM * item, GRAPH * dest, REGION * clip, 
							int x, int y, int width)
{
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x+2, y+4, width-10);
	gr_setcolor (color_highlight);
	gr_hline (dest, clip, x+2, y+5, width-10);
}

/**	Draws a normal, text menu item (including triggers and radio options)
 *
 *	@param item		Pointer to this menu item
 *	@param dest		Pointer to the destination graphic
 *	@param clip		Pointer to the clipping region
 *	@param x		X Screen of the top-left drawing point
 *	@param y		Y Screen of the top-left drawing point
 *	@param width	Width of the menu that contains the separator (without borders)
 **/

static void draw_action (MENUITEM * item, GRAPH * dest, REGION * clip, 
						 int x, int y, int width)
{
	char * tab = strchr(item->text, '\t');

	y += 2;

	if (item->enabled == 0)
	{
		gr_text_setcolor (color_shadow);
		gr_setcolor (color_shadow);
	}
	else
	{
		gr_text_setcolor (color_text);
		gr_setcolor (color_text);
	}

	if (tab)
	{
		*tab = 0;
		gui_text_put (dest, clip, x + MENU_PADDING_LEFT, y+1, item->text, 0, 0);
		gui_text_put (dest, clip, x + width - MENU_PADDING_RIGHT - 1, y+1, tab+1, 0, 0);
		*tab = '\t';
	}
	else
	{
		gui_text_put (dest, clip, x + MENU_PADDING_LEFT, y+1, item->text, 0, 0);
	}

	/* If the item is a toggle, draw the check mark if needed */
	if (item->type == MENUITEM_TOGGLE && item->data && (*(int *)item->data & 1))
	{
		int height = item->height-4;
		int bwidth = 12;

		x -= 2;
		y --;
		gr_setcolor (color_text);
		gr_line (dest, clip, x+4, y+height-5, 1, 2);
		gr_line (dest, clip, x+5, y+height-3, bwidth-8, -height+6);
		gr_line (dest, clip, x+bwidth-3, y+3, 2, 0);
	}

	/* If the item is a radio, draw the circle if needed */
	if (item->type == MENUITEM_RADIO && item->data && *(int *)item->data == item->value)
	{
		int height = item->height/2-3;

		gr_setcolor (color_text);
		gr_fcircle (dest, clip, x+5, y+height, 2);
	}

	/* If the item is a submenu, draw the arrow */
	if (item->type == MENUITEM_SUBMENU)
	{
		int height = item->height-3, i;

		gr_setcolor (color_text);
		for (i = 2 ; i <= height/2 ; i++)
			gr_vline (dest, clip, x+width-8-height+i, y+i-1, height-2*i);
	}
}

/** Create a separator menu item
 *
 *	@return			The object itself (by value)
 **/

MENUITEM gui_menuitem_separator()
{
	MENUITEM item;

	item.type = MENUITEM_SEPARATOR;
	item.text[0] = 0;
	item.draw = draw_separator;
	item.enabled = 0;
	item.height = 8;
	item.width = 12;
	item.callback = 0;
	return item;
}

/** Create a normal ("action") menu item
 *
 *	@param text		Name of the option. It may contain \t to separate the
 *					name from the shortcut, and \b to mark option letter(s)
 *	@return			The object itself (by value)
 **/

MENUITEM gui_menuitem (const char * text)
{
	MENUITEM item;

	item.type = MENUITEM_ACTION;
	strncpy (item.text, text, MENUITEM_TEXT_MAX);
	item.text[MENUITEM_TEXT_MAX-1] = 0;
	item.draw = draw_action;
	item.enabled = 1;
	item.height = 6 + gr_text_height(0, item.text);
	item.width = MENU_PADDING_LEFT + 1 + gr_text_width(0, item.text) + MENU_PADDING_RIGHT;
	item.callback = 0;
	return item;
}

/** Create a toggle-type menu item. This is an item that toggles a state
 *	variable and contains a check mark when it is activated (!= 0)
 *
 *	@param text		Name of the option. It may contain \t to separate the
 *					name from the shortcut, and \b to mark option letter(s)
 *  @param value	Pointer to the state variable of this option
 *	@return			The object itself (by value)
 **/

MENUITEM gui_menuitem_toggle (const char * text, int * value)
{
	MENUITEM item = gui_menuitem(text);

	item.type = MENUITEM_TOGGLE;
	item.data = value;
	item.callback = 0;
	return item;
}

/** Create a radio-type menu item. This is an item that changes a state
 *	variable setting it to a given value when clicked. A circle (o) is
 *  drawn if the variable already contains this value.
 *
 *	@param text		Name of the option. It may contain \t to separate the
 *					name from the shortcut, and \b to mark option letter(s)
 *  @param var		Pointer to the state variable of this option
 *	@param value	Value assigned to this option
 *	@return			The object itself (by value)
 **/

MENUITEM gui_menuitem_radio (const char * text, int * var, int value)
{
	MENUITEM item = gui_menuitem(text);

	item.type = MENUITEM_RADIO;
	item.data = var;
	item.value = value;
	item.callback = 0;
	return item;
}

/** Create a submenu entry. The submenu control will be created and destroyed
 *  automatically, but the data must remain available.
 *
 *	@param text		Name of the option. It may contain \t to separate the
 *					name from the shortcut, and \b to mark option letter(s)
 *	@param options	Pointer to an array of MENUITEM structs
 *	@param count	Number of entries in the previous array
 *	@return			The object itself (by value)
 **/

MENUITEM gui_menuitem_submenu (const char * text, MENUITEM * options, int count)
{
	MENUITEM item = gui_menuitem (text);

	item.type = MENUITEM_SUBMENU;
	item.data = options;
	item.value = count;
	item.callback = 0;
	return item;
}
/** Calculate the height (in pixels) of a menu. It sums the heights of
 *  all the options and adds some space for borders and padding.
 *
 *	@param menu		Pointer to the menu object
 **/

int gui_menu_height (MENU * menu)
{
	int option, height = 0;

	for (option = 0 ; option < menu->count ; option++)
		height += menu->item[option].height;
	return height + 6;
}

/** Calculates the width (in pixels) of a menu. It equals the maximum
 *	option width of the menu, plus some space for borders and padding.
 *
 *	@param menu		Pointer to the menu object
 **/

int gui_menu_width (MENU * menu)
{
	int option, width = 0;

	for (option = 0 ; option < menu->count ; option++)
		if (width < menu->item[option].width)
			width = menu->item[option].width;
	return width + 6;
}

/** Close a submenu, if any opened
 *
 *	@param menu			Pointer to the menu object
 **/

void gui_menu_closesubmenu (MENU * menu)
{
	if (menu->submenu != NULL)
	{
		((MENU*)menu->submenu)->parentmenu = NULL;
		gui_desktop_removepopup(&menu->submenu->control);
		menu->submenu = NULL;
		menu->submenuitem = -1;
	}

}

/** Open a submenu 
 *
 *	@param menu			Pointer to the menu object
 *	@param option		Number of menu item with the submenu
 **/

void gui_menu_opensubmenu (MENU * menu, int option)
{
	int x = menu->control.width + menu->control.x;
	int y = menu->control.y;
	int i;

	if (menu->submenu != NULL)
	{
		/* The menu is already opened */
		if (menu->submenuitem == option)
			return;

		/* Close any other opened submenu */
		((MENU*)menu->submenu)->parentmenu = NULL;
		gui_desktop_removepopup(&menu->submenu->control);
		menu->submenu = NULL;
		menu->submenuitem = -1;
	}

	/* Calculate the Y coordinate of the option */
	for (i = 0 ; i < menu->count ; i++)
	{
		if (i == option) break;
		y += menu->item[i].height;
	}

	/* Create and show the popup */
	if (menu->count > option && menu->item[option].type == MENUITEM_SUBMENU)
	{
		menu->submenu = (MENU *)gui_menu_new ((MENUITEM *)menu->item[option].data,
			menu->item[option].value);
		gui_desktop_addpopup (menu->control.window, &menu->submenu->control, x, y);
		menu->submenuitem = option;
		menu->submenu->parentmenu = menu;
		menu->submenu->parentmenuitem = option;
	}
}

/** Respond to a mouse-move event in a menu
 *
 *	@param c			Pointer to the menu object
 *	@param x			X local coordinate of the mouse
 *	@param y			Y local coordinate of the mouse
 *	@param buttons		Bit mask with the state of all mouse buttons
 **/

int gui_menu_mousemove (CONTROL * c, int x, int y, int buttons)
{
	MENU * menu = (MENU *)c;
	int cy = 2;
	int option;

	menu->pressed = (buttons & 1);

	for (option = 0 ; option < menu->count ; option++)
	{
		cy += menu->item[option].height;
		if (y <= cy) 
		{
			if (menu->item[option].enabled)
			{
				menu->selected = option;
				menu->selectedtime = gr_timer();
				if (menu->parentmenu)
				{
					menu->parentmenu->selected = menu->parentmenuitem;
					menu->parentmenu->selectedtime = 0;
				}
			}
			break;
		}
	}
	if (option == menu->count)
		menu->selected = -1;
	return 1;
}

/** Responde to a mouse button press or release event in a menu
 *
 *	@param c			Pointer to the menu object
 *	@param x			X local coordinate of the mouse
 *	@param y			Y local coordinate of the mouse
 *	@param b			Index of the button
 *	@param p			0 = Button released; 1 = Button pressed; 2+ = double click
 **/

int gui_menu_mousebutton (CONTROL * c, int x, int y, int b, int p)
{
	MENU * menu = (MENU *)c;

	if (b == 0 && p < 2)
		menu->pressed = p;

	if (p == 0 && menu->selected != -1)
	{
		MENUITEM * item = &menu->item[menu->selected];

		if (item->type == MENUITEM_TOGGLE && item->data)
			*(int *)item->data = ~*(int *)item->data;
		if (item->type == MENUITEM_RADIO && item->data)
			*(int *)item->data = item->value;
		if (item->callback)
			(*item->callback)(item->callbackparam[0], item->callbackparam[1]);
		if (item->type != MENUITEM_SUBMENU && item->type != MENUITEM_SEPARATOR)
			gui_desktop_removepopup(c);
		return 1;
	}
	if (p == 1 && menu->selected != -1)
	{
		MENUITEM * item = &menu->item[menu->selected];

		if (item->type == MENUITEM_SUBMENU)
			gui_menu_opensubmenu(menu, menu->selected);
		return 1;
	}
	return 0;
}

/** Response to a mouse leaving a menu
 *
 *	@param menu			Pointer to the menu object
 **/

int gui_menu_mouseleave (CONTROL * menu)
{
	((MENU *)menu)->selected = -1;
	return 1;

}
/** The user presses a key
 *
 *  @param c			Pointer to the menu object
 *  @param scancode		Keyboard scan code of the key pressed
 *  @param character	ASCII code of the character or 0 if N/A 
 */

int gui_menu_key (CONTROL * c, int scancode, int character)
{
	MENU * menu = (MENU *)c;
	int old_selected = menu->selected;
	MENUITEM * item;

	switch (scancode)
	{
		case KEY_HOME:
			menu->selected = -2;
		case KEY_DOWN:
			for (;;)
			{
				menu->selected++;
				if (menu->selected == old_selected)
					break;
				if (menu->selected < 0)
					continue;
				if (menu->selected == menu->count)
				{
					menu->selected = -2;
					continue;
				}
				if (menu->item[menu->selected].type != MENUITEM_SEPARATOR)
					break;
			}
			break;
		case KEY_END:
			menu->selected = menu->count;
		case KEY_UP:
			for (;;)
			{
				menu->selected--;
				if (menu->selected == old_selected)
					break;
				if (menu->selected < 0)
				{
					menu->selected = menu->count;
					continue;
				}
				if (menu->item[menu->selected].type != MENUITEM_SEPARATOR)
					break;
			}
			break;
		case KEY_LEFT:
			if (menu->lefttool && menu->lefttool->window)
			{
				(*menu->lefttool->mousebutton)(menu->lefttool, 0, 0, 1, 1);
				gui_desktop_removepopup(c);
				return 1;
			}
			if (menu->parentmenu == NULL)
				break;
			menu->parentmenu->selected = menu->parentmenuitem;
			menu->parentmenu->submenu = NULL;
			menu->parentmenu = NULL;
			gui_desktop_removepopup(c);
			return 1;
		case KEY_RIGHT:
			if ((menu->selected == -1 || menu->item[menu->selected].type != MENUITEM_SUBMENU)
				&& menu->righttool && menu->righttool->window)
			{
				(*menu->righttool->mousebutton)(menu->righttool, 0, 0, 1, 1);
				gui_desktop_removepopup(c);
				return 1;
			}
			if (menu->selected == -1 || menu->item[menu->selected].type != MENUITEM_SUBMENU)
			{
				if (menu->parentmenu)
				{
					if (menu->parentmenu->righttool)
						menu->parentmenu->selected = -1;
					return gui_menu_key ((CONTROL *) menu->parentmenu, scancode, character);
				}
				break;
			}
		default:
			if (scancode != KEY_RIGHT)
			{
				if (!(scancode & (KEYMOD_CONTROL | KEYMOD_ALT)))
				{
					int option = gui_menu_optionkey (menu, menu->selected, character);
					if (option != -1)
					{
						/* If there is only one option with the key, act like
						* if the user pressed key + ENTER */
						if (gui_menu_optionkey (menu, option, character) == option)
							menu->selected = option;
						else
						{
							menu->selected = option;
							break;
						}
					}
				}
				else break;
			}
		case KEY_RETURN:
			if (menu->selected == -1) break;
			item = &menu->item[menu->selected];
			if (item->type == MENUITEM_TOGGLE && item->data)
				*(int *)item->data = !*(int *)item->data;
			if (item->type == MENUITEM_RADIO && item->data)
				*(int *)item->data = item->value;
			if (item->type == MENUITEM_SUBMENU)
			{
				gui_menu_opensubmenu(menu, menu->selected);
				menu->submenu->selected = 0;
				return 1;
			}
			else 
			{
				if (item->callback)
					(*item->callback)(item->callbackparam[0], item->callbackparam[1]);
				gui_desktop_removepopup(c);
			}
			break;
	}

	if (menu->selected != old_selected)
	{
		return 1;
	}
	return 0;
}

/** Menu destructor
 *
 *	@param menu			Pointer to the menu object
 **/

void gui_menu_destructor (CONTROL * control)
{
	MENU * menu = (MENU *)control;
	MENU * child = (MENU *)menu->submenu;
	MENU * parent = (MENU *)menu->parentmenu;

	if (child)
	{
		menu->submenu = NULL;
		child->parentmenu = NULL;
		gui_desktop_removepopup(&child->control);
	}
	if (parent)
	{
		menu->parentmenu = NULL;
		parent->submenu = NULL;
		gui_desktop_removepopup(&parent->control);
	}
}

/** Initializes a menu struct. NOTE: the menu struct has a VARIABLE size.
 *  Calling this function with a MENU struct of base size will corrupt
 *	your memory. Use gui_menu_new always instead.
 *
 *	@param menu			Pointer to the MENU struct
 *	@param options		Pointer to an array of options to copy to the menu
 *	@param count		Number of options in the array
 **/

void gui_menu_init (MENU * menu, MENUITEM * options, int count)
{
	gui_control_init (&menu->control);

	/* Initialize data members */
	memcpy (menu->item, options, sizeof(MENUITEM) * count);
	menu->count = count;
	menu->selected = -1;
	menu->selectedtime = 0;
	menu->submenu = NULL;
	menu->submenuitem = -1;
	menu->parentmenu = NULL;
	menu->parentmenuitem = -1;
	menu->caller = NULL;
	menu->pressed = 0;
	menu->lefttool = 0;
	menu->righttool = 0;

	menu->control.bytes = sizeof(MENU) + sizeof(MENUITEM) * (count-1);
	menu->control.width = gui_menu_width (menu);
	menu->control.height = gui_menu_height (menu);

	/* Initialize virtual functions */
	menu->control.draw = gui_menu_draw;
	menu->control.mousemove = gui_menu_mousemove; 
	menu->control.mousebutton = gui_menu_mousebutton;
	menu->control.mouseleave = gui_menu_mouseleave;
	menu->control.key = gui_menu_key;
	menu->control.destructor = gui_menu_destructor;
}

/** Creates a menu
 *
 *	@param options		Pointer to an array of options to copy to the menu
 *	@param count		Number of options in the array
 *	@return				Pointer to the new menu control
 **/

CONTROL * gui_menu_new (MENUITEM * options, int count)
{
	MENU * menu;

	/* Alloc memory for the struct */
	menu = (MENU *) malloc(sizeof(MENU) + sizeof(MENUITEM) * (count-1));
	if (menu == NULL)
		return NULL;

	gui_menu_init (menu, options, count);
	return &menu->control;
}

/** Put the menu in a menu-bar context
 *
 *	@param control		Pointer to the menu
 *	@param left			Pointer to the menu or control to receive focus when the user presses LEFT
 *	@param right		Pointer to the menu or control to receive focus when the user presses RIGHT
 */

void gui_menu_context (CONTROL * control, CONTROL * left, CONTROL * right)
{
	MENU * menu = (MENU *)control;

	menu->lefttool = left;
	menu->righttool = right;
}
