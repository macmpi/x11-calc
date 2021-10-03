/*
 * x11-calc-22.c - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2018   MT
 *
 * Model specific functions
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
 * 21 Sep 21   0.1   - Initial version - MT
 *  3 Oct 21         - Updated switches - MT
 *
 */

#define VERSION        "0.1"
#define BUILD          "0001"
#define DATE           "21 Sep 21"
#define AUTHOR         "MT"

#define DEBUG 0        /* Enable/disable debug*/

#include <stdarg.h>    /* strlen(), etc. */
#include <stdio.h>     /* fprintf(), etc. */
#include <stdlib.h>    /* getenv(), etc. */

#include <X11/Xlib.h>  /* XOpenDisplay(), etc. */
#include <X11/Xutil.h> /* XSizeHints etc. */

#include "x11-calc-font.h"
#include "x11-calc-button.h"
#include "x11-calc-switch.h"
#include "x11-calc-colour.h"

#include "x11-calc-cpu.h"

#include "x11-calc.h"

#include "gcc-debug.h"

oregister o_mem[MEMORY_SIZE];

void v_init_keypad(obutton *h_button[], oswitch *h_switch[]) {

   /* Define the switches. */
   h_switch[0] = h_switch_create(00000, "OFF", "ON ", h_alternate_font, 12, 67, 67, 10, True, MID_GREY, DARK_GREY);
   h_switch[1] = h_switch_create(00000, "BEGIN", "END", h_alternate_font, 120, 67, 67, 10, False, MID_GREY, DARK_GREY);

   /* Define top row of keys. */
   h_button[0] = h_button_create(00263, 000, "n", "12\xd7", "", h_normal_font, h_small_font, h_alternate_font, 12, 89, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[1] = h_button_create(00262, 000, "i", "12\xf7", "", h_normal_font, h_small_font, h_alternate_font, 48, 89, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[2] = h_button_create(00261, 000, "PMT", "ACC", "", h_normal_font, h_small_font, h_alternate_font, 84, 89, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[3] = h_button_create(00260, 000, "PV", "INT", "", h_normal_font, h_small_font, h_alternate_font, 120, 89, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[4] = h_button_create(00264, 000, "FV", "BAL", "", h_normal_font, h_small_font, h_alternate_font, 156, 89, 33, 30, False, BLACK, YELLOW, BLACK);

   /* Define second row of keys. */
   h_button[5] = h_button_create(00103, 000, "X-Y", "LR", "", h_normal_font, h_small_font, h_alternate_font, 12, 132, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[6] = h_button_create(00102, 000, "R", "y", "", h_normal_font, h_small_font, h_alternate_font, 48, 132, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[7] = h_button_create(00101, 000, "STO", "x", "", h_normal_font, h_small_font, h_alternate_font, 84, 132, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[8] = h_button_create(00100, 000, "RCL", "s", "", h_normal_font, h_small_font, h_alternate_font, 120, 132, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[9] = h_button_create(00104, 'f', "", "", "", h_normal_font, h_small_font, h_alternate_font, 156, 132, 33, 30, False, YELLOW, BACKGROUND, YELLOW);

   /* Define third row of keys. */
   h_button[10] = h_button_create(00323, 015, "ENTER", "RESET", "", h_normal_font, h_small_font, h_alternate_font, 12, 175, 69, 30, False, BLACK, YELLOW, BLACK);
   h_button[11] = h_button_create(00321, 'c', "CHS", "%E", "", h_normal_font, h_small_font, h_alternate_font, 84, 175, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[12] = h_button_create(00320, 000, " % ", "d%", "", h_normal_font, h_small_font, h_alternate_font, 120, 175, 33, 30, False, BLACK, YELLOW, BLACK);
   h_button[13] = h_button_create(00324, 033, "CLX", "CLEAR", "", h_normal_font, h_small_font, h_alternate_font, 156, 175, 33, 30, False, BLACK, YELLOW, BLACK);

   /* Define fourth row of keys. */
   h_button[14] = h_button_create(00143, '-', "-", "ln", "", h_large_font, h_small_font, h_alternate_font, 12, 218, 33, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[15] = h_button_create(00142, '7', "7", "", "", h_large_font, h_small_font, h_alternate_font, 52, 218, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[16] = h_button_create(00141, '8', "8", "", "", h_large_font, h_small_font, h_alternate_font, 100, 218, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[17] = h_button_create(00140, '9', "9", "", "", h_large_font, h_small_font, h_alternate_font, 148, 218, 41, 30, False, BEIGE, YELLOW, BEIGE);

   /* Define fifth row of keys. */
   h_button[18] = h_button_create(00243, '+', "+", "eX", "", h_large_font, h_small_font, h_alternate_font, 12, 261, 33, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[19] = h_button_create(00242, '4', "4", "", "", h_large_font, h_small_font, h_alternate_font, 52, 261, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[20] = h_button_create(00241, '5', "5", "", "", h_large_font, h_small_font, h_alternate_font, 100, 261, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[21] = h_button_create(00240, '6', "6", "", "", h_large_font, h_small_font, h_alternate_font, 148, 261, 41, 30, False, BEIGE, YELLOW, BEIGE);

   /* Define sixth row of keys. */
   h_button[22] = h_button_create(00163, '*', "\xd7", "yX", "", h_large_font, h_small_font, h_alternate_font, 12, 304, 33, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[23] = h_button_create(00162, '1', "1", "", "", h_large_font, h_small_font, h_alternate_font, 52, 304, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[24] = h_button_create(00161, '2', "2", "", "", h_large_font, h_small_font, h_alternate_font, 100, 304, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[25] = h_button_create(00160, '3', "3", "", "", h_large_font, h_small_font, h_alternate_font, 148, 304, 41, 30, False, BEIGE, YELLOW, BEIGE);

   /* Define bottom row of keys. */
   h_button[26] = h_button_create(00223, '/', "\xf7", "V\xaf", "", h_large_font, h_small_font, h_alternate_font, 12, 347, 33, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[27] = h_button_create(00222, '0', "0", "", "", h_large_font, h_small_font, h_alternate_font, 52, 347, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[28] = h_button_create(00221, '.', ".", "", "", h_large_font, h_small_font, h_alternate_font, 100, 347, 41, 30, False, BEIGE, YELLOW, BEIGE);
   h_button[29] = h_button_create(00220, 000, "E+", "E-", "", h_normal_font, h_small_font, h_alternate_font, 148, 347, 41, 30, False, BEIGE, YELLOW, BEIGE);
}

int i_rom[ROM_SIZE * ROM_BANKS] = {
00672, 00672, 01710, 00410, 00432, 01160, 01260, 00610,
00432, 00214, 00110, 01410, 00310, 00231, 01566, 00021,
00432, 00072, 00661, 01610, 00134, 00174, 00361, 01112,
01512, 00034, 00272, 00223, 00551, 00116, 00216, 01352,
00043, 01216, 01152, 00216, 00715, 01047, 01352, 00055,
01156, 00752, 01152, 00273, 00756, 01020, 00774, 00552,
00317, 00432, 01020, 00406, 01146, 00412, 01020, 01310,
00047, 00000, 00000, 00000, 00774, 00656, 00473, 00032,
00246, 00526, 00447, 00662, 01676, 00656, 00124, 00111,
00620, 00156, 01020, 00344, 00077, 00620, 01116, 00457,
00377, 01721, 00114, 00354, 00130, 00104, 01474, 01102,
01710, 00053, 00006, 00420, 01106, 01410, 01020, 00434,
00362, 00764, 01563, 00234, 00146, 01310, 00204, 00432,
00774, 00551, 00022, 00172, 00032, 00662, 01672, 00662,
00662, 00132, 01020, 00174, 00706, 00706, 00046, 00102,
00642, 00642, 00102, 01020, 00516, 00361, 01512, 00266,
00551, 00116, 01074, 01502, 00302, 01116, 01443, 00016,
00116, 00514, 00534, 00256, 01721, 00615, 01771, 00420,
01074, 00642, 01553, 00706, 00720, 01154, 00221, 00620,
00620, 00232, 00272, 01146, 00216, 01410, 01526, 00312,
00036, 00236, 01715, 00344, 00242, 00624, 00217, 01566,
00217, 01502, 00254, 01213, 00635, 01073, 00774, 00342,
01242, 00342, 00620, 00044, 00214, 01277, 01666, 00656,
01333, 00016, 01610, 00774, 00620, 01116, 01363, 00006,
01526, 00305, 00174, 00375, 00137, 00551, 00116, 01037,
01726, 01027, 01274, 00620, 00756, 01302, 00313, 00774,
01156, 01542, 00241, 00620, 00726, 01503, 00624, 00227,
01726, 01137, 01102, 00754, 00326, 00016, 00624, 00230,
01410, 01074, 00642, 00132, 00661, 00216, 00256, 00715,
01715, 00624, 00350, 00756, 00231, 00254, 00346, 00053,
00204, 00053, 01262, 00414, 00262, 00210, 01714, 00274,
00620, 01354, 00370, 01724, 00366, 01760, 01734, 00376,
00310, 01474, 00002, 00020, 00774, 00072, 00652, 00652,
00752, 00752, 01416, 00415, 00232, 00226, 01366, 00421,
00232, 00366, 01416, 00431, 01732, 00656, 01332, 00431,
00113, 01152, 01152, 00016, 01622, 01522, 00443, 00472,
01522, 00715, 01020, 01466, 00447, 01262, 00132, 01032,
00072, 00453, 00604, 00574, 01020, 00064, 01717, 00616,
01214, 01245, 01174, 00545, 00774, 00072, 00156, 00446,
00363, 01646, 00756, 00642, 00226, 00272, 00072, 01020,
01073, 01357, 01553, 01327, 00110, 00404, 00064, 00507,
01522, 00711, 00774, 01506, 00633, 00416, 01020, 00774,
01366, 00734, 01056, 01245, 00346, 00226, 01621, 00323,
00472, 01142, 00543, 00244, 00510, 00720, 01672, 00547,
00642, 00642, 01177, 00424, 01352, 00771, 01262, 00661,
00737, 00642, 01020, 01262, 00021, 00321, 01020, 00642,
00642, 00642, 00647, 00424, 00775, 00771, 00275, 01534,
00757, 00064, 00231, 01360, 00510, 01677, 01534, 00610,
00265, 00410, 00232, 01160, 00070, 00232, 00510, 01020,
01010, 01020, 00064, 00615, 00070, 01677, 00664, 01547,
00740, 00253, 01020, 00424, 01354, 00771, 01004, 00475,
00737, 00662, 01156, 01502, 00516, 00706, 01147, 00642,
00642, 00642, 00677, 00424, 01647, 00771, 00637, 00764,
01433, 00072, 00126, 01062, 01273, 01222, 00032, 01020,
00440, 00440, 00440, 00440, 00440, 00424, 01747, 01010,
01310, 00232, 01677, 00664, 01517, 01074, 00006, 00265,
00344, 00215, 00232, 01160, 00232, 00024, 00612, 01360,
01677, 01034, 00715, 01224, 00717, 00756, 01672, 01020,
01237, 00064, 00577, 01533, 00364, 01653, 00424, 01653,
00064, 00337, 00764, 01543, 01024, 01132, 00406, 01146,
00412, 00272, 01020, 00742, 01036, 01617, 00476, 00732,
00620, 01154, 00744, 00272, 00022, 00356, 00453, 00040,
00064, 00635, 00232, 01630, 01230, 01230, 01430, 01230,
00232, 00066, 00110, 00064, 01045, 00410, 01010, 00232,
00025, 00064, 01703, 01214, 00037, 01204, 00614, 01014,
01114, 00774, 01366, 01125, 01562, 01140, 01356, 01176,
00756, 00032, 01626, 01356, 01222, 01646, 00162, 01274,
00620, 01122, 00143, 00122, 00162, 00436, 00207, 01672,
00742, 00122, 00172, 00420, 00522, 00610, 00222, 00732,
01672, 01162, 00243, 01410, 00610, 00472, 00732, 01122,
00177, 00262, 01122, 00522, 01137, 00132, 00732, 00353,
01162, 00720, 00372, 01531, 01672, 00372, 00403, 00472,
01142, 00377, 01562, 01070, 00244, 01242, 00432, 00720,
01156, 00254, 01107, 00164, 00451, 00022, 01134, 01121,
01162, 01224, 01632, 00164, 00323, 01234, 01132, 00410,
01566, 01134, 00664, 01623, 01562, 01132, 00432, 01020,
00422, 01234, 01132, 00410, 00232, 00172, 01512, 01132,
00656, 01116, 00736, 01526, 01173, 01516, 01167, 00222,
00262, 00562, 00562, 00622, 01362, 01170, 00604, 00422,
00372, 00410, 00073, 01516, 01151, 00553, 00272, 01102,
01526, 01205, 00432, 00032, 00507, 00164, 00451, 00072,
00366, 00432, 00222, 00164, 01621, 00420, 00522, 01122,
00432, 00127, 01104, 00164, 00451, 00416, 00127, 00544,
01065, 00762, 00132, 00162, 00732, 00262, 00420, 00462,
00620, 00207, 01356, 01113, 01172, 00372, 00066, 01425,
00232, 01072, 01312, 01255, 01072, 00232, 00372, 01352,
01262, 01272, 00206, 00620, 00732, 00554, 01263, 00774,
01502, 01302, 00726, 00232, 00222, 00164, 00545, 00022,
00426, 00507, 00656, 00620, 01357, 00432, 00774, 00230,
00330, 00030, 00230, 00530, 01030, 00530, 00030, 01130,
00230, 01130, 01130, 00430, 00774, 01020, 00432, 00244,
01362, 01172, 00430, 00772, 00422, 01772, 00720, 01444,
01400, 01544, 01415, 00444, 01430, 00144, 01441, 01644,
01450, 01020, 00015, 00007, 00232, 00764, 00723, 00000,
00000, 00000, 00474, 00630, 01130, 00330, 00130, 00430,
00730, 00130, 01030, 00030, 00530, 00630, 00774, 01020,
01574, 00330, 00130, 00030, 00130, 00730, 01130, 01030,
00030, 00430, 00330, 00474, 01020, 00274, 00330, 00330,
00030, 01030, 00530, 00330, 00130, 00730, 00674, 01020,
01674, 00330, 00330, 00330, 00030, 01030, 00430, 01574,
01020, 01174, 00330, 00330, 00330, 00330, 00374, 01020,
01074, 00330, 00330, 00274, 01020, 00032, 00226, 00372,
00264, 01425, 00372, 00333, 00756, 01672, 01352, 01527,
01522, 01464, 01216, 01352, 01514, 00426, 00032, 00556,
00473, 00406, 01562, 01510, 01172, 00412, 00624, 01511,
00422, 00272, 01020, 01672, 01156, 00457, 00416, 00617,
00232, 00716, 00232, 00732, 01552, 01501, 00573, 00232,
00706, 00706, 00706, 00232, 00573, 00756, 01032, 00567,
00472, 01166, 00503, 01362, 01550, 00132, 01032, 01256,
00436, 00372, 00774, 00264, 01531, 00372, 00703, 00762,
01032, 00677, 00472, 00732, 01776, 00372, 00620, 01254,
01553, 01274, 00730, 00222, 00372, 01007, 00172, 01162,
01107, 01646, 00232, 00736, 00232, 01122, 00773, 00132,
00642, 00634, 01614, 01162, 00164, 00451, 00164, 00323,
01646, 01122, 01103, 00022, 00472, 00642, 00773, 01646,
00642, 01007, 00132, 00232, 00410, 00616, 00164, 01255,
01174, 01004, 00164, 00545, 01014, 00426, 00277, 00265,
00064, 01703, 00000, 00410, 01735, 00510, 00064, 00053,
01204, 01014, 00614, 01114, 00774, 00210, 01352, 01734,
00272, 00072, 00166, 00022, 00656, 01443, 01562, 01744,
00774, 01732, 00132, 00642, 00132, 00222, 00406, 01503,
01732, 00662, 01363, 00032, 00226, 00264, 00457, 00742,
01032, 01477, 00472, 00732, 00620, 01354, 01720, 00032,
00232, 00774, 00264, 00127, 00032, 00642, 00164, 00021,
00164, 00321, 00264, 00047, 00774, 00264, 01113, 01604,
00764, 00773, 00434, 00010, 00432, 01310, 01310, 01310,
01735, 00050, 00150, 00250, 00350, 00450, 01273, 00432,
00610, 00432, 00550, 00650, 00750, 01050, 01150, 01020,
00232, 00432, 00774, 00742, 00742, 01772, 00742, 00756,
01020, 00064, 00233, 00064, 00617, 00000, 00000, 00000,
00000, 00000, 00000, 00000, 00000, 00034, 00033, 00055,
01270, 00064, 01703, 00610, 00774, 01342, 00051, 00610,
00434, 00046, 00001, 00164, 00275, 00045, 01250, 00064,
00047, 01621, 01552, 00060, 00742, 00774, 00742, 00177,
01711, 01574, 01755, 01352, 00520, 00374, 01755, 01352,
00437, 00564, 00063, 00034, 00100, 00055, 01370, 00147,
00610, 00474, 01342, 00115, 00610, 00434, 00113, 00001,
00164, 00475, 00045, 01350, 00237, 01621, 01552, 00124,
00742, 00474, 00742, 00423, 01731, 01574, 01755, 01352,
00611, 00374, 01755, 01352, 00557, 00564, 00557, 00034,
00144, 00055, 01470, 00147, 00610, 00674, 01342, 00153,
00610, 01450, 00237, 01621, 01552, 00162, 00742, 00674,
00742, 00643, 01711, 01731, 01574, 01755, 01352, 01133,
00664, 00647, 00034, 00177, 00055, 01570, 00147, 00610,
01404, 01574, 01342, 00207, 00610, 01550, 00237, 01621,
01552, 00216, 00742, 01574, 00742, 01023, 01711, 01731,
00374, 01755, 01352, 01230, 00664, 00773, 00034, 00233,
00055, 01670, 00147, 00610, 00374, 01342, 00242, 00610,
01650, 00237, 01621, 01552, 00251, 00742, 00374, 00742,
01177, 01711, 01731, 01574, 01755, 01352, 01267, 00664,
01107, 01347, 01333, 00357, 00127, 01363, 00424, 00277,
00577, 00424, 00332, 00753, 00424, 00300, 01133, 01304,
00710, 01711, 00620, 01342, 00371, 00620, 01342, 00371,
00774, 01404, 00564, 00213, 01250, 01324, 00321, 00764,
00513, 01070, 00232, 00432, 00742, 00164, 00655, 00750,
00764, 00273, 00710, 01731, 01711, 01574, 01342, 00371,
00774, 00764, 00067, 00000, 00412, 01174, 01342, 00361,
01142, 01342, 00360, 01142, 01342, 00357, 00752, 00742,
00742, 01020, 00474, 01342, 00370, 01020, 00774, 01717,
00610, 00664, 01623, 00412, 01342, 00377, 00752, 01020,
00164, 00663, 00164, 00657, 00164, 00277, 00164, 00477,
00264, 00017, 00264, 00027, 01755, 01370, 01346, 00371,
01715, 00001, 00041, 01750, 01670, 00232, 01570, 01346,
00371, 00031, 00041, 01346, 00512, 01770, 00423, 01755,
00334, 00444, 01470, 00243, 01715, 00001, 01470, 00021,
01750, 01715, 00001, 00041, 01624, 00522, 01366, 00514,
01262, 00410, 01570, 00232, 01370, 00021, 01770, 01346,
00371, 00756, 00756, 00031, 00032, 00662, 01672, 01624,
00527, 00011, 00041, 00510, 00031, 01424, 00314, 00064,
00231, 01250, 00064, 01703, 01570, 00232, 01470, 00423,
01604, 00177, 01366, 00531, 00410, 01670, 00317, 00001,
00413, 01670, 00467, 01755, 01270, 01346, 00371, 00032,
00642, 00031, 00410, 01670, 00232, 01570, 01346, 00371,
00031, 00051, 00432, 00742, 00011, 00664, 00527, 01751,
01270, 00334, 00566, 01150, 01570, 00777, 00232, 00432,
00742, 00011, 01150, 01570, 00232, 01470, 00011, 01750,
01701, 00232, 01470, 01701, 00031, 01750, 01170, 01701,
01273, 01751, 01270, 00334, 00620, 01150, 01670, 01147,
00232, 00432, 00742, 00001, 01150, 01670, 00232, 01470,
00001, 01750, 01170, 00232, 01470, 00021, 01770, 00011,
01372, 00512, 01770, 01701, 00232, 01470, 01701, 00031,
01262, 01750, 01170, 01701, 01262, 01150, 00014, 00774,
00032, 00662, 00642, 00642, 01672, 01770, 00021, 01170,
00232, 00031, 01352, 00720, 00001, 01352, 00725, 00032,
00642, 01170, 00031, 01262, 00410, 01770, 00422, 00051,
00432, 00742, 00011, 01350, 00034, 01016, 00664, 00507,
00032, 00642, 01770, 00031, 01457, 01170, 00232, 01770,
00011, 00001, 01350, 01170, 00031, 00756, 00756, 00756,
01352, 00743, 00004, 00032, 00642, 01170, 00001, 01170,
00021, 01370, 00232, 00031, 01457, 00000, 00000, 00000,
01346, 00371, 01020, 01370, 01156, 01156, 00032, 00662,
01672, 01020, 00704, 00610, 00210, 00774, 00314, 01020,
00164, 00663, 00164, 00657, 00164, 00277, 00164, 00477,
00364, 01303, 00564, 01717, 00064, 00233, 01170, 01262,
00410, 01370, 01372, 01121, 00041, 01050, 00032, 00642,
01370, 00001, 00410, 01170, 00232, 01070, 00021, 01370,
00021, 00510, 00031, 00410, 00032, 00642, 01070, 00011,
01050, 00510, 00011, 00410, 01770, 00232, 01370, 00021,
01070, 00011, 00510, 00031, 01050, 00032, 00642, 00011,
01370, 00021, 01350, 01070, 00021, 00032, 00415, 01070,
00032, 00425, 00073, 00656, 00656, 00656, 00656, 00656,
00656, 00656, 01366, 01121, 01352, 01120, 00516, 00507,
01020, 00432, 01150, 01050, 01370, 00756, 00756, 00061,
01350, 00064, 01703, 01755, 01370, 01372, 01167, 00334,
01143, 01670, 00643, 00051, 00001, 01670, 00232, 00031,
01417, 01755, 01370, 01372, 01171, 00334, 01161, 01570,
00733, 00051, 00001, 01570, 00232, 00031, 01223, 01670,
00753, 01570, 00232, 01270, 00031, 01033, 01761, 01270,
01262, 00410, 00051, 00041, 01670, 00021, 00061, 01624,
01217, 01424, 01215, 01650, 00547, 01550, 00547, 01450,
00547, 01761, 01270, 00410, 00051, 00041, 01570, 01027,
01761, 01370, 01366, 01317, 00334, 01240, 01470, 01223,
00051, 00001, 01470, 00021, 01750, 01270, 01262, 00410,
00051, 00041, 00032, 00642, 00011, 00410, 00051, 00232,
00510, 01624, 01264, 00232, 00031, 01770, 01027, 01761,
01370, 01366, 01317, 00334, 01277, 01470, 01417, 00051,
00001, 01470, 00021, 01750, 01270, 00410, 00051, 00041,
00432, 00742, 00011, 00410, 00051, 00232, 01303, 01270,
00232, 01470, 01027, 00424, 01336, 01504, 00014, 00164,
01367, 00424, 01541, 00004, 01514, 01537, 01004, 01170,
00232, 00770, 00031, 00547, 00432, 00310, 00724, 01354,
01334, 01361, 00750, 01677, 01050, 01150, 01350, 00164,
01703, 01434, 01357, 01034, 01357, 00610, 01677, 00000,
00000, 00000, 00000, 01604, 00610, 00774, 00314, 01020,
00164, 00663, 00232, 00164, 00657, 00164, 00277, 00164,
00477, 00564, 01717, 00364, 01303, 01270, 00232, 00432,
00330, 00630, 00756, 00756, 00774, 00035, 01570, 00025,
01370, 01156, 01156, 00025, 01750, 00432, 00032, 00330,
00630, 00756, 00756, 00272, 00530, 00774, 00035, 01770,
00025, 01310, 01570, 01310, 01770, 01473, 00770, 00232,
01170, 00015, 00410, 00045, 00055, 00032, 00642, 00015,
01750, 01170, 00232, 01270, 00015, 00410, 00045, 00055,
00656, 00656, 01370, 00035, 01770, 00025, 01750, 01170,
00232, 00770, 00015, 01770, 00015, 00432, 00750, 01470,
00025, 01473, 01170, 00232, 01270, 00015, 00410, 00045,
00055, 00032, 00642, 00015, 00656, 00656, 01370, 00035,
00477, 01004, 01170, 00272, 00025, 00770, 00035, 01070,
00011, 00410, 01424, 01601, 00032, 00642, 00770, 00011,
00510, 00232, 00035, 01262, 00432, 00774, 00530, 01156,
00774, 00410, 00232, 00264, 00025, 01473, 00610, 01404,
00607, 01170, 00232, 00670, 00025, 00770, 00035, 00570,
00011, 00510, 00035, 01624, 01741, 01577, 00000, 00000,
00000, 00272, 00410, 01170, 00434, 01630, 00011, 01147,
00001, 01771, 01150, 00510, 00272, 00025, 01070, 00434,
01643, 00011, 01223, 00001, 01771, 01050, 01210, 00510,
00025, 00570, 00434, 01656, 00011, 01277, 00001, 01771,
00550, 01210, 00670, 00434, 01667, 00011, 01343, 00001,
01771, 00650, 00032, 00642, 00770, 00434, 01701, 00011,
01413, 00001, 01771, 00750, 00064, 00047, 01010, 00232,
01310, 00424, 01720, 00025, 01156, 01156, 00064, 01703,
01004, 00015, 01110, 01310, 01156, 01156, 00035, 01473,
00424, 01576, 01110, 01473, 00272, 01170, 01523, 00710,
00025, 00610, 01170, 00232, 00570, 00025, 00770, 00035,
01750, 00670, 00232, 01070, 00025, 00770, 00035, 01770,
00015, 00510, 00035, 00710, 01624, 01773, 00001, 00432,
00610, 00232, 01473, 01771, 01310, 01737, 00064, 00233
};
