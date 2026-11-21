/*
 *  Fenix - Videogame compiler/interpreter
 *  Current release       : FENIX - PROJECT 1.0 - R 0.85
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

/*
 * FILE        : f_joystick.c
 * DESCRIPTION : FXI functions for joystick handling
 */

#include <SDL.h>
#include "fxi.h"

SDL_Joystick * selected_joystick;

/**
 *	NUM_JOY (int CD)
 *	Returns the number of joysticks present in the system
 **/

int fxi_joy_num (INSTANCE * my, int * params) 
{
	return SDL_NumJoysticks() ;
}

/**
 *	JOY_NAME (int JOY)
 *	Returns the name for a given joystick present in the system
 **/

int fxi_joy_name (INSTANCE * my, int * params) 
{
	int result;
	
	if (params[0] < 0 || params[0] >= SDL_NumJoysticks())
		return 0;

	result = string_new(SDL_JoystickName(params[0]));
	string_use(result);
	return result;
}


/**
 *	JOY_SELECT (int JOY)
 *	Returns the selected joystick number
 **/

int fxi_joy_select (INSTANCE * my, int * params)
{
	if (params[0] >= 0 && params[0]<SDL_NumJoysticks()) {
		// Close previous one
		if (selected_joystick!=NULL) SDL_JoystickClose(selected_joystick) ;
		// Open new
		selected_joystick = SDL_JoystickOpen(params[0]) ;
		if (selected_joystick) {
			SDL_JoystickUpdate() ;
			return params[0] ;
			}
		}		
	return -1 ;
}

/**
 *	JOY_BUTTONS (int JOY)
 *	Returns the selected joystick total buttons
 **/

int fxi_joy_buttons (INSTANCE * my, int * params)
{
	if (selected_joystick==NULL)
		return -1 ;
	else
		return SDL_JoystickNumButtons(selected_joystick) ;		
}


/**
 *	JOY_AXIS (int JOY)
 *	Returns the selected joystick total axes
 **/

int fxi_joy_axes (INSTANCE * my, int * params)
{
	if (selected_joystick==NULL) {
		return -1 ;
	} else {
		return SDL_JoystickNumAxes(selected_joystick) ;		
	}
}

/**
 *	JOY_GET_BUTTON (int JOY)
 *	Returns the selected joystick state for the given button
 **/

int fxi_joy_get_button (INSTANCE * my, int * params)
{
	if (selected_joystick==NULL || params[0]<0 || params[0]>SDL_JoystickNumButtons(selected_joystick))
		return -1 ;
	else
		return SDL_JoystickGetButton(selected_joystick,params[0]) ;
	
}

/**
 *	JOY_GET_POSITION (int JOY)
 *	Returns the selected joystick state for the given axis
 **/

int fxi_joy_get_position (INSTANCE * my, int * params)
{
	if (selected_joystick==NULL || params[0]<0 || params[0]>SDL_JoystickNumAxes(selected_joystick))
		return 0 ;
	else
		return SDL_JoystickGetAxis(selected_joystick,params[0]) ;
		
}



