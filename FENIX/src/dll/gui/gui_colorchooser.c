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
 *  Contains the CONTROL:IMAGEPALETTE class member functions
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#define MULTIPLE_FILES
#include <fxdll.h>

#include "gui.h"

static GRAPH * ccm_rainbow = 0;

/** Converts an RGB magnitude to an HSL color.
  *
  *	@param h	Hue	(in degrees, 0 to 360)
  *	@param s	Saturation (percentile, 0 to 100)
  *	@param l	Luminance (percentile, 0 to 100)
  *	@param r	Pointer to variable to hold R value
  *	@param g	Pointer to variable to hold G value
  *	@param b	Pointer to variable to hold B value
  */

void rgb2hsl (int r, int g, int b, int *h, int *s, int *l)
{
	float fr, fg, fb, v, m, vm, r2, g2, b2;

	fr = r*100.0f/255.0f;
	fg = g*100.0f/255.0f;
	fb = b*100.0f/255.0f;

	v = (fr > fg ? fr : fg);
	v = (v > fb  ?  v : fb);
	m = (fr < fg ? fr : fg);
	m = (m < fb  ?  m : fb);

	*l = (int)( (m+v) / 2.0f );
	*s = (int)( 100.0f * (vm = v-m) / (*l <= 50 ? (v+m) : (200.0f-v-m)) );

	r2 = (v - fr) / vm;
	g2 = (v - fg) / vm;
	b2 = (v - fb) / vm;

	if (fr == v)
		*h = (int)( (fg == m ? 5.0f + b2 : 1.0f - g2)*60.0f );
	else if (fg == v)
		*h = (int)( (fb == m ? 1.0f + r2 : 3.0f - b2)*60.0f );
	else
		*h = (int)( (fr == m ? 3.0f + g2 : 5.0f - r2)*60.0f );
}

/** Converts an HSL magnitude to a RGB color.
  *
  *	@param h	Hue	(in degrees, 0 to 360)
  *	@param s	Saturation (percentile, 0 to 100)
  *	@param l	Luminance (percentile, 0 to 100)
  *	@param r	Pointer to variable to hold R value
  *	@param g	Pointer to variable to hold G value
  *	@param b	Pointer to variable to hold B value
  */

void hsl2rgb (int ih, int isl, int il, int *ir, int *ig, int *ib)
{
	float h = (ih % 360) / 360.0f;
	float sl = isl / 100.0f;
	float l = il / 100.0f;
	float r, g, b, v;

	v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);

	if (v <= 0)
		r = g = b = 0.0f;
	else
	{
		float m, sv, fract, vsf, mid1, mid2;
		int sextant;

		m = l + l - v;
		sv = (v - m) / v;
		h *= 6.0;
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant)
		{
			case 0: r = v; g = mid1; b = m; break;
			case 1: r = mid2; g = v; b = m; break;
			case 2: r = m; g = v; b = mid1; break;
			case 3: r = m; g = mid2; b = v; break;
			case 4: r = mid1; g = m; b = v; break;
			case 5: r = v; g = m; b = mid2; break;
		}
	}

	*ir = (int)(r * 255.0f);
	*ig = (int)(g * 255.0f);
	*ib = (int)(b * 255.0f);
}

/** Creates the colorchooser rainbow area to pick colors
 */

void gui_colorchooser_createrainbow()
{
	int x, y, h, s, l, r, g, b;
	
	ccm_rainbow = bitmap_new (0, 175, 80, 16);

	for (y = 0 ; y < 80 ; y++)
	{
		l = (int)(y * 1.25f) ;
		s = 100;

		for (x = 0 ; x < 175 ; x++)
		{
			h = x * 360/175;
			hsl2rgb (h, s, l, &r, &g, &b);
			gr_put_pixel (ccm_rainbow, x, y, gr_rgb(r,g,b));
		}
	}

	bitmap_add_cpoint (ccm_rainbow, 0, 0);
}

/** Updates a COLORCHOOSER control's text fields
 */

