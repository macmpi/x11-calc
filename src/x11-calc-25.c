/*
 * x11-calc-25.c - RPN (Reverse Polish) calculator simulator.
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
 * 09 Mar 14   0.1   - Initial version - MT
 * 10 Mar 14         - Changed indexes to BCD hex values - MT
 * 10 Dec 18         - Alternate  function key now LIGHT_BLUE, allowing  it
 *                     to be a different colour to the alternate text - MT
 * 26 Sep 21         - Replaced BCD hex key codes with octal values - MT
 *                   - Added shortcut key definitions - MT
 * 03 Oct 21         - Added switches and updated keys - MT
 *                   - Fixed button positions - MT
 * 04 Nov 21         - Allows size of the window to be changed by modifying
 *                     the value of SCALE at compile time - MT
 * 23 Nov 21         - Added the alternate function text - MT
 * 06 Dec 21         - Label text colour now explicitly defined - MT
 * 03 Jan 22         - Changed debug() macro so that debug code is executed
 *                     when DEBUG is defined (doesn't need to be true) - MT
 * 11 Jan 22         - Removed ROM_BANKS - MT
 * 23 Jan 22         - Changed the colour of the numeric keys - MT
 * 12 Feb 22         - Updated layout and separated the initialisation code
 *                     for buttons and switches into two functions - MT
 * 21 Oct 23         - Updated switch parameters to accomodate a 3 position
 *                     switch - MT
 *
 */

#define VERSION        "0.1"
#define BUILD          "0014"
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
   h_switch[1] = h_switch_create(00000, "PRGM", "", "RUN", h_alternate_font, KBD_LEFT + 3 * KEY_WIDTH + 2 * KEY_GAP, KBD_TOP, 2 * KEY_WIDTH + KEY_GAP, SWITCH_HEIGHT, True, MID_GREY, DARK_GREY);
}

