/*
 * x11-calc-display.h - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2013   MT
 *
 * 10 Digit digit display functions.
 *
 * Contains  the functions needed to create and display a 10 digit 7 seven
 * segment display element.
 *
 * This  program is free software: you can redistribute it and/or modify it
 * under  the terms of the GNU General Public License as published  by  the
 * Free  Software Foundation, either version 3 of the License, or (at  your
 * option) any later version.
 *
 * This  program  is distributed in the hope that it will  be  useful,  but
 * WITHOUT   ANY   WARRANTY;   without even   the   implied   warranty   of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You  should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 14 Jul 13         - Initial version - MT
 * 17 Aug 13         - Added constants for mask values - MT
 * 10 Mar 14         - Changed names of display masks to highlight their
 *                     association with the display module - MT
 * 30 Aug 20         - Base  the number of display segments on the  maximum
 *                     number of digits in the display - MT
 * 08 Aug 21         - Tidied up spelling errors in the comments - MT
 * 20 Dec 21         - Updated display for HP67 - MT
 * 29 Jan 22         - Added an optional bezel to the display - MT
 * 01 Mar 22         - Only define constants that are required - MT
 * 12 Mar 22         - Added display annunciators - MT
 * 11 Dec 22         - Renamed models with continious memory and added HP25
 *                     HP33E, and HP38E - MT
 *
 */

#include "x11-calc-cpu.h"

#define DISPLAY_SPACE      0x0000

#if !(defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c))

#define DISPLAY_ONE        0x0030
#define DISPLAY_TWO        0x006d
#define DISPLAY_THREE      0x0079
#define DISPLAY_FOUR       0x0072
#define DISPLAY_FIVE       0x005b
#define DISPLAY_SIX        0x005f
#define DISPLAY_SEVEN      0x0031
#define DISPLAY_EIGHT      0x007f
#define DISPLAY_NINE       0x0073
#define DISPLAY_ZERO       0x003f
#define DISPLAY_MINUS      0x0040
#define DISPLAY_DECIMAL    0x0080
#define DISPLAY_COMMA      0x0100
#define DISPLAY_COLON      0x0200
#define DISPLAY_r          0x0044
#define DISPLAY_c          0x004c
#define DISPLAY_C          0x000f
#define DISPLAY_o          0x005c
#define DISPLAY_d          0x007c
#define DISPLAY_P          0x0067
#define DISPLAY_E          0x004f
#define DISPLAY_TEST       0x01ff

#endif

typedef struct { /* Calculator display structure. */
   int index;
   osegment* segment[DIGITS];
   int left;
   int top;
   int width;
   int height;
   int display_left;
   int display_top;
   int display_width;
   int display_height;
   unsigned int foreground;
   unsigned int background;
   unsigned int fill;
   unsigned int border;
#if defined(INDECATORS)
   olabel* label[INDECATORS];
#endif
} odisplay;

odisplay *h_display_create(int i_index,
   int i_left, int i_top, int i_width, int i_height,
   int i_display_left, int i_display_top, int i_display_width, int i_display_height,
   unsigned int i_foreground, unsigned int i_background, unsigned int i_fill, unsigned int i_border);

int i_display_draw(Display* x_display, int x_application_window, int i_screen, odisplay *h_display);

int i_display_update(Display* x_display, int x_application_window, int i_screen, odisplay *h_display, oprocessor *h_processor);