void gui_colorchooser_update (COLORCHOOSER * ch)
{
	int r, g, b;
	int h, s, l;
	int mr, mg, mb;

	if (ch->color)
	{
		ch->transparent = *ch->color == 0;
		ch->none        = *ch->color == -1;
			
		gr_get_rgb (0xFFFFFFFF, &mr, &mg, &mb);
		gr_get_rgb (*ch->color, &r, &g, &b);
		rgb2hsl (r, g, b, &h, &s, &l);

		r = r*255/mr;
		g = g*255/mg;
		b = b*255/mb;

		sprintf (ch->tr, "%d", r);
		sprintf (ch->tg, "%d", g);
		sprintf (ch->tb, "%d", b);
		sprintf (ch->th, "%d", h);
		sprintf (ch->ts, "%d", s);
		sprintf (ch->tl, "%d", l);

		ch->r = r;
		ch->g = g;
		ch->b = b;
		ch->h = h;
		ch->s = s;
		ch->l = l;
	}

	ch->previous_color = *ch->color;
}

/** Updates the color from the colorchooser's text areas
 */

void gui_colorchooser_updatecolor (COLORCHOOSER * ch)
{
	int r, g, b;
	int h, s, l;
	int mr, mg, mb;

	if (ch->transparent)
	{
		if (*ch->color == 0 && ch->none == 1)
			ch->transparent = 0;
		else
		{
			(*ch->color) = 0;
			ch->none = 0;
	 		return;
		}
	}
	if (ch->none)
	{
		(*ch->color) = -1;
		return;
	}
	if (*ch->color <= 0)
		(*ch->color) = gr_rgb (ch->r, ch->g, ch->b);

	if (atoi(ch->tr) != ch->r ||
		atoi(ch->tg) != ch->g ||
		atoi(ch->tb) != ch->b)
	{
		*ch->color = gr_rgb (atoi(ch->tr), atoi(ch->tg), atoi(ch->tb));
		gr_get_rgb (*ch->color, &r, &g, &b);
		rgb2hsl (r, g, b, &h, &s, &l);
		sprintf (ch->th, "%d", h);
		sprintf (ch->ts, "%d", s);
		sprintf (ch->tl, "%d", l);
		ch->h = h, ch->s = s, ch->l = l;
	}
	else if (atoi(ch->th) != ch->h ||
		atoi(ch->ts) != ch->s ||
		atoi(ch->tl) != ch->l)
	{
		hsl2rgb (atoi(ch->th), atoi(ch->ts), atoi(ch->tl), &r, &g, &b);
		*ch->color = gr_rgb(r, g, b);
		gr_get_rgb (*ch->color, &r, &g, &b);
		gr_get_rgb (0xFFFFFFFF, &mr, &mg, &mb);
		rgb2hsl (r, g, b, &h, &s, &l);
		r = r*255/mr;
		g = g*255/mg;
		b = b*255/mb;
		sprintf (ch->tr, "%d", r);
		sprintf (ch->tg, "%d", g);
		sprintf (ch->tb, "%d", b);	
		ch->r = r, ch->g = g, ch->b = b;
	}

	ch->previous_color = *ch->color;
}

/** Draw the COLORCHOOSER control. Most of the work is the parent's task.
 */

