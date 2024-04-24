/*
 * x11-calc-11.h - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2022    MT
 *
 * Model specific constants and function prototypes.
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
 * 30 Jan 22         - Initial version (derived from x11-calc-10.c) - MT
 * 09 Mar 22         - Fixed width and height (when scaled) - MT
 * 22 Apr 24         - Define display colour separately - MT
 *
 */

#define HEIGHT             276 * SCALE_HEIGHT /* 256 / 280 */
#define WIDTH              411 * SCALE_WIDTH  /* 381 / 411 */
#define BUTTONS            39
#define LABELS             1

#define DIGITS             11
#define INDECATORS         8

#define DIGIT_COLOUR       GREY
#define DIGIT_BACKGROUND   MID_GREY
#define DISPLAY_BACKGROUND MID_GREY
#define BEZEL_COLOUR       LIGHT_GREY

#define KBD_LEFT           12 * SCALE_WIDTH
#define KBD_TOP            59 * SCALE_HEIGHT
#define KBD_ROW            48 * SCALE_HEIGHT /* 43 / 48 */

#define KEY_HEIGHT         30 * SCALE_HEIGHT
#define KEY_WIDTH          36 * SCALE_WIDTH  /* 33 / 36 */
#define KEY_NUMERIC        41 * SCALE_WIDTH
#define KEY_GAP            3 * SCALE_WIDTH

#define BEZEL_LEFT         0 * SCALE_WIDTH
#define BEZEL_TOP          0 * SCALE_HEIGHT
#define BEZEL_WIDTH        411 * SCALE_WIDTH /* 381 / 411 */
#define BEZEL_HEIGHT       64 * SCALE_HEIGHT

#define DISPLAY_LEFT       KBD_LEFT + KEY_WIDTH + KEY_GAP
#define DISPLAY_TOP        8 * SCALE_HEIGHT
#define DISPLAY_WIDTH      200 * SCALE_WIDTH
#define DISPLAY_HEIGHT     48 * SCALE_HEIGHT

#define MEMORY_SIZE        256
#define ROM_SIZE           014000
#define CONTINIOUS

extern int i_rom [ROM_SIZE];

void v_init_buttons(obutton *h_button[]);

void v_init_labels(olabel *h_label[]);

