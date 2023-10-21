/*
 * x11-calc-70.c - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2018   MT
 *
 * Model specific functions.
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
 * 04 Jan 22         - Initial version (50th anniversary) - MT
 * 11 Jan 22         - Removed ROM_BANKS - MT
 * 12 Feb 22         - Updated layout and separated the initialisation code
 *                     for buttons and switches into two functions - MT
 *                   - Added keyboard shortcuts for 'n' and 'i' - MT
 * 27 Nov 22         - Specify button style when creating buttons (the code
 *                     to handle a 'flat' button was there all along!) - MT
 * 21 Oct 23         - Updated switch parameters to accomodate a 3 position
 *                     switch - MT
 *
 */

#define VERSION        "0.1"
#define BUILD          "0004"
#define DATE           "12 Feb 22"
#define AUTHOR         "MT"

#include <stdarg.h>    /* strlen(), etc. */
#include <stdio.h>     /* fprintf(), etc. */
#include <stdlib.h>    /* getenv(), etc. */

#include <X11/Xlib.h>  /* XOpenDisplay(), etc. */
#include <X11/Xutil.h> /* XSizeHints etc. */

#include "x11-calc-font.h"
#include "x11-calc-button.h"
#include "x11-calc-switch.h"
#include "x11-calc-label.h"
#include "x11-calc-colour.h"

#include "x11-calc-cpu.h"

#include "x11-calc.h"

#include "gcc-debug.h"

oregister o_mem[MEMORY_SIZE];

void v_init_switches(oswitch *h_switch[]) /* Define the switches. */
{
   h_switch[0] = h_switch_create(00000, "OFF", "", "ON ", h_alternate_font, KBD_LEFT, KBD_TOP, 2 * KEY_WIDTH + KEY_GAP, SWITCH_HEIGHT, True, MID_GREY, DARK_GREY);
   h_switch[1] = NULL;
}