void gui_colorchooser_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	SDL_Color * palette;
	COLORCHOOSER * ch = (COLORCHOOSER *)c;
	int cx, cy;

	if (*ch->color != ch->previous_color)
		gui_colorchooser_update(ch);
	else
		gui_colorchooser_updatecolor(ch);

	if (ccm_rainbow == 0)
		gui_colorchooser_createrainbow();
	palette = ch->palette;
	if (!palette)
		palette = gpalette;

	gr_setcolor (color_face);
	gr_box (dest, clip, x, y, c->width, c->height);
	gr_setcolor (color_highlight);
	gr_hline (dest, clip, x, y, c->width);
	gr_vline (dest, clip, x, y, c->height);
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x, y+c->height-1, c->width);
	gr_vline (dest, clip, x+c->width-1, y, c->height);

	/* Draw the hue/luminance matrix */

	gr_blit (dest, clip, x+90, y+4, 128, ccm_rainbow);

	/* Draw the palette */

	if (ch->mode & CCM_ENABLEPALETTE)
	{
		for (cy = 0 ; cy < 8 ; cy++)
		{
			for (cx = 0 ; cx < 32 ; cx++)
			{
				SDL_Color * color = palette + (cy*32 + cx);
				gr_setcolor (gr_rgb(color->r, color->g, color->b));
				gr_box (dest, clip, x+90+cx*6, y+90+cy*8, 6, 8);
			}
		}
	}

	/* Draw the saturation bar */

	for (cy = 0 ; cy < 80 ; cy++)
	{
		int r, g, b;
		hsl2rgb (ch->h, cy*100/80, ch->l, &r, &g, &b);
		gr_setcolor (gr_rgb(r,g,b));
		gr_hline (dest, clip, x+270, y+cy+4, 11);
	}

	/* Draw the current color */

	cx = x + 90 + ch->h*175/360;
	cy = y + 4  + ch->l*80/100;
	gr_setcolor (gr_rgb(255,255,255));
	gr_hline (dest, clip, cx-4, cy-1, 9);
	gr_hline (dest, clip, cx-4, cy+1, 9);
	gr_vline (dest, clip, cx-1, cy-4, 9);
	gr_vline (dest, clip, cx+1, cy-4, 9);
	gr_setcolor (0);
	gr_hline (dest, clip, cx-4, cy, 9);
	gr_vline (dest, clip, cx, cy-4, 9);
	
	cy = y + 4 + ch->s*80/100;
	gr_hline (dest, clip, x+268, cy, 6);
	gr_hline (dest, clip, x+268, cy-1, 4);
	gr_hline (dest, clip, x+268, cy+1, 4);
	gr_hline (dest, clip, x+268, cy-2, 2);
	gr_hline (dest, clip, x+268, cy+2, 2);
	gr_setcolor (gr_rgb(255,255,255));
	gr_line (dest, clip, x+275, cy, -6, -3);
	gr_line (dest, clip, x+275, cy, -6,  3);

	gui_container_draw (c, dest, x, y, clip);
}

/** Response to mouse clicks
 */

int gui_colorchooser_mousebutton (CONTROL * c, int x, int y, int b, int p)
{
	COLORCHOOSER * ch = (COLORCHOOSER *)c;

	if (!ch->color) return 0;

	if (x >= 90 && x < 265 && y >= 4 && y < 84)
	{
		int r, g, b, h, s = 100, l;
		hsl2rgb (h = (x-90)*360/175, s, l = (y-4)*100/80, &r, &g, &b);
		*ch->color = gr_rgb(r, g, b);
		ch->r = r, ch->g = g, ch->b = b;
		gui_colorchooser_update(ch);
		ch->s = s, ch->h = h, ch->l = l;
		return 1;
	}
	if ((ch->mode & CCM_ENABLEPALETTE) && x >= 90 && x < 314 && y >= 90 && y < 154)
	{
		SDL_Color * palette = ch->palette ? ch->palette : gpalette;
		SDL_Color * color = palette + ((y-90)/8*32 + (x-90)/6);
		*ch->color = gr_rgb (color->r, color->g, color->b);
		gui_colorchooser_update(ch);
		return 1;
	}
	if (x >= 264 && x < 280 && y >= 4 && y < 84)
	{
		int r, g, b, h = ch->h, s, l = ch->l;
		hsl2rgb (h, s = (y-4)*100/80, l, &r, &g, &b);
		*ch->color = gr_rgb(r, g, b);
		ch->r = r, ch->g = g, ch->b = b;
		gui_colorchooser_update(ch);
		ch->s = s, ch->h = h, ch->l = l;
		return 1;
	}

	return gui_container_mousebutton (c, x, y, b, p);
}

