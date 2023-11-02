/*
 * x11-calc-80.c - RPN (Reverse Polish) calculator simulator.
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
 * 04 Jan 22         - Initial version - MT
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
#define BUILD          "0003"
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
   h_button[i_count++] = h_button_create(00036, 'n', "n", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00034, 'i', "i", "YTM", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00033, 000, "PMT", "INTR", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00032, 000, "PV", "BOND", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00030, 000, "FV", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, BLACK, YELLOW, BACKGROUND, BACKGROUND);

   /* Define second row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00056, 'f', "", "" , "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, YELLOW, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00054, 000, "%", "d%", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00053, 000, "TL", "      ", "COMPUTE", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, YELLOW, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00052, 000, "SOD", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00050, 000, "DAY", "DATE", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, BACKGROUND, BACKGROUND);

   /* Define third row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00016, 000, "X-Y", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00014, 000, "Rv", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00013, 000, "STO", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00012, 000, "yX", "/\xaf", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00010, 000, "x", "- E", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, DIM_GREY, YELLOW, BACKGROUND, BACKGROUND);

   /* Define fourth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00076, 015, "SAVE", "      ", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, 2* KEY_WIDTH + KEY_GAP, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (2 * (KEY_WIDTH + KEY_GAP));
   h_button[i_count++] = h_button_create(00073, 000, "RCL", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00072, 'c', "CHS", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[i_count++] = h_button_create(00070, 033, "CLX", "CLEAR", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, True, MID_GREY, YELLOW, BACKGROUND, BACKGROUND);

   /* Define fifth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00066, '-', "-", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, MID_GREY, YELLOW, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00064, '7', "7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00063, '8', "8", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00062, '9', "9", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define sixth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00026, '+', "+", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00024, '4', "4", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00023, '5', "5", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00022, '6', "6", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define seventh row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00006, '*', "\xd7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00004, '1', "1", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00003, '2', "2", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00002, '3', "3", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GREY, BACKGROUND, BACKGROUND, BACKGROUND);

   /* Define bottom row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[i_count++] = h_button_create(00046, '/', "\xf7", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_SMALL, KEY_HEIGHT, False, True, MID_GREY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_SMALL + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00044, '0', "0", "", "", "", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00043, '.', ".", "", "", "", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
   i_left += (KEY_NUMERIC + 3 * KEY_GAP);
   h_button[i_count++] = h_button_create(00042, 000, "E+", "E-", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, True, LIGHT_GRAY, BACKGROUND, BACKGROUND, BACKGROUND);
}
int i_rom[ROM_SIZE] = {
01311, 00563, 01752, 01752, 01752, 00223, 00650, 00220,
01450, 01203, 00220, 01767, 01450, 00477, 00650, 00207,
00000, 00000, 01752, 01752, 01752, 00013, 00650, 00220,
01304, 00173, 00173, 01204, 00173, 00060, 00724, 00303,
00620, 00220, 00220, 00504, 00220, 00000, 00650, 00220,
01420, 00000, 01020, 01020, 00650, 00220, 00704, 00413,
00704, 00477, 01752, 01752, 01752, 00113, 00376, 00131,
00316, 00421, 01733, 01024, 00463, 00467, 00450, 01344,
01244, 00744, 00444, 00503, 00724, 01073, 00424, 00447,
01073, 00450, 00450, 00373, 00450, 01250, 00744, 00404,
01044, 00616, 00456, 01414, 00326, 00742, 00742, 00672,
00777, 00352, 00672, 01107, 00504, 01632, 00607, 00316,
00501, 00316, 00562, 00332, 01676, 00503, 00444, 01144,
01414, 00044, 00034, 01454, 00653, 01050, 01124, 00747,
00544, 00412, 00320, 01104, 00114, 00524, 00753, 00762,
00717, 00050, 00024, 00717, 00647, 00452, 01227, 01171,
01352, 01752, 00412, 01456, 01012, 00767, 00452, 01552,
01157, 00354, 01133, 00220, 00034, 01047, 00744, 00444,
00507, 00612, 01171, 01752, 01552, 01147, 00552, 01063,
01057, 01306, 01121, 00034, 01106, 01035, 00220, 00060,
00220, 01032, 01227, 01752, 01572, 00312, 00314, 01454,
01707, 01056, 00644, 00635, 01366, 00424, 01303, 00450,
00644, 01647, 01650, 00064, 00204, 00447, 01752, 00006,
01633, 00354, 01377, 00316, 00776, 00776, 01514, 01122,
00006, 01457, 01414, 01142, 01457, 01352, 01552, 01142,
01457, 00406, 01433, 00452, 01056, 01656, 00544, 00631,
01056, 00312, 00766, 00014, 00542, 00074, 00542, 01553,
00422, 01527, 01452, 00456, 00524, 01603, 00604, 01457,
00624, 01333, 00034, 00254, 01377, 01403, 01116, 01056,
00631, 00316, 00056, 01514, 00330, 01044, 00552, 01052,
01517, 01142, 01247, 00542, 00074, 01237, 00376, 00636,
00612, 00637, 00000, 00000, 00000, 00250, 01250, 00413,
00000, 00000, 00104, 00333, 00104, 00467, 01144, 00420,
01631, 00467, 00420, 01004, 00724, 00133, 00744, 01761,
00467, 01020, 00327, 00104, 00304, 00467, 00031, 00463,
00744, 00661, 00467, 00744, 00020, 00060, 00424, 00303,
00157, 00000, 01477, 00557, 00467, 00724, 00157, 00173,
00744, 00373, 00450, 00724, 00403, 00343, 01656, 00253,
00064, 00320, 00000, 00000, 00104, 00304, 00204, 00467,
00744, 00655, 01450, 00450, 00552, 00552, 01621, 00465,
01631, 00552, 00552, 00465, 01450, 00552, 00552, 01356,
01776, 01316, 01656, 01577, 00650, 00020, 00336, 00332,
01242, 00543, 00222, 00756, 00676, 00543, 01126, 01226,
01656, 00636, 01243, 00724, 01267, 00424, 01253, 00620,
00312, 00324, 01137, 00752, 01252, 01252, 00224, 01137,
00124, 01163, 01203, 00376, 00456, 01772, 01772, 00772,
00772, 00112, 00723, 01656, 01146, 01737, 00753, 01306,
01146, 01743, 00572, 00572, 01352, 01676, 01536, 01176,
01023, 01726, 01536, 00420, 00506, 01037, 00266, 00606,
00420, 00000, 00000, 00000, 00742, 00312, 00562, 01056,
01656, 00034, 00473, 00000, 00000, 00000, 00000, 00124,
01153, 00752, 00224, 01173, 00752, 00752, 00020, 00603,
00020, 01450, 00724, 01647, 00744, 01545, 00450, 00043,
00644, 00020, 00376, 00744, 00404, 00650, 01044, 00450,
00661, 01450, 00616, 01631, 00424, 01337, 00376, 00650,
00450, 01656, 00435, 00424, 01373, 00576, 00661, 01450,
00650, 00661, 00450, 01450, 01450, 00467, 00435, 00655,
01056, 01450, 01656, 01621, 01004, 01761, 00650, 00450,
01656, 00467, 01450, 01450, 00376, 01450, 01450, 01577,
00000, 01056, 01450, 00650, 01056, 00450, 00216, 01050,
01723, 00424, 00107, 00620, 01666, 01637, 00420, 00512,
00420, 00650, 01545, 00450, 01656, 01621, 01450, 01631,
00650, 00655, 01545, 01433, 00724, 00167, 01607, 01656,
00112, 00753, 01752, 00737, 00146, 00053, 00060, 00000,
00020, 01616, 00576, 00007, 01656, 00426, 01656, 00363,
00650, 00450, 01656, 01356, 00604, 01506, 00003, 01316,
00576, 00003, 00776, 00456, 00133, 01322, 01576, 00127,
01376, 01616, 00624, 00553, 01542, 00113, 01462, 00422,
01636, 01607, 00714, 00363, 01752, 01172, 01577, 01426,
00223, 01626, 00416, 00552, 00227, 01316, 00322, 01652,
00676, 00327, 01456, 01416, 00356, 01316, 01044, 00363,
00776, 01416, 00343, 01616, 01056, 00316, 00546, 00430,
00746, 01116, 00554, 01507, 00614, 01362, 01514, 01056,
01656, 00630, 00607, 01354, 01233, 00630, 01130, 00330,
00130, 00430, 00730, 00130, 01030, 00030, 00530, 00630,
01314, 01537, 01742, 00456, 00576, 00133, 01322, 01656,
00426, 01656, 01576, 00557, 01456, 01742, 01047, 01126,
00416, 00347, 01154, 00723, 00714, 00330, 00330, 00030,
01030, 00530, 01114, 01537, 01254, 00457, 01114, 00330,
00130, 00030, 00130, 00730, 01130, 01030, 00130, 01214,
01537, 01626, 00542, 01007, 01316, 00074, 01554, 01013,
00752, 01376, 01414, 01142, 01113, 00416, 00552, 01156,
01057, 00316, 01652, 01272, 01133, 01766, 01652, 01176,
01023, 01646, 00616, 01024, 01573, 00624, 01233, 00644,
01124, 01627, 00316, 00034, 00530, 01637, 01454, 01537,
00316, 00230, 00330, 00030, 00230, 00530, 01030, 00530,
00030, 01130, 00330, 01414, 01656, 00624, 01443, 00516,
00032, 01357, 00516, 01456, 00034, 00416, 00154, 01363,
01656, 00676, 01423, 00346, 00752, 00050, 01314, 01013,
00456, 00606, 01272, 00237, 00772, 01316, 00752, 01467,
00303, 01054, 00653, 00514, 00330, 00330, 01014, 01056,
00624, 00637, 01316, 00074, 00074, 00013, 00220, 01746,
00267, 01476, 01126, 01776, 00117, 01450, 00450, 00314,
00712, 00536, 01663, 00276, 01456, 01356, 00476, 01454,
01013, 00006, 00003, 01662, 01733, 00742, 01426, 01727,
01626, 00426, 00034, 00054, 01733, 00636, 01656, 01047,
00431, 01656, 01615, 00123, 01004, 00220, 01044, 00047,
01004, 00220, 01625, 00431, 01324, 00367, 00655, 00031,
01450, 00031, 00431, 00547, 00650, 00450, 01545, 00573,
01344, 01463, 00423, 01453, 00667, 00000, 00616, 01450,
01727, 00424, 01723, 00320, 00000, 01450, 00250, 01450,
00450, 00415, 00655, 00650, 00450, 01656, 00431, 00655,
00021, 01450, 00450, 01450, 01450, 01656, 01615, 01250,
00655, 01244, 01344, 00467, 00000, 01656, 00651, 01456,
01615, 00077, 00000, 00220, 01344, 00443, 00220, 01304,
01505, 00415, 00655, 01541, 00473, 00020, 00021, 00431,
01656, 01224, 00563, 00651, 01450, 01450, 01450, 01324,
00553, 01656, 01615, 00650, 00650, 00650, 01625, 00347,
01224, 00227, 00444, 01020, 00752, 01615, 00431, 00655,
00650, 00737, 00220, 00220, 00220, 01324, 01603, 01344,
01224, 01357, 01505, 00376, 00450, 00650, 01450, 01615,
00250, 01450, 00450, 00431, 00655, 01456, 01625, 00250,
00650, 00450, 01505, 01450, 00651, 00655, 00250, 01615,
00250, 01450, 00250, 00650, 00431, 00655, 00450, 00450,
00216, 00250, 00021, 01450, 00450, 01625, 00216, 01450,
01450, 01615, 01450, 01450, 00431, 00651, 01250, 01615,
01450, 01450, 00651, 00650, 00450, 01056, 00655, 01450,
01056, 01450, 01056, 01615, 01250, 01625, 00250, 00655,
00250, 00616, 01661, 01172, 01513, 01047, 01625, 01545,
00752, 01304, 00623, 00650, 01656, 01615, 00650, 00450,
01656, 00431, 01656, 01615, 00021, 00431, 00651, 00752,
00752, 00345, 01324, 00437, 01224, 00447, 00376, 00445,
00000, 00220, 01324, 00347, 01450, 01450, 01450, 00003,
00220, 00316, 00742, 00776, 01262, 01126, 00752, 00060,
01224, 01633, 00723, 00220, 00060, 00220, 01656, 01450,
00450, 00450, 00450, 01333, 01250, 00752, 00752, 01752,
01752, 01752, 01752, 00060, 01020, 01615, 00415, 00655,
01450, 00650, 00650, 01056, 01656, 01224, 00077, 00053,
00020, 00000, 00000, 00620, 00060, 00071, 00704, 00444,
00431, 00655, 00207, 00650, 00450, 00060, 01450, 01450,
01450, 00060, 00524, 00023, 01220, 00724, 00027, 00177,
00003, 00000, 01220, 01733, 00504, 01220, 00003, 00424,
01637, 01450, 00444, 00650, 01450, 00616, 00075, 00651,
00055, 00431, 00553, 00127, 00724, 00313, 00424, 00667,
00444, 01147, 00704, 00424, 00437, 00444, 01450, 01127,
00620, 01656, 00316, 00330, 00630, 00752, 00752, 00060,
00651, 00055, 01220, 00220, 01450, 00343, 00220, 00704,
00055, 01656, 00431, 00655, 01450, 00450, 01625, 01450,
01056, 01450, 01456, 00655, 01450, 00650, 00655, 00450,
00075, 00343, 00655, 01250, 01625, 00655, 00055, 00216,
01625, 00055, 01656, 00343, 00250, 00415, 01250, 01656,
00250, 01213, 00220, 00220, 00220, 00055, 00450, 01615,
01450, 00431, 00655, 00216, 01450, 01625, 00650, 01615,
00655, 01450, 00450, 00651, 00655, 01456, 00655, 00071,
00431, 00651, 00650, 01615, 00655, 01450, 01456, 00651,
00650, 00055, 00216, 01625, 00650, 00655, 00376, 00250,
00316, 00450, 01250, 00450, 00423, 00431, 00655, 00450,
00450, 00650, 00650, 00616, 00075, 01625, 01250, 00655,
00055, 00423, 01250, 01615, 01450, 00650, 00651, 00055,
00216, 00651, 00250, 00431, 00655, 00450, 00250, 00015,
00250, 00431, 00651, 00055, 01625, 01450, 00431, 00655,
00055, 01656, 00015, 00650, 00250, 00431, 00651, 01250,
01625, 00250, 00431, 00651, 01450, 00376, 00450, 01625,
01450, 00650, 00650, 00250, 00651, 01250, 01625, 00343,
01625, 00250, 00650, 00055, 00345, 01414, 01615, 01450,
00345, 00530, 01414, 01615, 00055, 00651, 00075, 00650,
01656, 00450, 00403, 00220, 00000, 00220, 00220, 00444,
00250, 01450, 00450, 00450, 00431, 00655, 01056, 01625,
00250, 01656, 01615, 00250, 00213, 00320, 00444, 00250,
00415, 01656, 00450, 01615, 00650, 01450, 00250, 01503,
00000, 00000, 01020, 01450, 00450, 01656, 00450, 00316,
00130, 01030, 00230, 00530, 00030, 00752, 00752, 01414,
00060, 00316, 00130, 01030, 00067, 01224, 00143, 00060,
01420, 00000, 00000, 00504, 00425, 00667, 00371, 01304,
00456, 00616, 00661, 01456, 01615, 00250, 00411, 01656,
01615, 00015, 01615, 00172, 00563, 00351, 01250, 01450,
01250, 00425, 00655, 00371, 01656, 01307, 00650, 00544,
00620, 01450, 01450, 01450, 00060, 01450, 00650, 00450,
00060, 00000, 01020, 00504, 00433, 01020, 01250, 01625,
00411, 01656, 01615, 00365, 01250, 01625, 00411, 01656,
01615, 01450, 01324, 00333, 00337, 01450, 00616, 01172,
00103, 01552, 00406, 00537, 00650, 01250, 00655, 00371,
00105, 01615, 00425, 00651, 01056, 00250, 00376, 01625,
00425, 01547, 00220, 00220, 00220, 01256, 01615, 00250,
00411, 00015, 01615, 00172, 01153, 00365, 00425, 01256,
01656, 00655, 00216, 01615, 01450, 00650, 01450, 00376,
00011, 00525, 01656, 00425, 00655, 00011, 00345, 00371,
00651, 00655, 01250, 01625, 01450, 00065, 01450, 00616,
01250, 01625, 01450, 00650, 01615, 00650, 00655, 01450,
00651, 00337, 00365, 00105, 01615, 01450, 01625, 00425,
01256, 00655, 00250, 00425, 00065, 00655, 00216, 00250,
01615, 00655, 00345, 00425, 00651, 00250, 01625, 01450,
01773, 00011, 00425, 00651, 01541, 00345, 00651, 01541,
01450, 01656, 01615, 01250, 01625, 00371, 00655, 00650,
00351, 00216, 00365, 01250, 00655, 00250, 00216, 00065,
00065, 00065, 00646, 01473, 00672, 00303, 01324, 01757,
01344, 01450, 00450, 00525, 01656, 00425, 00651, 01633,
00220, 00655, 00650, 01615, 00425, 00651, 01250, 01615,
01763, 00000, 00000, 00220, 00000, 00220, 00216, 01625,
01250, 01625, 00351, 00616, 00351, 01625, 00425, 01256,
01656, 00655, 00216, 01615, 00371, 01625, 01450, 00650,
01625, 00450, 00277, 01250, 00065, 00616, 00655, 00337,
00020, 00404, 01663, 01154, 01063, 00060, 01456, 00254,
01047, 01746, 00546, 00652, 00077, 01746, 00546, 00060,
00330, 00630, 00530, 00424, 00137, 01752, 00060, 00230,
00530, 00060, 00000, 00000, 00031, 01456, 00706, 00034,
00054, 00163, 00312, 00616, 00724, 01113, 00650, 01656,
01163, 00724, 00267, 00424, 00303, 00744, 00450, 00450,
00650, 00724, 00323, 00450, 01656, 01657, 00654, 00017,
00060, 00000, 00526, 00367, 00266, 01056, 00650, 01450,
00376, 00645, 01376, 00316, 00514, 00130, 01030, 01656,
01026, 00627, 00450, 00216, 00641, 00544, 00744, 00020,
00074, 00552, 01027, 01656, 00006, 00003, 00031, 00706,
01456, 00424, 00563, 00177, 01006, 00177, 00003, 01356,
01062, 00101, 01014, 01062, 00703, 00216, 00641, 00453,
01356, 00504, 00704, 01414, 00220, 01616, 00542, 00667,
01216, 00034, 00354, 00673, 01556, 00737, 01756, 01752,
00074, 01116, 01154, 00743, 00330, 00414, 01062, 01116,
00034, 01754, 00777, 00652, 00003, 01454, 00503, 00003,
00000, 00454, 00333, 00060, 01354, 01077, 00060, 01546,
00746, 00060, 00542, 01450, 01124, 01647, 00424, 00007,
01144, 01663, 00014, 01106, 00742, 01153, 00152, 00003,
00676, 01217, 00246, 01726, 00714, 00316, 00730, 00330,
00030, 00530, 00126, 00003, 01014, 00101, 00056, 01056,
01146, 01317, 00003, 01746, 01216, 00034, 01343, 00742,
01416, 01337, 01616, 00054, 01323, 01146, 01407, 01616,
00552, 00152, 01423, 01556, 00414, 00330, 00014, 01056,
00316, 01652, 00074, 01752, 00306, 00035, 01406, 01507,
01517, 01146, 01453, 01606, 01706, 00406, 01746, 01452,
00316, 00572, 01716, 01456, 01514, 00034, 00416, 00754,
01567, 01616, 00074, 00416, 01454, 01613, 01752, 01656,
00463, 00424, 00353, 00444, 00552, 01703, 01106, 00312,
00152, 00003, 00056, 00616, 01014, 00322, 00230, 00130,
01014, 00122, 00003, 00130, 01130, 01014, 00522, 00577
};