void v_init_buttons(obutton *h_button[]) {
   int i_left, i_top;

   /* Define top row of keys. */
   i_top = KBD_TOP + 2* (SWITCH_HEIGHT + 1); /* Assumes the function label and switch text use the same font */
   i_left = KBD_LEFT;
   h_button[0] = h_button_create(00263, ' ', "SST", "FIX", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[1] = h_button_create(00262, 000, "BST", "SCI", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[2] = h_button_create(00261, 000, "GTO", "ENG", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[3] = h_button_create(00260, 'f', "f", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, YELLOW, BACKGROUND, YELLOW, YELLOW);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[4] = h_button_create(00264, 'g', "g", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, LIGHT_BLUE, BACKGROUND, LIGHT_BLUE, LIGHT_BLUE);

   /* Define second row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[5] = h_button_create(00103, 000, "X-Y", "x", "", "%", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[6] = h_button_create(00102, 000, "R", "s", "", "1/x", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[7] = h_button_create(00101, 000, "STO", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[8] = h_button_create(00100, 000, "RCL", "", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[9] = h_button_create(00104, 000, "E+", "E-", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);

   /* Define third row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[10] = h_button_create(00323, 015, "ENTER", "PREFIX", "", "", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, 2 * KEY_WIDTH + KEY_GAP, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += 2 * (KEY_WIDTH + KEY_GAP);
   h_button[11] = h_button_create(00321, 'c', "CHS", "PRGM", "", "DEG", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[12] = h_button_create(00320, 'e', "EEX", "REG", "", "RAD", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[13] = h_button_create(00324, 033, "CLX", "STK", "", "GRD", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, BLACK, YELLOW, MID_BLUE, BLACK);
   i_left += (KEY_WIDTH + KEY_GAP);

   /* Define fourth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[14] = h_button_create(00143, '-', "-", "X<Y", "", "X<0", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[15] = h_button_create(00142, '7', "7", "LN", "", "eX", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[16] = h_button_create(00141, '8', "8", "LOG", "", "10x", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[17] = h_button_create(00140, '9', "9", "-R", "", "-P", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);

   /* Define fifth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[18] = h_button_create(00243, '+', "+", "X\x1b\x59", "", "X\x1b\x30", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[19] = h_button_create(00242, '4', "4", "SIN", "", "SIN-\xb9", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[20] = h_button_create(00241, '5', "5", "COS", "", "COS-\xb9", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[21] = h_button_create(00240, '6', "6", "TAN", "", "TAN-\xb9", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);

   /* Define sixth row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[22] = h_button_create(00163, '*', "\xd7", "X\x1d\x59", "", "X\x1d\x30", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[23] = h_button_create(00162, '1', "1", "INT", "", "FRAC", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[24] = h_button_create(00161, '2', "2", "V\xaf", "", "x\xb2", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[25] = h_button_create(00160, '3', "3", "yX", "", "ABS", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);

   /* Define bottom row of keys. */
   i_top += KBD_ROW;
   i_left = KBD_LEFT;
   h_button[26] = h_button_create(00223, '/', "\xf7", "X=Y", "", "X=0", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_WIDTH, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_WIDTH + KEY_GAP);
   h_button[27] = h_button_create(00222, '0', "0", "-H.MS", "", "-H", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[28] = h_button_create(00221, '.', ".", "LASTx", "", "\x1c", h_large_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
   i_left += (KEY_NUMERIC + 2 * KEY_GAP );
   h_button[29] = h_button_create(00220, 000, "R/S", "PAUSE", "", "NOP", h_normal_font, h_small_font, h_alternate_font, i_left, i_top, KEY_NUMERIC, KEY_HEIGHT, False, False, LIGHT_GRAY, YELLOW, MID_BLUE, LIGHT_GRAY);
}

int i_rom[ROM_SIZE] = {
   01173, 00202, 01242, 00202, 00427, 01053, 00555, 00313,
   00643, 01671, 00710, 01566, 00030, 00704, 00006, 01526,
   00110, 01731, 00774, 00432, 00742, 00342, 00302, 00610,
   01605, 00174, 01306, 00015, 00333, 00724, 00076, 01671,
   00332, 01074, 00742, 01553, 00732, 00720, 01154, 00042,
   00620, 00206, 00620, 00272, 01146, 00216, 01410, 01526,
   00006, 00036, 00236, 00610, 00132, 01605, 00132, 00124,
   01754, 00132, 00334, 01672, 00164, 00727, 01474, 00102,
   00132, 01074, 00706, 01174, 00706, 00174, 01646, 01646,
   00710, 00232, 00256, 01410, 01352, 00120, 00412, 01216,
   00236, 01601, 00705, 00236, 00555, 00236, 01431, 00610,
   01324, 01751, 00333, 00116, 00016, 01610, 00616, 00774,
   01542, 00353, 01156, 00620, 00724, 00140, 00726, 00603,
   00132, 00174, 00724, 00001, 01262, 00262, 01034, 01746,
   00317, 00174, 00002, 00642, 00642, 00142, 00706, 00706,
   01020, 00624, 00050, 01710, 00656, 01710, 00243, 00000,
   00432, 01160, 01150, 01250, 01350, 01450, 01550, 01650,
   01750, 01020, 00734, 00215, 00110, 00604, 01034, 00037,
   00327, 00334, 00730, 01124, 01743, 01224, 01743, 00414,
   00104, 00364, 01163, 01726, 01647, 00000, 01160, 01260,
   00756, 00752, 00762, 00572, 00410, 00432, 00610, 00310,
   01410, 00072, 00032, 01710, 01605, 00510, 00256, 00012,
   00264, 01017, 00774, 00406, 01146, 00412, 01020, 01304,
   00552, 01313, 00432, 01020, 00134, 00072, 00210, 00432,
   00772, 00772, 01366, 00300, 00337, 00710, 01410, 01474,
   01366, 00272, 01352, 00321, 01156, 00752, 01152, 01337,
   00756, 01020, 01134, 00726, 00032, 00324, 01517, 01001,
   00364, 00477, 01142, 00754, 00171, 00604, 00416, 00253,
   00132, 00032, 00662, 01672, 00662, 00662, 00136, 01610,
   01474, 01102, 01157, 00116, 00056, 01020, 01024, 00376,
   00432, 00610, 00234, 00365, 01310, 00204, 00432, 00420,
   01172, 01710, 00032, 01710, 00422, 00372, 01004, 01020,
   00424, 00545, 00132, 01474, 00232, 01430, 01474, 00202,
   00502, 01755, 00324, 00514, 00732, 01074, 00430, 01074,
   00502, 00656, 00732, 00732, 00463, 00756, 00756, 00756,
   00132, 00242, 01211, 01751, 00324, 00610, 00732, 00732,
   01656, 00652, 01037, 00432, 01172, 01174, 01020, 00420,
   00432, 00610, 00710, 01172, 00214, 00774, 01630, 01230,
   01230, 01430, 01230, 00110, 00232, 00432, 00072, 00340,
   01714, 00274, 00620, 01354, 00472, 01724, 00470, 01020,
   00433, 01433, 00756, 01227, 00756, 00413, 00215, 00230,
   00430, 01245, 01174, 01330, 01641, 01524, 00625, 00256,
   00716, 01772, 01057, 00272, 00724, 00537, 01352, 00537,
   00432, 01474, 00530, 00232, 00516, 00643, 00272, 01020,
   00756, 01167, 01177, 01624, 00400, 01074, 01730, 01373,
   01134, 01741, 00272, 00764, 01013, 00004, 00756, 00756,
   00756, 00756, 01237, 00756, 01217, 01114, 01214, 01353,
   01124, 00425, 00215, 00130, 00330, 01245, 01641, 00024,
   00674, 00132, 00242, 01751, 00324, 00610, 00732, 00732,
   01641, 00216, 00416, 01772, 01756, 00616, 00364, 00533,
   00040, 01547, 01020, 00756, 00717, 00110, 01363, 00215,
   00130, 00430, 00110, 01104, 01347, 01504, 00756, 00756,
   00756, 00667, 00677, 00756, 00617, 00756, 01523, 00756,
   01020, 00324, 00655, 00372, 00032, 00132, 00432, 01020,
   01137, 00743, 00340, 00340, 00215, 00130, 00530, 00110,
   01204, 01245, 01614, 01645, 01074, 01430, 01074, 01224,
   00704, 01134, 00616, 01142, 01142, 01073, 00215, 00230,
   00330, 01245, 00110, 01604, 01174, 01230, 01645, 00467,
   01527, 00040, 00000, 01563, 00756, 00756, 00756, 00756,
   00756, 00756, 00404, 01117, 01124, 00740, 01234, 00727,
   00324, 01743, 00314, 00334, 01672, 01734, 00742, 01713,
   00110, 00210, 00341, 01760, 00324, 00742, 00324, 00247,
   01734, 00756, 00310, 00420, 01474, 00416, 00132, 01514,
   00020, 00000, 00232, 00732, 00732, 00732, 00232, 01020,
   00407, 00407, 00407, 00407, 00407, 00603, 00577, 00573,
   00164, 00237, 01104, 01204, 00773, 01104, 01204, 00617,
   00163, 00203, 00742, 00742, 00742, 01410, 01242, 01142,
   01142, 01674, 00130, 01574, 00230, 00330, 00232, 00543,
   00230, 00430, 00173, 00741, 00752, 00307, 00752, 00752,
   00752, 00303, 00752, 00745, 00742, 00742, 00742, 00742,
   00742, 00742, 00752, 00742, 00232, 00553, 00310, 00256,
   00436, 00420, 01176, 01374, 01114, 01214, 01614, 00661,
   01604, 00716, 00012, 01410, 00656, 00420, 01512, 01120,
   01624, 01122, 00130, 00330, 01174, 00232, 00706, 00547,
   00402, 00220, 01746, 00130, 01574, 00721, 00236, 00012,
   01174, 00706, 00420, 00510, 00364, 00207, 00742, 00742,
   00742, 00742, 01020, 00665, 01256, 00256, 00716, 00012,
   01174, 00406, 00652, 00652, 00220, 01234, 01167, 01124,
   01164, 00130, 00530, 00733, 00130, 00430, 01730, 01020,
   00752, 00752, 00752, 01020, 00000, 00000, 00665, 00232,
   00716, 00012, 00543, 01614, 01152, 01053, 01604, 00656,
   00656, 01063, 01152, 01427, 00710, 01705, 01112, 01512,
   01223, 00272, 01233, 01401, 00116, 01634, 01240, 00156,
   00356, 01352, 01235, 00412, 01156, 01156, 01333, 00316,
   00216, 01352, 01245, 01216, 01152, 00216, 00056, 00064,
   00705, 00262, 00110, 00514, 00524, 00274, 00774, 00342,
   01242, 00342, 00620, 00044, 00274, 01277, 01156, 01347,
   01367, 01156, 01167, 01116, 01726, 01116, 01726, 01177,
   00420, 00006, 01106, 01410, 01020, 00710, 00516, 01504,
   01705, 01512, 01327, 01401, 00116, 01074, 01502, 01345,
   01116, 01527, 00016, 00116, 01247, 01726, 01503, 01666,
   00656, 01537, 00410, 00256, 00410, 00012, 00774, 00620,
   01116, 01577, 00006, 01526, 01350, 00174, 01721, 01073,
   01401, 00116, 01513, 00344, 01364, 00620, 01116, 01657,
   01723, 00774, 00656, 01673, 00032, 00246, 00526, 01773,
   00662, 01676, 00656, 01534, 01376, 00620, 00156, 01020,
   00443, 00443, 00443, 00443, 00443, 00443, 00443, 00443,
   00443, 00443, 01247, 01027, 00143, 00143, 00143, 00143,
   01077, 01077, 01077, 01077, 01077, 00752, 00752, 01213,
   00564, 00203, 00752, 00540, 00040, 00752, 00752, 00464,
   00303, 01566, 01445, 00032, 01132, 00574, 00232, 00720,
   00732, 01454, 01447, 00206, 00432, 00372, 00334, 00565,
   00210, 00164, 00341, 00310, 00424, 00247, 00372, 01663,
   01051, 00040, 00510, 00272, 00432, 01474, 01030, 01174,
   00730, 01474, 01410, 01126, 01407, 00114, 01170, 01020,
   01144, 01626, 00132, 00732, 00036, 00732, 00732, 00510,
   00226, 01566, 01524, 00114, 00410, 01617, 00324, 01766,
   00372, 01751, 00351, 00132, 00766, 01233, 00070, 00202,
   00720, 00202, 00620, 01360, 01357, 01410, 01626, 01407,
   01160, 00232, 00272, 00666, 01263, 01020, 00072, 00106,
   01732, 00674, 00472, 00620, 00054, 01562, 01020, 01410,
   00510, 01366, 01775, 01166, 01767, 01772, 00620, 01713,
   01124, 00426, 00735, 00404, 01353, 00610, 00234, 01611,
   01310, 01613, 00134, 01615, 00735, 00114, 01020, 00156,
   01756, 01160, 00070, 00716, 00012, 00220, 00510, 01656,
   00112, 00216, 00410, 01617, 00510, 01566, 01641, 00766,
   00410, 01353, 01274, 00352, 00412, 01103, 00732, 00732,
   00563, 00710, 01360, 01613, 00720, 00720, 00657, 01772,
   00620, 01363, 00000, 01003, 01124, 00427, 00414, 00324,
   01634, 01751, 00351, 00070, 01354, 01657, 00412, 00264,
   00333, 00420, 00742, 00627, 00510, 00426, 00410, 00114,
   01617, 00464, 01615, 00132, 00671, 00671, 00664, 01425,
   01607, 00464, 01615, 00675, 00072, 00106, 00132, 00732,
   00705, 00664, 01425, 00432, 00330, 00630, 00774, 00564,
   01561, 00610, 00204, 00110, 00064, 01425, 00610, 00254,
   01752, 01051, 00134, 00247, 01724, 01707, 00210, 00351,
   01751, 00070, 01354, 01575, 00436, 00310, 00412, 00256,
   00652, 00220, 01410, 00510, 00766, 00410, 00420, 01020,
   00737, 01037, 00763, 01453, 00040, 01027, 00167, 00040,
   00040, 01503, 01533, 01563, 01253, 01657, 01657, 01657,
   01104, 01427, 01104, 01173, 01237, 01613, 01657, 00662,
   00662, 01743, 00557, 00267, 01613, 01310, 00214, 01613,
   01104, 01413, 01104, 01163, 01573, 01227, 01657, 01657,
   01667, 01477, 01127, 00343, 01047, 00032, 00642, 01457,
   00716, 00212, 00710, 00774, 01410, 00022, 01114, 00220,
   01761, 00770, 00272, 01551, 01101, 00370, 01004, 00774,
   01555, 01101, 00670, 00232, 01515, 00610, 00370, 00272,
   00432, 00742, 01515, 00032, 00642, 01555, 00710, 01551,
   01101, 00422, 00764, 00505, 01603, 01732, 00656, 00527,
   00032, 01374, 01020, 01210, 01050, 01156, 01156, 01551,
   01157, 00032, 01552, 00163, 00232, 00156, 00620, 00344,
   00154, 01116, 00633, 00206, 00316, 00016, 00236, 01134,
   00233, 00664, 01425, 01124, 00233, 00432, 01157, 01050,
   01262, 01010, 01521, 01603, 01010, 01050, 01551, 01603,
   00000, 00000, 00000, 00000, 00000, 00114, 01613, 01050,
   00747, 00272, 01050, 01262, 00610, 00710, 01204, 01267,
   00064, 01431, 00244, 00340, 01020, 01761, 00770, 00232,
   00370, 01004, 01555, 01603, 01210, 01515, 01566, 00307,
   01134, 00342, 00364, 01751, 01613, 00110, 00040, 01761,
   00564, 01523, 00272, 01050, 01214, 01551, 01104, 00670,
   00564, 01605, 00770, 00564, 00465, 00710, 01210, 01551,
   00570, 00564, 01605, 01210, 00232, 00272, 01234, 00301,
   01262, 00540, 01210, 00232, 01515, 01362, 00240, 01134,
   00242, 01613, 01010, 01050, 01004, 01555, 01603, 00010,
   00432, 00610, 00173, 01262, 00564, 00253, 01010, 01310,
   00232, 01603, 00540, 00540, 01110, 01603, 01761, 01070,
   00610, 00204, 00340, 00272, 00366, 00416, 01474, 00530,
   01632, 01512, 00125, 00164, 00237, 00432, 01160, 00050,
   00150, 00250, 00350, 00450, 00550, 00650, 00750, 01613,
   00410, 00222, 00410, 01613, 00234, 00377, 01310, 01020,
   01503, 00577, 01507, 01513, 00713, 00707, 00717, 00457,
   00447, 01243, 00364, 01447, 01573, 00467, 00617, 00637,
   00053, 00603, 01057, 01747, 01123, 01117, 01127, 01767,
   01763, 01107, 00064, 01433, 00716, 00212, 00710, 01050,
   00072, 00774, 00110, 01410, 00220, 00000, 00140, 01134,
   00730, 01020, 00774, 00072, 00652, 00652, 00752, 00752,
   01416, 00463, 00232, 00226, 01366, 00467, 00232, 00366,
   01416, 01670, 01732, 00656, 01332, 01670, 00343, 00410,
   00262, 00410, 00774, 01122, 00667, 00032, 00642, 00656,
   01213, 01004, 00640, 01004, 00640, 00272, 00710, 01607,
   00740, 00432, 00474, 00730, 01030, 00530, 00330, 01130,
   01030, 00130, 00630, 00330, 00530, 00774, 01020, 01104,
   00440, 00000, 00000, 00272, 00710, 01555, 01613, 00272,
   00710, 00774, 01004, 01561, 01613, 01122, 01203, 01504,
   01020, 00704, 01604, 00375, 01104, 00164, 00515, 00774,
   01634, 01427, 01262, 00222, 00764, 00077, 00610, 00232,
   01210, 01555, 00151, 01434, 00606, 01262, 01010, 01310,
   00710, 01555, 01537, 00501, 01234, 00727, 01310, 00610,
   00717, 01204, 01221, 00704, 01604, 00375, 01524, 00635,
   00232, 01561, 00505, 00572, 01555, 00164, 00515, 00740,
   00032, 01102, 00656, 01020, 01010, 01310, 00232, 01020,
   01204, 01362, 00654, 01404, 00610, 00710, 01210, 01526,
   00663, 01366, 00727, 00774, 01561, 00151, 00610, 01551,
   01221, 01551, 01010, 00251, 01112, 01112, 01512, 00613,
   00151, 01073, 00232, 00470, 01605, 00774, 00432, 00742,
   01234, 00713, 01262, 00232, 00370, 01605, 00464, 01507,
   00340, 01551, 00422, 01537, 00505, 00572, 00572, 00610,
   00464, 01607, 00272, 00640, 01056, 00640, 00272, 00710,
   01262, 00251, 00151, 01360, 00254, 00447, 00432, 00420,
   01172, 00030, 01330, 00164, 00317, 01234, 00727, 00032,
   00642, 01366, 00576, 00610, 01561, 00710, 01555, 00151,
   01003, 01010, 00232, 01304, 00604, 01004, 00032, 01626,
   01643, 01672, 01162, 01643, 00774, 00762, 00172, 01755,
   01102, 00027, 00106, 00462, 00757, 00274, 01275, 00374,
   00341, 01574, 00335, 01541, 00674, 00335, 01001, 00474,
   00335, 00621, 00335, 01701, 00335, 00715, 00232, 01072,
   01312, 01043, 01072, 00132, 00620, 00732, 00554, 01044,
   00232, 01362, 01054, 01266, 00756, 00474, 01371, 01324,
   01332, 00634, 00727, 00715, 01565, 01153, 00740, 01672,
   00372, 00357, 00472, 01162, 00353, 00232, 00736, 00232,
   01277, 00232, 00172, 00266, 00552, 01203, 00752, 01672,
   00756, 00437, 01247, 00711, 01561, 00711, 00405, 01701,
   00474, 00331, 00621, 00674, 00331, 01001, 01574, 00331,
   01541, 00374, 00331, 00331, 00331, 01374, 00006, 01274,
   00372, 00232, 00630, 01123, 01574, 00330, 00130, 00030,
   00130, 00730, 01130, 01030, 00030, 00530, 00530, 01344,
   01212, 01533, 00272, 00432, 00774, 00230, 00330, 00030,
   00230, 00530, 01023, 00122, 00662, 01776, 00706, 00033,
   00274, 00330, 00330, 00030, 01030, 00530, 00030, 01144,
   01213, 01130, 00330, 01533, 01755, 00642, 00172, 01162,
   01063, 01646, 00232, 00736, 00232, 01122, 01073, 00132,
   00642, 01425, 00440, 01034, 01240, 00656, 01512, 01370,
   01036, 01157, 00476, 00732, 01156, 01173, 01672, 00406,
   00216, 01362, 01256, 00132, 01032, 01272, 01672, 00372,
   00432, 01166, 01034, 01270, 00430, 00766, 01357, 00630,
   00554, 01267, 01772, 01772, 01020, 00472, 01142, 01367,
   01672, 00720, 01154, 01276, 00756, 00022, 00774, 00072,
   01502, 01317, 00732, 01156, 01532, 01310, 00432, 00156,
   00472, 01522, 01300, 00226, 00272, 00072, 00774, 01020,
   01674, 01007, 00710, 00463, 01174, 00616, 01062, 01607,
   01222, 00072, 00126, 00032, 00254, 01276, 01566, 01355,
   01024, 00446, 00306, 01126, 00752, 00346, 00226, 00740,
   00630, 01130, 00330, 00130, 00430, 00730, 00130, 00657,
   00666, 01233, 01646, 01122, 01753, 00022, 00472, 01020,
   00132, 00271, 00461, 00271, 00461, 00734, 01410, 00232,
   01634, 01424, 01362, 01415, 01404, 00422, 01555, 01661,
   01551, 00235, 00505, 00461, 01555, 01134, 00755, 00032,
   00642, 00166, 00226, 01156, 01706, 01352, 01433, 01646,
   00756, 00177, 01672, 01732, 01661, 00372, 00573, 00032,
   00642, 00540, 01010, 01310, 00232, 01153, 01661, 00232,
   01342, 01463, 01232, 00272, 00316, 01423, 00640, 00732,
   01736, 00372, 00363, 00762, 01032, 00357, 00472, 00640,
   00640, 00742, 01632, 00407, 00532, 00732, 00620, 01746,
   01354, 01502, 00402, 00317, 00232, 00710, 00232, 01020,
   00540, 00272, 00072, 00372, 00174, 01577, 00000, 01706,
   01122, 00537, 00762, 00106, 00606, 00132, 00172, 01606,
   00543, 00461, 01672, 00206, 00132, 00706, 01661, 00662,
   00662, 00227, 00432, 00056, 01676, 01675, 01142, 00461,
   00232, 00174, 00375, 01374, 00331, 00374, 00331, 00574,
   01030, 00674, 00331, 01741, 00331, 00501, 00732, 00331,
   00332, 01421, 00501, 00572, 00724, 01613, 01434, 01620,
   00572, 00232, 00262, 00232, 01262, 00245, 00501, 00572,
   00232, 01524, 01630, 00232, 01555, 00564, 00375, 01551,
   01224, 01452, 00440, 00620, 00476, 01563, 00164, 00237,
   00272, 00501, 00572, 00572, 00572, 00401, 00501, 00674,
   00335, 01741, 00374, 00341, 00574, 01030, 01374, 00335,
   00174, 00335, 00335, 00132, 01772, 01274, 00530, 01523,
   01152, 01152, 00016, 01622, 01522, 01677, 00640, 01466,
   01703, 01262, 00132, 01032, 00640, 01706, 01706, 01162,
   01427, 00606, 01006, 00346, 00332, 01122, 01437, 00206,
   00461, 01322, 01400, 00732, 00206, 01661, 01706, 01162,
   00362, 01463, 00540, 00540, 00456, 01577, 01142, 00762,
   01354, 01633, 00216, 00016, 01542, 01747, 01672, 01772,
   00356, 00416, 00774, 00437, 00610, 00710, 01020, 00742,
   01036, 01677, 00476, 00736, 00620, 01354, 01760, 00317,
   01374, 01030, 00630, 00530, 00230, 00430, 01130, 01020
};