void v_init_buttons(obutton *h_button[]) {
   int i_left, i_top, i_count = 0;

   /* Define top row of keys. */
   i_top = KBD_TOP + 2* (SWITCH_HEIGHT + 1); /* Assumes the function label and switch text use the same font */
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00006, 'n', "n", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00004, 'i', "i", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00003, 000, "PMT", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00002, 000, "PV", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00000, 000, "FV", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define second row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00056, 000, "INT", "" , "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00054, 000, "%", "" , "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00053, 000, "d%", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, YELLOW, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00052, 000, "yX", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00050, 000, "CLR", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define third row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00016, 000, "X-Y", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00014, 000, "Rv", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00013, 000, "STO", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00012, 000, "K", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00010, 000, "DSP", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define fourth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00076, 015, "ENTER", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, 2 * KEY_WIDTH + KEY_GAP, KEY_HEIGHT, False, True, ORANGE, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (2 * (KEY_WIDTH + KEY_GAP));
   h_button[i_count++] = h_button_create(00073, 'c', "CHS", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, ORANGE, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00072, 000, "M", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00070, 033, "M +", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define fifth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00066, '-', "-", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, ORANGE, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00064, '7', "7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00063, '8', "8", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00062, '9', "9", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define sixth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00026, '+', "+", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, ORANGE, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00024, '4', "4", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00023, '5', "5", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00022, '6', "6", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define seventh row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00036, '*', "\xd7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, ORANGE, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00034, '1', "1", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00033, '2', "2", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00032, '3', "3", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define bottom row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00046, '/', "\xf7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, ORANGE, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00044, '0', "0", "", "", "", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00043, '.', ".", "", "", "", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00042, 000, "CLX", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
}

int i_rom[ROM_SIZE] = {
   01431, 00420, 00420, 00420, 00564, 00007, 00764, 00043,
   00620, 01641, 01751, 01671, 00015, 00041, 00146, 00107,
   00267, 00376, 01656, 00316, 01731, 01656, 01360, 00316,
   01741, 01656, 00316, 01751, 00021, 00316, 01745, 00662,
   01633, 00752, 00752, 00005, 01356, 01776, 01316, 00011,
   00041, 00316, 01731, 00005, 00703, 00316, 01741, 01656,
   00316, 01745, 00257, 01641, 01751, 01671, 00015, 00041,
   00146, 00357, 00527, 00316, 01731, 01656, 01360, 00316,
   01735, 01656, 00316, 01751, 00021, 00316, 01745, 00662,
   01633, 00752, 00752, 00005, 00316, 00776, 01116, 00015,
   00041, 00316, 01731, 00005, 00703, 00316, 01735, 00277,
   01641, 01735, 01656, 00316, 01741, 00662, 01633, 00005,
   00041, 00662, 00703, 00316, 01731, 01656, 01360, 00316,
   01751, 00662, 01633, 01671, 00015, 00041, 00733, 00000,
   00646, 00723, 00364, 00723, 00316, 00713, 00316, 01731,
   01656, 00005, 00703, 00000, 00000, 00000, 01641, 01755,
   01656, 00316, 01745, 00021, 00316, 01735, 00011, 00656,
   00703, 00316, 01735, 00662, 01633, 01656, 00316, 01745,
   00662, 01633, 00005, 00376, 01656, 00316, 01731, 01656,
   01360, 00316, 01755, 00662, 01633, 00376, 01360, 01547,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   01641, 01755, 00662, 01633, 01356, 01742, 00005, 00316,
   01721, 01656, 01360, 00316, 01735, 01656, 00316, 01741,
   00662, 01633, 00005, 00316, 01721, 01656, 00031, 00316,
   00742, 00011, 00752, 00752, 00000, 00703, 00620, 01641,
   01755, 00662, 01633, 00316, 01741, 00662, 01633, 01656,
   00316, 01745, 00662, 01633, 00005, 00316, 01731, 01656,
   01360, 01564, 00047, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00564, 00727,
   00250, 01414, 00104, 00404, 00316, 00060, 00552, 00552,
   01356, 01776, 01316, 00060, 00742, 00742, 00742, 00742,
   00742, 00742, 00742, 00742, 00742, 01160, 01370, 00060,
   00227, 01653, 00303, 00307, 00000, 01020, 01020, 00000,
   00204, 00477, 00443, 00417, 01450, 00723, 00650, 00450,
   01656, 00723, 01756, 01756, 01756, 00153, 01671, 01747,
   00000, 00000, 01756, 01756, 01756, 00223, 01671, 01717,
   00000, 00000, 00407, 00314, 00453, 01020, 01671, 01727,
   00764, 01603, 00527, 01707, 01364, 00047, 01164, 01337,
   01020, 01020, 01756, 01756, 01756, 00113, 01671, 01737,
   01764, 01013, 00433, 00376, 00244, 00477, 00450, 01201,
   00733, 01175, 00733, 01204, 00244, 00503, 01764, 01057,
   01764, 01077, 00224, 00467, 00653, 01244, 01767, 01244,
   01316, 01023, 00000, 00000, 00000, 00304, 01656, 00316,
   01130, 01414, 01160, 01656, 01360, 00650, 01656, 00764,
   00043, 00000, 01324, 00633, 01564, 01763, 00424, 01773,
   00164, 01777, 00416, 00416, 01454, 00677, 00707, 01356,
   01552, 00250, 01652, 00250, 01201, 00704, 01021, 01366,
   00724, 00757, 00450, 00704, 00316, 00056, 01056, 00556,
   00276, 00776, 01056, 01277, 00636, 01044, 01103, 00772,
   01004, 00524, 01073, 00752, 01037, 00050, 00024, 01043,
   00044, 00034, 01454, 01107, 01050, 01024, 01067, 00416,
   00544, 00320, 00322, 00562, 00332, 01612, 01203, 00316,
   00064, 00616, 01414, 00452, 00612, 00672, 01253, 00252,
   00572, 01153, 01652, 00420, 01222, 01535, 01021, 01326,
   00636, 01454, 01377, 00216, 00756, 00114, 00422, 00074,
   00642, 01333, 00624, 01407, 00034, 00254, 01263, 00604,
   01267, 01222, 01146, 01437, 01535, 01021, 00767, 00354,
   01267, 01535, 01205, 01314, 00062, 01021, 01454, 01513,
   00752, 01451, 01205, 01314, 00062, 01021, 01511, 01352,
   01656, 00616, 01514, 00426, 00552, 01176, 01627, 01166,
   01557, 00312, 00060, 00034, 00752, 00002, 01617, 01326,
   01646, 00060, 01050, 00324, 00613, 00723, 00304, 00650,
   00060, 01364, 00107, 00564, 00007, 00564, 00013, 00564,
   00017, 00564, 00023, 00742, 01160, 00060, 01220, 00000,
   00220, 00433, 00443, 00376, 00056, 01772, 01772, 00772,
   00772, 00112, 00063, 01656, 01646, 00646, 00103, 01656,
   01046, 00112, 00143, 01216, 01752, 00016, 00143, 00107,
   00572, 00572, 01352, 01536, 01176, 00177, 00243, 01006,
   00217, 00376, 01456, 01416, 00000, 00273, 01616, 00542,
   00233, 01316, 00074, 01554, 00237, 00752, 01376, 01414,
   00056, 01142, 00343, 00416, 00552, 01156, 00307, 00316,
   00452, 01616, 01176, 00247, 01646, 00616, 00056, 01414,
   01124, 00003, 00624, 00643, 00644, 00653, 00314, 00252,
   00056, 00512, 00536, 00467, 00276, 01446, 01356, 01454,
   00237, 00146, 00553, 00124, 00727, 00322, 00562, 00332,
   00000, 00367, 01062, 01646, 00573, 00742, 01426, 00567,
   01626, 00426, 00034, 00054, 00573, 00616, 00212, 00273,
   00764, 01003, 00764, 01427, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00731, 00250, 00414,
   00642, 01127, 00050, 00302, 00250, 01250, 01114, 00142,
   01047, 00316, 00776, 00776, 01116, 01160, 01370, 00376,
   01360, 00316, 01356, 00776, 00776, 00776, 01116, 01160,
   01370, 01656, 01360, 00050, 01133, 00250, 00064, 00504,
   00316, 00364, 01207, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00616, 01250, 01656,
   01377, 01326, 01752, 01307, 01514, 01472, 00472, 00034,
   00254, 01517, 01452, 01572, 01377, 01356, 01552, 00056,
   01004, 00114, 00472, 00606, 00426, 01142, 01323, 01044,
   01614, 00612, 00172, 01307, 00034, 00254, 01503, 01367,
   01552, 01463, 01327, 01572, 01337, 00442, 00034, 01362,
   00612, 01626, 01577, 01326, 01776, 01024, 01603, 00074,
   01326, 00066, 01456, 01742, 01742, 00426, 01572, 01627,
   01362, 01562, 01326, 01024, 01757, 00002, 01703, 01723,
   01542, 00074, 01454, 01667, 01052, 01352, 00672, 01753,
   00252, 00572, 01052, 01456, 00220, 00000, 00000, 00000,
   00564, 00433, 00564, 00237, 00564, 00273, 00564, 00727,
   01104, 01356, 00604, 01506, 00033, 01316, 00576, 00033,
   00776, 00456, 00123, 01322, 01576, 00117, 01376, 01616,
   00624, 00327, 01542, 00103, 01462, 00422, 01636, 01407,
   00714, 00233, 00776, 01416, 00213, 01616, 01056, 00316,
   00546, 00430, 00746, 01116, 00554, 00433, 00614, 01362,
   01514, 01056, 01656, 00630, 00363, 01742, 00456, 00576,
   00123, 01322, 01656, 00426, 01656, 01576, 00333, 01456,
   01742, 00023, 01126, 00416, 00000, 00217, 01054, 00553,
   00514, 00330, 00330, 01014, 01056, 00624, 00413, 01316,
   00074, 00074, 00523, 01616, 00576, 00517, 01656, 00426,
   01656, 00233, 01154, 00623, 00714, 00330, 00330, 00030,
   01030, 00530, 01114, 00463, 01254, 00707, 01114, 00330,
   00130, 00030, 00130, 00730, 01130, 01030, 00130, 01214,
   00463, 01354, 01003, 00630, 01130, 00330, 00130, 00430,
   00730, 00130, 01030, 00030, 00530, 00630, 01314, 00463,
   01454, 00463, 00316, 00230, 00330, 00030, 00230, 00530,
   01030, 00530, 00030, 01130, 00330, 01414, 01656, 00624,
   01213, 00516, 00032, 01127, 00516, 01456, 00034, 00416,
   00154, 01133, 01656, 00676, 01173, 00346, 00752, 00050,
   01314, 00013, 00456, 00606, 01272, 01327, 00772, 01316,
   00752, 01237, 00676, 01277, 01456, 01416, 00356, 01316,
   01144, 00233, 01752, 01172, 01377, 01426, 01313, 01626,
   00416, 00552, 01317, 01316, 00322, 01652, 01253, 01746,
   01357, 01476, 01126, 01776, 00107, 01370, 00003, 01050,
   00064, 01650, 00056, 00772, 00772, 00250, 01414, 01160,
   01642, 01360, 01642, 00742, 01477, 00316, 01414, 00742,
   01160, 00742, 01116, 00742, 00752, 01360, 00316, 01723,
   00316, 01356, 01745, 01745, 01761, 01745, 01761, 01745,
   01761, 01745, 01761, 01745, 01761, 00316, 00450, 00450,
   00450, 00250, 00306, 00250, 00364, 00723, 00000, 00000,
   00000, 00742, 01160, 00060, 01656, 01360, 01656, 00060,
   00000, 00000, 00000, 00000, 01364, 01033, 00103, 00224,
   00337, 01765, 01735, 01656, 00450, 01370, 00164, 00703,
   00224, 00427, 01765, 01731, 00057, 00224, 00153, 01765,
   01721, 00057, 01611, 00176, 01143, 01447, 01114, 00142,
   00213, 00742, 01755, 01721, 00513, 00000, 00224, 00533,
   01765, 01715, 00057, 01114, 01505, 00676, 00023, 01364,
   01243, 00127, 00224, 00713, 01765, 01725, 00057, 01611,
   00176, 00623, 01414, 00142, 00413, 01471, 01414, 00142,
   00413, 00742, 01755, 01735, 00513, 01611, 00176, 01227,
   01314, 00142, 00503, 01471, 01314, 00142, 00503, 00742,
   01755, 01731, 01656, 01360, 00364, 00377, 01611, 00176,
   01057, 01014, 00142, 00607, 01471, 01014, 00142, 00607,
   00742, 01755, 01715, 00513, 01414, 00142, 00413, 01571,
   01114, 01505, 00676, 01003, 01014, 01505, 00676, 01017,
   00164, 00543, 01611, 00176, 01027, 01214, 00142, 00767,
   01471, 01214, 00142, 00767, 00742, 01755, 01725, 00513,
   00164, 00317, 00000, 00164, 00047, 01214, 00142, 00767,
   01571, 01551, 00257, 01014, 00142, 00607, 01571, 01551,
   01114, 01505, 00676, 01133, 01364, 00233, 01364, 00337,
   01114, 00142, 00213, 01571, 01551, 01014, 01505, 00676,
   01217, 01364, 00543, 01364, 00653, 01314, 00142, 00503,
   01551, 01114, 01505, 00676, 01317, 01014, 01505, 00676,
   01327, 00164, 01243, 00164, 00773, 00164, 01437, 01244,
   00250, 01571, 01551, 01114, 01505, 00676, 01413, 01755,
   01764, 00047, 01014, 01505, 00676, 01537, 01204, 01377,
   00000, 01114, 00142, 00213, 01471, 00173, 00314, 00742,
   00060, 00336, 00642, 01527, 00776, 00060, 00000, 00250,
   00564, 00727, 01414, 00642, 01537, 00060, 01314, 00642,
   01537, 00060, 00250, 00336, 00314, 00642, 01703, 00542,
   00642, 01677, 00542, 00642, 01673, 00776, 00742, 00742,
   00060, 00742, 00742, 00742, 00742, 00742, 00742, 00742,
   00742, 01160, 00060, 00250, 01414, 01656, 00316, 00060,
   00000, 00420, 00420, 00420, 00564, 00007, 00764, 00043,
   00620, 00650, 01656, 00450, 00021, 00552, 00552, 00164,
   00703, 00650, 01656, 00450, 00011, 01450, 00450, 00552,
   00552, 00005, 00077, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 01645, 01751,
   01671, 00015, 00316, 01755, 01056, 00316, 01721, 01056,
   01360, 01656, 00031, 00316, 01741, 00021, 00077, 01645,
   01751, 00662, 00513, 01671, 00015, 00316, 01755, 01056,
   00316, 01721, 01056, 01360, 01656, 00031, 00316, 00742,
   00011, 00316, 01745, 00021, 00316, 01751, 00552, 00552,
   00005, 00077, 01755, 01656, 00316, 01745, 00021, 00077,
   01645, 01751, 01671, 00015, 00316, 01755, 00376, 01056,
   00316, 01721, 01056, 01360, 01656, 00031, 00316, 01735,
   00021, 00077, 01645, 01751, 00662, 00513, 01671, 00015,
   00316, 01755, 00376, 01056, 00316, 01721, 01056, 01360,
   01656, 00031, 01356, 01742, 00011, 00316, 01745, 00021,
   00316, 01751, 00552, 00552, 00005, 00077, 01645, 01751,
   00662, 01517, 01671, 00015, 00316, 01755, 01056, 00316,
   01721, 00216, 01360, 01656, 00031, 00316, 00776, 01116,
   00011, 00316, 01731, 01656, 01360, 00316, 01751, 01671,
   01656, 00316, 01735, 00021, 00316, 01731, 00005, 00077,
   01645, 01755, 00376, 01656, 00316, 01721, 01656, 01360,
   00316, 01751, 00662, 01473, 01671, 00015, 00316, 01721,
   01656, 00031, 01356, 01776, 01316, 00011, 00316, 01731,
   01656, 01360, 00316, 01751, 00552, 00552, 01656, 00316,
   01741, 00021, 00316, 01731, 00005, 00077, 01741, 01656,
   00316, 01755, 01463, 01735, 01477, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00250, 00104, 01414, 00316, 00060, 00552, 00552,
   01356, 01776, 01316, 00060, 00742, 00742, 00742, 00742,
   00742, 00742, 00742, 00742, 00742, 01160, 01370, 00060,
   00000, 00420, 00420, 00420, 00564, 00007, 00764, 00043,
   00620, 01625, 01244, 00776, 00742, 01262, 01116, 01656,
   00316, 01705, 00021, 00316, 01731, 01656, 00005, 00316,
   01675, 01656, 01360, 00672, 00473, 00616, 00015, 00672,
   00343, 00316, 01731, 01356, 01742, 00005, 00376, 01656,
   00316, 01675, 01656, 01360, 00316, 01705, 01656, 00316,
   01675, 01656, 00336, 00031, 00316, 00742, 00011, 00577,
   00316, 01731, 01656, 00316, 01705, 00011, 00015, 00316,
   01675, 01656, 01360, 01656, 00316, 01731, 00005, 00752,
   00752, 00752, 00672, 00523, 01204, 00523, 01356, 00316,
   01742, 01705, 00005, 00577, 00316, 01731, 01356, 01742,
   00015, 01370, 00021, 00316, 01675, 01656, 00005, 00316,
   01725, 01656, 01360, 01224, 00633, 01347, 01244, 01356,
   00316, 01742, 01725, 00656, 01347, 00015, 00316, 01731,
   00376, 01056, 00316, 01675, 01056, 01360, 01656, 00031,
   00316, 01701, 01656, 01360, 00316, 01356, 01742, 01725,
   00015, 00316, 01675, 01656, 01360, 00316, 01731, 01656,
   00316, 01701, 00021, 00316, 01725, 00021, 00316, 01675,
   00005, 01360, 01356, 00316, 01742, 01701, 00011, 01360,
   00316, 01675, 00011, 01360, 00316, 01705, 01656, 00316,
   01725, 00021, 00316, 01701, 00011, 00316, 01675, 00005,
   01360, 01356, 01742, 00011, 00316, 01725, 00021, 01360,
   00316, 01675, 00021, 01356, 01535, 01370, 01356, 01545,
   00637, 00250, 00414, 00302, 00250, 01250, 01114, 00142,
   01427, 00316, 01731, 00376, 01360, 00316, 01356, 01725,
   01656, 01360, 01656, 00752, 00752, 00000, 00164, 00703,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 01752,
   01752, 01752, 01752, 01752, 01752, 01752, 00646, 01347,
   00672, 01623, 01712, 01347, 00060, 00250, 00414, 00130,
   00250, 01414, 01304, 00444, 00104, 00316, 00060, 00776,
   00776, 00776, 00776, 00776, 00776, 00776, 00776, 00776,
   01116, 01160, 01370, 00060, 00424, 01777, 01620, 00060,
   00000, 00420, 00420, 00420, 00564, 00007, 00764, 00043,
   00620, 01635, 01751, 01656, 00316, 00330, 00630, 00752,
   00752, 01414, 00005, 00316, 01224, 00143, 01731, 00147,
   01735, 00021, 01056, 00316, 01745, 01665, 01456, 00021,
   00316, 01725, 01656, 01360, 00316, 01356, 00330, 00630,
   00752, 00752, 00616, 00530, 01414, 00005, 00316, 01725,
   00021, 00450, 00316, 01735, 00450, 00316, 01725, 01224,
   00733, 00316, 01721, 01450, 01450, 01360, 00316, 01731,
   01656, 00316, 01224, 00433, 01725, 00437, 01721, 00011,
   00316, 01751, 00021, 00316, 01715, 01656, 01360, 00316,
   00330, 00630, 01224, 00527, 00533, 00530, 00014, 00430,
   01414, 01656, 00316, 01224, 00577, 01725, 00603, 01721,
   00021, 00316, 01715, 00005, 01224, 00663, 00316, 01741,
   01656, 01360, 01244, 00373, 00450, 00316, 01721, 00450,
   00316, 01741, 00450, 00316, 01360, 01725, 00164, 00703,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00224, 01027, 01057, 01656, 01635, 01761,
   00015, 01551, 01127, 01656, 00316, 01761, 01113, 01656,
   00316, 01755, 01656, 01224, 01143, 01360, 00364, 00377,
   00450, 01370, 00733, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00000, 00000, 00000, 00000,
   00000, 00000, 00000, 00000, 00322, 00562, 00332, 01612,
   01553, 00316, 00064, 00616, 01414, 00452, 00612, 00672,
   01623, 00252, 00572, 01523, 01652, 00060, 00250, 00316,
   01414, 00104, 00404, 01304, 00060, 00552, 00552, 01356,
   01776, 01316, 00060, 00776, 00776, 00776, 00776, 00776,
   00776, 00776, 00776, 00776, 01116, 01160, 01370, 00060
};