/** Response to mouse drags and moves
 */

int gui_colorchooser_mousemove (CONTROL * c, int x, int y, int buttons)
{
	COLORCHOOSER * ch = (COLORCHOOSER *)c;

	if (buttons && x >= 90 && x < 265 && y >= 4 && y < 84)
	{
		gui_colorchooser_mousebutton (c, x, y, 0, 1);
		return 1;
	}
	if ((ch->mode & CCM_ENABLEPALETTE) && buttons && x >= 90 && x < 314 && y >= 90 && y < 154)
	{
		gui_colorchooser_mousebutton (c, x, y, 0, 1);
		return 1;
	}
	if (buttons && x >= 264 && x < 280 && y >= 4 && y < 84)
	{
		gui_colorchooser_mousebutton (c, x, y, 0, 1);
		return 1;
	}
	return gui_container_mousemove (c, x, y, buttons);
}

/** Return the appropiate mouse cursor for the control
 */

int gui_colorchooser_mousepointer (CONTROL * c, int x, int y)
{
	COLORCHOOSER * ch = (COLORCHOOSER *)c;

	if (x >= 90 && x < 265 && y >= 4 && y < 84)
		return POINTER_DROPPER;
	if ((ch->mode & CCM_ENABLEPALETTE) && x >= 90 && x < 314 && y >= 90 && y < 154)
		return POINTER_DROPPER;
	if (x >= 264 && x < 280 && y >= 4 && y < 84)
		return POINTER_DROPPER;

	return gui_container_mousepointer (c, x, y);
}

/** Callback function for OK button */

static void gui_colorchooser_ok (CONTROL * c)
{
	if (gui_desktop_ispopup(c))
		gui_desktop_removepopup(c);
	else
		gui_desktop_removewindow(c->window);
}

/** Creates a new COLORCHOOSER control, to edit a given color
 *
 *	@param color		Pointer to the color we're editing
 *	@param mode			Mode flags (See gui_colorchooser.h)
 */

CONTROL * gui_colorchooser_newc (int * color, int mode)
{
	COLORCHOOSER * ch = (COLORCHOOSER *) malloc(sizeof(COLORCHOOSER));
	CONTROL * c = &ch->container.control;
	CONTROL * button;
	int width, height;

	width = 286;
	height = 89;

	if (mode & CCM_ENABLEPALETTE)
		height += 70;
	if (mode & (CCM_ENABLETRANSPARENT | CCM_ENABLENONE))
		height += 15;
	
	ch->tr[0] = 0;
	ch->tg[0] = 0;
	ch->tb[0] = 0;
	ch->th[0] = 0;
	ch->ts[0] = 0;
	ch->tl[0] = 0;
	ch->mode = mode;
	ch->palette = 0;
	ch->color = color;
	ch->previous_color = *color;
	ch->transparent = (*color == 0);
	ch->none = (*color == -1);
	gui_colorchooser_update (ch);

	gui_container_init (&ch->container, width, height);
	gui_container_add (c,  4,   4, gui_label_new ("R"));
	gui_container_add (c,  4,  19, gui_label_new ("G"));
	gui_container_add (c,  4,  34, gui_label_new ("B"));
	gui_container_add (c, 20,   4, gui_inputline_new(6, ch->tr, 4));
	gui_container_add (c, 20,  19, gui_inputline_new(6, ch->tg, 4));
	gui_container_add (c, 20,  34, gui_inputline_new(6, ch->tb, 4));
	if (height > 110)
	{
		gui_container_add (c,  4,  59, gui_label_new ("H"));
		gui_container_add (c,  4,  74, gui_label_new ("S"));
		gui_container_add (c,  4,  89, gui_label_new ("L"));
		gui_container_add (c, 20,  59, gui_inputline_new(6, ch->th, 4));
		gui_container_add (c, 20,  74, gui_inputline_new(6, ch->ts, 4));
		gui_container_add (c, 20,  89, gui_inputline_new(6, ch->tl, 4));
	}
	if (mode & CCM_ENABLEOK)
	{
		gui_container_add (c,  4, height-26, button = gui_button_new ("&Ok"));
		button->width = 72;
		gui_button_callbackp (button, gui_colorchooser_ok, c);
	}

	if (mode & CCM_ENABLETRANSPARENT)
		gui_container_add (c, 90, height-15, gui_checkbox_new("Transparent", &ch->transparent));
	if (mode & CCM_ENABLENONE)
		gui_container_add (c, 220, height-15, gui_checkbox_new("None", &ch->none));

	c->draw = gui_colorchooser_draw;
	c->mousebutton = gui_colorchooser_mousebutton;
	c->mousemove = gui_colorchooser_mousemove;
	c->mousepointer = gui_colorchooser_mousepointer;

	return c;
}

/** Draws a clickable color */

void gui_clickablecolor_draw (CONTROL * c, GRAPH * dest, int x, int y, REGION * clip)
{
	CLICKABLECOLOR * cc = (CLICKABLECOLOR *)c;

	gr_setcolor (*cc->color);
	gr_box (dest, clip, x, y, c->width, c->height);
	gr_setcolor (c->highlight ? color_border : color_highlight);
	gr_hline (dest, clip, x, y, c->width);
	gr_vline (dest, clip, x, y, c->height);
	gr_setcolor (color_face);
	gr_hline (dest, clip, x+1, y+1, c->width-1);
	gr_vline (dest, clip, x+1, y+1, c->height-1);
	gr_setcolor (color_border);
	gr_hline (dest, clip, x+1, y+c->height-1, c->width-1);
	gr_vline (dest, clip, x+c->width-1, y+1, c->height-1);
	gr_setcolor (color_shadow);
	gr_hline (dest, clip, x, y+c->height-2, c->width-1);
	gr_vline (dest, clip, x+c->width-2, y, c->height-1);
}

int gui_clickablecolor_mousebutton (CONTROL * c, int x, int y, int b, int p)
{
	CLICKABLECOLOR * cc = (CLICKABLECOLOR *)c;

	if (p && b == 0)
	{
		gui_desktop_removepopup (0);
		gui_desktop_addpopup (c->window, gui_colorchooser_newc (cc->color, cc->mode), c->x - 150, c->y + c->height);
		return 1;
	}
	return 0;
}

int gui_clickablecolor_key (CONTROL *c, int key, int code)
{
	CLICKABLECOLOR * cc = (CLICKABLECOLOR *)c;

	if (key == KEY_SPACE || key == KEY_RETURN)
	{
		gui_desktop_removepopup (0);
		gui_desktop_addpopup (c->window, gui_colorchooser_newc (cc->color, cc->mode), c->x - 150, c->y + c->height);
		return 1;
	}
	return 0;
}

int gui_clickablecolor_enter (CONTROL *c)
{
	c->focused = 1;
	return 1;
}

int gui_clickablecolor_leave (CONTROL *c)
{
	c->focused = 0;
	return 1;
}

/** Creates a new CLICKABLECOLOR control, to edit a given color
 *
 *	@param color		Pointer to the color we're editing
 *	@param mode			Mode flags (See gui_colorchooser.h)
 */

CONTROL * gui_clickablecolor_new (int * color, int mode)
{
	CLICKABLECOLOR * cc = (CLICKABLECOLOR *) malloc(sizeof(CLICKABLECOLOR));

	gui_control_init (&cc->control);
	cc->control.width = 24;
	cc->control.height = 14;
	cc->color = color;
	cc->mode = mode | CCM_ENABLEOK;
	cc->control.enter = gui_clickablecolor_enter;
	cc->control.leave = gui_clickablecolor_leave;
	cc->control.draw = gui_clickablecolor_draw;
	cc->control.mousebutton = gui_clickablecolor_mousebutton;
	cc->control.key = gui_clickablecolor_key;
	return &cc->control;
}
