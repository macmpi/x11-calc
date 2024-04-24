/*
 * x11-calc-cpu.c - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2020   MT
 *
 * Provides a way to simulate the execution of each instruction used by the
 * processors in several different families of HP calculator.
 *
 * This processor simulator is based on the work of a number of individuals
 * including  Jacques LAPORTE, David HICKS, Greg SYDNEY-SMITH, Eric  SMITH,
 * Tony  NIXON and Bernhard EMESE.  Without their efforts and in some cases
 * assistance and encouragement this simulator not have been possible.
 *
 * Thank you.
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
 * 10 Sep 20         - Initial version - MT
 * 08 Aug 21         - Tidied up spelling errors in the comments and  added
 *                     a section describing the processor registers - MT
 *                   - Changed the status word into an integer - MT
 * 10 Aug 21         - Modified register names - MT
 *                   - Defined register size as a constant - MT
 * 16 Aug 21         - Added   tick()  to  execute  a  single  instruction,
 *                     currently this only decodes two  instructions ('nop'
 *                     and  'hi I'm woodstock')  neither of which  actually
 *                     does anything.  Any unrecognised opcodes are  simply
 *                     ignored - MT
 * 19 Aug 21         - Created  two data structures to hold  the  processor
 *                     properties and the register properties and added the
 *                     code to create a new processor instance - MT
 * 20 Aug 21         - Updated tick() using the new data structures - MT
 * 22 Aug 21         - Can now decode the four main instruction types - MT
 * 24 Aug 21         - Added code to decode the field modifier - MT
 * 26 Aug 21         - Re-implemented load_reg() to use the processor  data
 *                     and renamed it set_register() - MT
 *                   - Reversed order of nibbles in each register to  match
 *                     the actual numbering (when the register format  says
 *                     the left hand nibble is number 13 it means it!) - MT
 *                   - Implemented my first real op code (0 -> a[f]) - MT
 * 27 Aug 21         - Added code to exchange register contents -MT
 * 28 Aug 21         - Created a separate method to display the contents of
 *                     a  register  (replacing the existing debug  code  in
 *                     register load and exchange routines) - MT
 *                   - Implemented routines to add and the contents of  two
 *                     registers  and used this to increment a register  by
 *                     setting the carry flag and then calling add register
 *                     with a single parameter - MT
 *                   - Added an identifier to the register properties  which
 *                     allows  the register name to be shown when  printing
 *                     the value - MT
 * 29 Aug 21         - Tidied up trace output - MT
 *                   - Changed the register names - MT
 * 30 Aug 21         - Changed  carry to use the processor flags, and  made
 *                     the  first  and last values used when selecting  the
 *                     field in a register processor properties - MT
 *                   - Changed all flags and status bits to integers - MT
 *                   - Run mode set by default - MT
 *                   - Tidied up the comments (again) - MT
 *                   - Separated the 'special' instruction into four groups
 *                     to make it easier to parse the parameters - MT
 *                   - Added several more functions - MT
 *                   - Implemented code to handle subroutines - MT
 *                   - Fixed compiler warnings - MT
 *                   - Added ROM select, and register copy - MT
 * 31 Aug 21         - Fixed typo in RAM initialisation code - MT
 *                   - Using a NULL argument with the copy register routine
 *                     means it can be used to replace the function used to
 *                     zero a register - MT
 *                     using the copy routine - MT
 *                   - Added tests for equals and not equals - MT
 *                   - Implemented shift right operations - MT
 *                   - Tidied up code used to trace execution - MT
 *                   - Unimplemented operations now generate errors - MT
 *                   - Removed superfluous breaks - MT
 *  1 Sep 21         - Removed  separate routines to set and  clear  status
 *                     bits - MT
 *                   - Added  a  routine  to display the  contents  of  the
 *                     registers and processor flags - MT
 *                   - Implemented  f -> a, if s(n) = 0, p = n,  if nc goto
 *                     instructions - MT
 *  2 Sep 21         - Added  a routine to subtract two registers,  setting
 *                     carry flag beforehand and passing a single parameter
 *                     allows it to be used to decrement a register - MT
 *                   - Carry cleared at the beginning of each tick - MT
 *                   - Added a subroutine to handle go to - MT
 *  4 Sep 21         - Sorted out bug in p != 0 test - MT
 *  5 Sep 21         - Moved trace output to the start of each  instruction
 *                     to make debugging easier - MT
 *                   - Fixed calculation of 8-bit branch addresses - MT
 *                   - Made  the handling of the carry flag consistent  (so
 *                     that  incrementing the program counter always clears
 *                     the carry flag and set the previous carry) - MT
 *                   - Implemented code used to read a key code and jump to
 *                     the correct address in memory - MT
 *                   - Added a keycode and keystate properties to store the
 *                     key  code of the key and the state of the actual key
 *                     (necessary as clearing status bit 15 when the key is
 *                     released does NOT work!) - MT
 *                   - Modified  code to allow status bit 15 to be  cleared
 *                     if a key is not pressed - MT
 *  8 Sep 21         - Stack operations now work (ENTER, X-Y, and R) - MT
 *  9 Sep 21         - Modified add and subtract to specify the destination
 *                     separately and allow NULL arguments - MT
 *                   - Logical shift clears carry flags - MT
 *                   - Don't clear status bits when tested! - MT
 *                   - Added 'y -> a', '0 - c - 1 -> c[f]', 'if a >= c[f]',
 *                     'a - c -> c[f]',  and 'if a >= b[f]' - MT
 *                   - Basic arithmetic operations now work!! - MT
 *                   - Fixed bug in clear s - MT
 * 10 Sep 21         - Restructured  decoder  for type 2  instructions  and
 *                     implemented load n - MT
 * 14 Sep 21         - Fixed  'n -> c' (registers were reversed) - MT
 *                   - Implemented '0 - c - 1 -> c[f]' - MT
 *                   - Removed  'return'  function(only two lines and  used
 *                     just once) - MT
 *                   - Modified some error messages - MT
 *                   - Tidied up extra spaces - MT
 *                   - HP model 21 simulator works !!
 * 15 Sep 21         - Added 'clear data registers', and delayed ROM select
 *                     handling to 'jsb' and 'goto' - MT
 *                   - Moved  the processor initialisation into a  separate
 *                     routine  to allow main routine to do a reset without
 *                     having to exit and restart the program - MT
 * 16 Sep 21         - Hopefully now handles bank switching better - MT
 * 22 Sep 21         - Added 'c -> data address' - MT
 *                   - Added the line number to the unexpected opcode error
 *                     message - MT
 * 23 Sep 21         - Fixed 'go to' address calculation (hopefully) - MT
 *                   - Fixed bug in 'p - 1 -> p' - MT
 * 25 Sep 21         - Updated register debug output - MT
 *                   - Changed DATA_REGISTERS to MEMORY_SIZE - MT
 * 26 Sep 21         - Display register contents when debugging - MT
 *                   - Implemented 'c -> data' and 'data -> c' - MT
 *                   - Added  range checking to 'data register(n)-> c'  and
 *                     'c -> data address' - MT
 * 30 Sep 21         - Added routines to clear flags and status bits - MT
 *                   - Added properties for enable and select - MT
 *                   - For  the HP model 21 and 22 the state of the  select
 *                     switch is checked when setting or clearing S 3 - MT
 *  1 Oct 21         - Converted flags to Boolean variables - MT
 *  4 Oct 21         - Added 'a -> rom address' - MT
 *  6 Oct 21         - Dumps all registers when displaying status - MT
 *  7 Oct 21         - Fixed bug in 'data register(n)-> c' - MT
 *                   - Fixed bug in 'clear registers' - MT
 *                   - HP model 25 simulator now working..
 * 10 Oct 21         - Fixed bug in code to check the ROM size - MT
 *                   - Added 'keys -> a' and 'keys -> a' - MT
 *                   - Fixed bug in 'c -> data register(n)' - MT
 * 14 Oct 21         - Added  processor_save() and processor_restore()  and
 *                     modified  'clear data registers' to prevent it  from
 *                     clearing  the  registers  if  continuous  memory  is
 *                     enabled (bit of a 'kuldge') - MT
 * 16 Oct 21         - Checks for continuous memory when clearing registers
 *                     only when executing an instruction - MT
 *                   - Forgot to close the data file - MT
 *                   - Fixed bug with program counter (do not decrement the
 *                     program  counter  before checking for a delayed  ROM
 *                     select) - MT
 *                   - HP model 29 simulator works..!
 *                   - Fixed bug in 'keys -> rom address' - MT
 *                   - HP model 27 simulator works (requires testing).
 * 17 Oct 21         - Renamed init() to reset() and fixed bug in init() by
 *                     consolidating  the code used to clear the  registers
 *                     into reset() - MT
 *                   - Modified  'p = p + 1' as woodstock and spice  series
 *                     machines behave differently - MT
 *                   - HP  model 31E and 32E simulators work (more testing
 *                     required).
 *                   - Added dummy 'rom checksum' instruction - MT
 * 20 Oct 21         - Modified 'rom checksum' to set status bit and return
 *                     and ignores invalid memory addresses (prints warning
 *                     message) - MT
 * 26 Oct 21         - Added processor_load() to load saved registers  from
 *                     a file - MT
 * 17 Nov 21         - Fixed pathname on VMS - MT
 * 22 Nov 21         - Only saves the state of continuous registers - MT
 * 25 Nov 21         - Removed current rom property as this is not used and
 *                     added the rom bank property, since this is needed by
 *                     the HP model 34C - MT
 *                   - Implemented bank switching - MT
 *                   - Updated  the display format used to show the address
 *                     in any error or warning messages to include the bank
 *                     number - MT
 *                   - Fixed  bug in clear registers (it was  clearing  the
 *                     processor stack which it should not) - MT
 *                   - Only  loads and saves a subset of the registers when
 *                     the set of persistent registers is defined - MT
 * 27 Nov 21         - Mode the register display less verbose by creating a
 *                     separate routine to display memory - MT
 * 28 Nov 21         - Made the trace flag a processor property - MT
 *                   - Fixed bug when implementing the p register and moved
 *                     the code to modify p into a couple of functions - MT
 *                   - Fixed missing break in bank switch - MT
 *                   - Removed unnecessary data validation - MT
 *                   - Made the single step flag a processor property - MT
 *                   - Saves the last opcode executed - MT
 *                   - Incrementing the p register now works properly!
 *                     Literally the only way to get it to work properly is
 *                     to check the previous opcode if the current value is
 *                     zero and ignore it if it is the same. I would really
 *                     like to know why it works this way - MT
 * 02 Dec 21         - Modified  check above to compare previous opcode with
 *                     the current opcode (not a hard coded value) - MT
 *                   - Removed any references to TRACE - MT
 * 11 Dec 21         - Print flags as a hex value - MT
 *                   - Started to add additional instructions and processor
 *                     flags to support the HP model 67 - MT
 * 17 Dec 21         - Fixed bug in clear data registers - MT
 * 19 Dec 21         - Unused load register routine deleted - MT
 *                   - Bank switch sets flag - MT
 *                   - Jump to subroutine and return now saves and restores
 *                     the current bank number (it is amazing that this bug
 *                     didn't cause problems earlier) - MT
 * 20 Dec 21         - Discovered that on the HP model 67 when the  program
 *                     counter is less than 02000 it automatically switches
 *                     to memory bank 0 - MT
 * 21 Dec 21         - Fixed implicit bank switching - MT
 *                   - Modified 'clear data registers' so it does not clear
 *                     the memory at power on for the HP model 67  allowing
 *                     it to behave as if it has continuous memory - MT
 * 22 Dec 21         - Uses model numbers for conditional compilation - MT
 * 25 Dec 21         - A jump to ROM 0 always switches to bank 0 - MT
 * 29 Dec 21         - Added conditional compilation options to support the
 *                     classic ACT processor in the HP model 35, 45, 80, 70
 *                     and 55 - MT
 * 30 Dec 21         - Resetting the processor doesn't reset trace and step
 *                     properties - MT
 *                   - Fixed display of target address in 'then go to' - MT
 * 31 Dec 21         - Having fixed 'clear data registers' keeping track of
 *                     which registers are persistent is not a  requirement
 *                     anymore - MT
 * 02 Jan 22         - Fixed bug in 'select rom' (program counter _must_ be
 *                     incremented _before_ updating the address) - MT
 *                   - Defined text messages as string constants.  It would
 *                     be  better to use separate language  specific module
 *                     but I can't figure out how to make it work with some
 *                     of the older C compilers - MT
 *                   - Addresses and opcodes can be displayed using  either
 *                     Octal or Hexadecimal (selected at compile time) - MT
 * 03 Jan 22         - Changed debug() macro so that debug code is executed
 *                     when DEBUG is defined (doesn't need to be true) - MT
 * 07 Jan 22         - Fixed bug in 'return' - MT
 *                   - Fixed bug in 'delayed select rom' - MT
 * 10 Jan 22         - Program counter is now incremented immediately after
 *                     the  next opcode is fetched which makes it easier to
 *                     calculate the correct target address of  conditional
 *                     or  unconditional branch instructions and subroutine
 *                     calls correctly - MT
 * 11 Jan 22         - Since  the program counter is now incremented before
 *                     the current opcode is decoded any instructions  that
 *                     modified the program counter were updated an in most
 *                     cases simplified - MT
 *                   - Removed the processor bank property and any explicit
 *                     references to the ROM bank. Note that bank switching
 *                     assumes there are only two ROM banks - MT
 * 12 Jan 22         - Modified code sets the processor status bit based on
 *                     the select property - MT
 *                   - Added a timer property to the processor to allow the
 *                     processor  status bit to be set appropriately if the
 *                     timer is enabled - MT
 *                   - Updated  inc_p() to get the current opcode from  the
 *                     current address - MT
 *                   - Program counter wraps around to the start of the ROM
 *                     in the Classic series, but wraps around to the start
 *                     of the current bank in later models - MT
 * 16 Jan 21         - The behaviour of 'p + 1 -> p' on the HP67 is not the
 *                     same as the woodstock series and it actually appears
 *                     to match the later spice series - MT
 * 21 Jan 22         - Moved text messages to a separate file  - MT
 * 23 Jan 22         - Removed unwanted debug code - MT
 *                   - Updated 'select rom' - MT
 * 30 Jan 22         - Tidied up the conditional code blocks a bit, putting
 *                     the Type 0, Type 1, Type 2 and Type 3 opcodes in the
 *                     same place - MT
 *                   - Added a second pointer register 'q' and pointer flag
 *                     to select which pointer is active - MT
 *                   - Added  arithmetic operations to support the  voyager
 *                     series - MT
 * 07 Feb 22         - Tidied up the code by using a function to return the
 *                     address of the active pointer - MT
 * 12 Feb 22         - Added a new property to allow the processor to sleep
 *                     between key strokes and the additional registers and
 *                     flags used by the NUT processor - MT
 * 22 Feb 22         - Displays the contents of any changed registers (10C,
 *                     11C, 12C, 15C and 16C only) - MT
 * 28 Feb 22         - Can now read ROM contents from a file - MT
 * 01 Mar 22         - Branch to subroutine now calls op_jsb(), and 'rstkb'
 *                     now clears the carry flag - MT
 * 02 Mar 22         - Fixed 'm = c', 'c = m', and 'cmex' (forgot about the
 *                     field size) - MT
 *                   - Changed the 'g' register from an integer to an array
 *                     to make it simpler to reference each nibble - MT
 * 03 Mar 22         - Read ROM file using octal if hexadecimal is not used
 *                     to display opcodes - MT
 *                   - Added a minimal implementation of 'display blink' to
 *                     simply  enable the display (actually getting  it  to
 *                     blink is done in hardware) - MT
 * 04 Mar 22         - Debug  now displays the ROM contents in the  correct
 *                     format - MT
 * 04 Mar 22         - The HP10C, HP11C, HP12C, HP15C and HP16C include the
 *                     state  of the processor when saving or restoring the
 *                     contents of memory - MT
 * 05 Mar 22         - Changed the data type of the register nibbles, flags
 *                     and status bits to unsigned char (reading each value
 *                     into a temporary unsigned int first allows fscanf to
 *                     be used to read the saved values) - MT
 * 06 Mar 22         - Fixed the issue with the relative jump offset - MT
 *                   - Added 'cstex' instruction (exchange c and st)- MT
 *                   - Added 'c =st', 'c = stk', 'c = c and a', and fixed a
 *                     bug in 'cstex' - MT
 * 07 Mar 22         - Removed unused debug code - MT
 * 31 Mar 22         - Modified to compile on NetBSD - MT
 * 12 May 22         - A branch to subroutine instruction is ignored if the
 *                     target address does not exist - MT
 *                   - Print a newline to stdout before an error message if
 *                     trace is enabled, to tidy up the output - MT
 * 26 Nov 22         - Added the original HP10 (based on the assumption  it
 *                     uses the same processor as the woodstock series).
 * 03 Dec 22         - Changed the behaviour of the 'p' register in th HP10
 *                     to match the later spice series.
 * 07 Dec 22         - Changed the label used for the 'p' register when the
 *                     registers are displayed  - MT
 *                   - All models now display the modified registers values
 *                     in the trace output - MT
 * 11 Dec 22         - Renamed models with continious memory and added HP25
 *                     HP33E, and HP38E - MT
 * 14 Dec 22         - Changed 'keys -> a' to return the state of the print
 *                     mode  switch  for  printing models,  currently  this
 *                     always selects the 'manual' mode as printing has not
 *                     been implemented yet - MT
 * 20 Dec 22         - Can now print to stdout, this works well enough with
 *                     the HP10 as this doesn't use many special characters
 *                     and they can be substituted for an ASCII equlivelent
 *                     but this will be a problem with other models - MT
 * 21 Dec 22         - Created a seperate function to print the contents of
 *                     the print buffer to a file - MT
 * 24 Dec 22         - Added and explicit check for '__APPLE__' in order to
 *                     allow Mac OS  to be handled in the same way as other
 *                     unix like systems - MT
 * 28 Dec 22         - Changed the name of printer mode status from mode to
 *                     print - MT
 *                   - Changed the name of prgm/run mode status from select
 *                     to mode - MT
 * 01 May 23         - Corrected default mode setting -
 * 12 Jan 23         - Tidied up some of the processor trace output - MT
 * 06 Jun 23         - Removed unused references to HP91c and HP97 - MT
 * 21 Oct 23         - Define MANUAL, NORMAL, and TRACE print modes - MT
 * 04 Feb 24         - Found  and  fixed a memory leak in save_state()  and
 *                     restore_state() - MT
 *                   - Added a seperate function to return the path to  the
 *                     data  file that is used to store the processor state
 *                     and modified the code to store the data file in  the
 *                     $HOME/.local/share directory  if it doesn't  already
 *                     exist in the $HOME directory, or the use the current
 *                     directory if $HOME is not defined as before - MT
 * 10 Feb 24         - Clears  the ROM before reading the new ROM  contents
 *                     from the specified file - MT
 * 12 Feb 24         - Will use $XDG_DATA_HOME if the it is defined and the
 *                     directory exists - MT
 *                   - Do NOT clear the ROM before loading a new ROM from a
 *                     file as this allows a ROM file to be used to apply a
 *                     patch to an existing ROM (it seemed like a good idea
 *                     at the time) - MT
 * 16 Feb 24         - Creates a seperate application folder (if it doesn't
 *                     exist) if using $XDG_DATA_HOME or $HOME/.local/share
 *                     to store the data files - MT
 *                   - Fixed bug in $XDG_DATA_HOME checking code - MT
 * 17 Feb 24         - Only uses hidden data files in the $HOME folder - MT
 *                   - Fixed get_datafile_path() - again. Hopefully it will
 *                     work properly now - MT
 * 19 Feb 24         - Closes ROM file after reading - MT
 * 23 Feb 24         - Fixed bug in read_rom() - MT
 * 03 Mar 24         - Updated error handling (now passes the  error number
 *                     to the error handler) - MT
 * 21 Mar 24         - Fixed display position (12C 15C and 16C)- MT
 * 29 Mar 24         - Fixed undefined behavior warnings - MT
 *                   - Fixed unused result compiler warnings - MT
 *                   - Fixed array subscript and string operation  compiler
 *                     warnings -MT
 * 23 Apr 24         - Separated out prototypes for error handlers - MT
 *
 * To Do             - Finish adding code to display any modified registers
 *                     to every instruction.
 *                   - Use a dedicated procesor flag for the printer mode.
 *                   - Figure out how to get the display to blink..?
 *
 */

#define NAME           "x11-calc-cpu"
#define BUILD          "0171"
#define DATE           "29 Mar 24"
#define AUTHOR         "MT"

#include <errno.h>     /* errno */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "x11-calc-messages.h"
#include "x11-calc-errors.h"

#include "x11-calc-label.h"
#include "x11-calc-switch.h"
#include "x11-calc-button.h"

#include "x11-calc-cpu.h"

#include "gcc-debug.h"  /* print() */
#include "gcc-exists.h" /* i_isfile(), i_isdir(), i_exists() */

#if defined(unix) || defined(__unix__) || defined(__APPLE__)
#include <sys/stat.h>
#endif

static void v_fprint_register(FILE *h_file, oregister *h_register) /* Print the contents of a register */
{
   const char c_name[8] = {'A', 'B', 'C', 'Y', 'Z', 'T', 'M', 'N'};
   int i_count;
   if (h_register != NULL)
   {
      fprintf(h_file, "\treg[");
      if (h_register->id < 0)
         fprintf(h_file, "\'%c\'", c_name[h_register->id * -1 - 1]);
      else
         fprintf(h_file, "%03d", h_register->id);
      fprintf(h_file, "] = 0x");
      for (i_count = REG_SIZE - 1; i_count >=0 ; i_count--)
         fprintf(h_file, "%1x", h_register->nibble[i_count]);
   }
}

static void v_fprint_status(FILE *h_file, oprocessor *h_processor) /* Display the current processor status word */
{
   int i_count, i_temp = 0;
   for (i_count = STATUS_BITS - 1; i_count >= 0; i_count--)
   {
      i_temp <<= 1;
      if (h_processor->status[i_count]) i_temp |= 1;
   }
   fprintf(h_file, "\tstatus = 0x%04X%11c  ", i_temp, ' ');
}

static void v_fprint_flags(FILE *h_file, oprocessor *h_processor) /* Display the current processor flags */
{
   int i_count, i_temp = 0;
   for (i_count = 0; i_count < FLAGS; i_count++)
      i_temp += h_processor->flags[i_count] << i_count;
   fprintf(h_file, "\tflags = 0x%04X%12c   ", i_temp, ' ');
}

#if defined(HP10)
static void v_fprint_buffer(FILE *h_file, oprocessor *h_processor) /* Display the current processor flags */
{
   static const unsigned char c_charmap[0x40] = {
      ' ', 'Y', '=', '0', 'L', 'M', ' ', '1', 'G', ' ', '>', '2', 'O', 'H', ' ', '3',  /* ' ', ' ', '=', '0', 'L', 'M', '≠', '1', 'G', '¿', '>', '2', 'O', 'H', '≤', '3', */
      'P', ' ', 'X', '4', 'R', 'F', 'Z', '5', 'S', '?', 'x', '6', 'T', ' ', ' ', '7',  /* 'P', '√', 'X', '4', 'R', 'F', 'Z', '5', 'S', '?', 'x', '6', 'T', '→', '⇔', '7', */
      '%', ' ', ' ', '8', 'J', 'X', '>', '9', 'A', '#', 'K', '.', 'B', 'b', '/', '-',  /* '%', ' ', '¿', '8', 'J', 'X', '>', '9', 'A', '#', 'K', '.', 'B', 'b', '/', '-', */
      'C', 'c', '/', '+', 'D', 'd', ' ', '#', 'E', 'e', ' ', ' ', 'I', 'i', 'x', ' '   /* 'C', 'c', '÷', '+', 'D', 'd', '↑', '#', 'E', 'e', '↓', ' ', 'I', 'i', 'x', ' '  */
      };

   if (h_processor->position < BUFSIZE) /* Are there any characters in the buffer? */
   {
      int i_count;
      for (i_count = 0; (i_count < BUFSIZE); i_count++) /* Print the contents of the buffer */
         fprintf(h_file, "%c", c_charmap[h_processor->buffer[i_count]]);
      fprintf(h_file, "\n");
      h_processor->position = BUFSIZE; /* Clear the buffer contents */
      for (i_count = 0; i_count < BUFSIZE; i_count++)
         h_processor->buffer[i_count] = 0x3f;
   }
}
#endif

void v_fprint_registers(FILE *h_file, oprocessor *h_processor) /* Display current register contents */
{
   if (h_processor != NULL)
   {
      int i_count;
      for (i_count = 0; i_count < REGISTERS; i_count++)
      {
         if ((i_count % 3 == 0) && (i_count > 0)) fprintf(h_file, "\n");
         v_fprint_register(h_file, h_processor->reg[i_count]);
      }
      fprintf(h_file, "\n");
      for (i_count = 0; i_count < MEMORY_SIZE; i_count++)
      {
         if (i_count % 3 == 0) fprintf(h_file, "\n");
         v_fprint_register(h_file, h_processor->mem[i_count]);
      }
      fprintf(h_file, "\n");
      v_fprint_flags(h_file, h_processor);
      v_fprint_status(h_file, h_processor);
      fprintf(h_file, "\tp = %d\t\t", h_processor->p);
      fprintf(h_file, "  addr = %02d\n", h_processor->addr);
   }
}

void v_fprint_memory(FILE *h_file, oprocessor *h_processor) /* Display current memory contents */
{
   if (h_processor != NULL)
   {
      int i_count;
      if (MEMORY_SIZE > 1) { /* Bit of a fudge but... */
         for (i_count = 0; i_count < MEMORY_SIZE; i_count++)
         {
            if (i_count % 3 == 0) fprintf(h_file, "\n\t\t");
            v_fprint_register(h_file, h_processor->mem[i_count]);
         }
         fprintf(h_file, "\n");
      }
   }
}

oregister *h_register_create(int i_id) /* Create a new register , */
{
   oregister *h_register; /* Pointer to register */
   int i_count, i_temp;
   if ((h_register = malloc (sizeof(*h_register))) == NULL)
   {
      fprintf(stdout, h_err_register_alloc, __FILE__, __LINE__);
   }
   i_temp = sizeof(h_register->nibble) / sizeof(*h_register->nibble);
   h_register->id = i_id;
   for (i_count = 0; i_count < i_temp; i_count++)
      h_register->nibble[i_count] = 0;
   return(h_register);
}

static void v_reg_exch(oprocessor *h_processor, oregister *h_destination, oregister *h_source) /* Exchange the contents of two registers */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++)
   {
      i_temp = h_destination->nibble[i_count];
      h_destination->nibble[i_count] = h_source->nibble[i_count];
      h_source->nibble[i_count] = i_temp;
   }
}

static void v_reg_copy(oprocessor *h_processor, oregister *h_destination, oregister *h_source) /* Copy the contents of a register */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++)
   {
      if (h_source != NULL) i_temp = h_source->nibble[i_count]; else i_temp = 0;
      h_destination->nibble[i_count] = i_temp;
   }
}

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
static void v_reg_or(oprocessor *h_processor, oregister *h_destination, oregister *h_source, oregister *h_argument) /* Or the contents of two registers */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++){
      if (h_argument != NULL) i_temp = h_argument->nibble[i_count]; else i_temp = 0;
      h_destination->nibble[i_count] = h_source->nibble[i_count] | i_temp;
   }
}

static void v_reg_and(oprocessor *h_processor, oregister *h_destination, oregister *h_source, oregister *h_argument) /* And the contents of two registers */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++){
      if (h_argument != NULL) i_temp = h_argument->nibble[i_count]; else i_temp = 0;
      h_destination->nibble[i_count] = h_source->nibble[i_count] & i_temp;
   }
}
#endif

static void v_reg_add(oprocessor *h_processor, oregister *h_destination, oregister *h_source, oregister *h_argument) /* Add the contents of two registers */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++){
      if (h_argument != NULL) i_temp = h_argument->nibble[i_count]; else i_temp = 0;
      i_temp = h_source->nibble[i_count] + i_temp;
      if (h_processor->flags[CARRY]) i_temp++;
      if (i_temp >= h_processor->base)
      {
         i_temp -= h_processor->base;
         h_processor->flags[CARRY] = True;
      }
      else
      {
         h_processor->flags[CARRY] = False;
      }
      if (h_destination != NULL) h_destination->nibble[i_count] = i_temp & 0x0f; /* Destination can be null */
   }
}

static void v_reg_sub(oprocessor *h_processor, oregister *h_destination, oregister *h_source, oregister *h_argument) /* Subtract the contents of two registers */
{
   int i_count, i_temp;
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++)
   {
      if (h_argument != NULL) i_temp = h_argument->nibble[i_count]; else i_temp = 0;
      if (h_source != NULL) i_temp = (h_source->nibble[i_count] - i_temp); else i_temp = (0 - i_temp);
      if (h_processor->flags[CARRY]) i_temp--;
      if (i_temp < 0)
      {
         i_temp += h_processor->base;
         h_processor->flags[CARRY] = True;
      }
      else
         h_processor->flags[CARRY] = False;
      if (h_destination != NULL) h_destination->nibble[i_count] = i_temp & 0x0f; /* Destination can be null */
   }
}

static void v_reg_test_eq(oprocessor *h_processor, oregister *h_destination, oregister *h_source) /* Test if registers are equal */
{
   int i_count, i_temp;
   h_processor->flags[CARRY] = True; /* Clear carry - Do If True */
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++)
   {
      if (h_source != NULL) i_temp = h_source->nibble[i_count]; else i_temp = 0;
      if (h_destination->nibble[i_count] != i_temp)
      {
         h_processor->flags[CARRY] = False; /* Set carry - Do If True */
         break;
      }
   }
}

static void v_reg_test_ne(oprocessor *h_processor, oregister *h_destination, oregister *h_source) /* Test if registers are not equal */
{
   v_reg_test_eq(h_processor, h_destination, h_source);
   h_processor->flags[CARRY] = !h_processor->flags[CARRY];
}

static void v_reg_inc(oprocessor *h_processor, oregister *h_register) /* Increment the contents of a register */
{
   h_processor->flags[CARRY] = True; /* Set carry */
   v_reg_add (h_processor, h_register, h_register, NULL); /* Add carry to register */
}

static void v_reg_shr(oprocessor *h_processor, oregister *h_register) /* Logical shift right a register */
{
   int i_count;
   h_processor->flags[CARRY] = False; /* Clear carry */
   for (i_count = h_processor->first; i_count <= h_processor->last; i_count++)
   {
      if (i_count == h_processor->last)
         h_register->nibble[i_count] = 0;
      else
         h_register->nibble[i_count] = h_register->nibble[i_count + 1];
   }
}

static void v_reg_shl(oprocessor *h_processor, oregister *h_register) /* Logical shift left a register */
{
   int i_count;
   for (i_count = h_processor->last; i_count >= h_processor->first; i_count--)
   {
      if (i_count == h_processor->first)
         h_register->nibble[i_count] = 0;
      else
         h_register->nibble[i_count] = h_register->nibble[i_count - 1];
   }
   h_processor->flags[PREV_CARRY] = h_processor->flags[CARRY] = False;
}

void v_read_rom(oprocessor *h_processor, char *s_pathname) /* Load rom from 'object' file */
{
   FILE *h_file;
   unsigned int i_addr, i_opcode;
   int i_count, i_counter;
   char c_char;

   h_file = fopen(s_pathname, "r");
   if (h_file != NULL)
   {
      i_count = 0;
      while ((!feof(h_file)) && (i_count < ROM_SIZE))
      {
         i_counter = fscanf(h_file, h_msg_rom, &i_addr, &i_opcode);
         if (i_counter < 2)
            while (((c_char = fgetc(h_file)) != '\n') && (!feof(h_file)));
         else
         {
            while ((i_count < i_addr) && (i_count < ROM_SIZE))
               /** i_rom[i_count++] = 0; */
               i_count++; /* Don't clear ROM */
            if (i_count < ROM_SIZE) i_rom[i_count++] = i_opcode;
         }
      }
      fclose(h_file);
   }
   else
      v_error(errno, h_err_opening_file, s_pathname); /* Can't open data file */
}

void v_read_state(oprocessor *h_processor, char *s_pathname) /* Read processor state from file */
{
#if defined(CONTINIOUS)
   FILE *h_file;
   unsigned int i_temp;
   int i_count, i_counter;

   if ((h_processor != NULL) && (s_pathname != NULL)) { /* Check processor and pathname are defined */
      h_file = fopen(s_pathname, "r");
      if (h_file !=NULL) { /* If file exists and can be opened restore state */
         fprintf(stderr,h_msg_loading, s_pathname);
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
         for (i_count = 0; i_count < FLAGS; i_count++)
         {
            if (fscanf(h_file, "%x,", &i_temp)) h_processor->flags[i_count] = i_temp;
         }
         for (i_count = 0; i_count < STATUS_BITS; i_count++)
         {
            if (fscanf(h_file, "%x,", &i_temp)) h_processor->status[i_count] = i_temp;
         }
         for (i_count = 0; i_count < REGISTERS; i_count++)
            for (i_counter = REG_SIZE - 1; i_counter >= 0 ; i_counter--)
            {
               if (fscanf(h_file, "%x,", &i_temp)) h_processor->reg[i_count]->nibble[i_counter] = i_temp;
            }
         if (fscanf(h_file, "%x,", &i_temp)) h_processor->p = i_temp;
         if (fscanf(h_file, "%x,", &i_temp)) h_processor->q = i_temp;
         if (fscanf(h_file, "%x,", &i_temp)) h_processor->f = i_temp;
         if (fscanf(h_file, "%x,", &i_temp)) h_processor->g[0] = i_temp;
         if (fscanf(h_file, "%x,", &i_temp)) h_processor->g[1] = i_temp;
#endif
         for (i_count = 0; i_count < MEMORY_SIZE; i_count++)
            for (i_counter = REG_SIZE - 1; i_counter >= 0 ; i_counter--)
            {
               if (fscanf(h_file, "%x,", &i_temp)) h_processor->mem[i_count]->nibble[i_counter] = i_temp;
            }
         fclose(h_file);
      }
      else
         v_warning(h_err_opening_file, s_pathname); /* Can't open data file */
   }
#endif
}

void v_write_state(oprocessor *h_processor, char *s_pathname) /* Write processor state to file */
{
#if defined(CONTINIOUS)
   FILE *h_file;
   int i_count, i_counter;

   if ((h_processor != NULL) && (s_pathname != NULL)) { /* Check processor and path name are defined */
      h_file = fopen(s_pathname, "w");
      if (h_file !=NULL) { /* If file exists and can be opened save state */
         fprintf(stderr,h_msg_saving, s_pathname);
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
         for (i_count = 0; i_count < FLAGS; i_count++)
         {
            fprintf(h_file, "%02x,", h_processor->flags[i_count]);
         }
         fprintf(h_file,"\n");
         for (i_count = 0; i_count < STATUS_BITS; i_count++)
         {
            fprintf(h_file, "%02x,", h_processor->status[i_count]);
         }
         fprintf(h_file,"\n");
         for (i_count = 0; i_count < REGISTERS; i_count++)
         {
            for (i_counter = REG_SIZE - 1; i_counter >= 0 ; i_counter--)
               fprintf(h_file, "%02x,", h_processor->reg[i_count]->nibble[i_counter]);
            fprintf(h_file,"\n");
         }
         fprintf(h_file, "%02x,", h_processor->p);
         fprintf(h_file, "%02x,", h_processor->q);
         fprintf(h_file, "%02x,", h_processor->f);
         fprintf(h_file, "%02x,", h_processor->g[0]);
         fprintf(h_file, "%02x,", h_processor->g[1]);
         fprintf(h_file,"\n");
#endif
         for (i_count = 0; i_count < MEMORY_SIZE; i_count++)
         {
            for (i_counter = REG_SIZE - 1; i_counter >= 0 ; i_counter--)
               fprintf(h_file, "%02x,", h_processor->mem[i_count]->nibble[i_counter]);
            fprintf(h_file,"\n");
         }
         fclose(h_file);
      }
      else
         v_warning(h_err_opening_file, s_pathname); /* Can't open data file */
   }
#endif
}

#if defined(CONTINIOUS)
char *v_get_datafile_path(oprocessor *h_processor) /* Returns path the the data file */
/*
 * Returns the path to the data file use to store the machine state when it
 * is 'powered off'.
 *
 *  - If $HOME is defined and the data file already exists in there  return
 *    the pathname of the data file in $HOME to maintain compatibility with
 *    earlier releases.
 *
 *  - If the data file in not in the $HOME folder then if $XDG_DATA_HOME is
 *    defined or $HOME/.local/share/ exists this routine will search for an
 *    application specific subdirectory in the first of these two locations
 *    if finds (creating it if necessary) and will use this to generate the
 *    pathname of the data file.
 *
 *  - Otherwise it will use the current directory path to generate the data
 *    file's pathname.
 *
 */
{
   char *s_directory = getenv("HOME");
   char s_filename[] = FILENAME;
   char s_filetype[] = ".dat";
   char *s_pathname;

   if (s_directory == NULL) s_directory = ""; /* Use current folder if HOME not defined */
#if defined(unix) || defined(__unix__) || defined(__APPLE__)
   s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype) + 2) * sizeof(char*));
   strcpy(s_pathname, s_directory);
   strcat(s_pathname, "/.");
   strcat(s_pathname, s_filename);
   strcat(s_pathname, s_filetype);
   if (!(i_isfile(s_pathname))) /* File does not exists in home or current directory continue searching... */
   {
      free(s_pathname);
      s_directory = getenv("XDG_DATA_HOME");
      if (s_directory) /* XDG_DATA_HOME is defined so atempt to use it */
      {
         s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype) + 10) * sizeof(char*));
         strcpy(s_pathname, s_directory);
      }
      else /* Otherwise try to use $HOME/.local/share */
      {
         s_directory = getenv("HOME");
         s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype) + 23) * sizeof(char*));
         strcpy(s_pathname, s_directory);
         strcat(s_pathname, "/.local/share");
      }
      debug(printf("Searching for '%s'\n", s_pathname));
      if (i_exists(s_pathname) && i_isdir(s_pathname)) /* Check that the selected directory exists, and if it doesn't use  $HOME */
      {
         strcat(s_pathname, "/x11-calc");
         if (i_exists(s_pathname) == 0) mkdir(s_pathname, (S_IRWXU|S_IRGRP|S_IXGRP)); /* If the application data folder does not exist attempt to create it (no need to check status here as we check the directory exists below) */
         if (i_isdir(s_pathname) == 0) /* Check the directory exists and if it doesn't just use $HOME */
         {
            v_warning(h_err_creating_file, s_pathname); /* Can't create directory */
            free(s_pathname);
            s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype) + 2) * sizeof(char*));
            strcpy(s_pathname, s_directory);
            strcat(s_pathname, "/.");
         }
         else
            strcat(s_pathname, "/");
      }
      else
      {
         free(s_pathname);
         s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype) + 2) * sizeof(char*));
         strcpy(s_pathname, s_directory);
         strcat(s_pathname, "/.");
      }
      strcat(s_pathname, s_filename);
      strcat(s_pathname, s_filetype);
   }
#else
   s_pathname = malloc((strlen(s_directory) + strlen(s_filename) + strlen(s_filetype)) * sizeof(char*));
   strcpy(s_pathname, s_directory);
   strcat(s_pathname, s_filename);
   strcat(s_pathname, s_filetype);
#endif
   return s_pathname;
}
#endif

void v_save_state(oprocessor *h_processor) /* Restore saved processor state */
{
#if defined(CONTINIOUS)
   char *s_pathname = v_get_datafile_path(h_processor);
   v_write_state(h_processor, s_pathname); /* Save settings */
   free(s_pathname); /* Free up pathname */
#endif
}

void v_restore_state(oprocessor *h_processor) /* Restore saved processor state */
{
#if defined(CONTINIOUS)
   char *s_pathname = v_get_datafile_path(h_processor);
   v_read_state(h_processor, s_pathname); /* Load settings */
   free(s_pathname); /* Free up pathname */
#endif
}

void v_processor_reset(oprocessor *h_processor) /* Reset processor */
{
   int i_count;
   h_processor->first = 0; h_processor->last = REG_SIZE - 1;
   for (i_count = 0; i_count < REGISTERS - 2; i_count++) /*Clear the CPU registers and stack */
      v_reg_copy(h_processor, h_processor->reg[i_count], NULL); /* Copying nothing to a register clears it */
   for (i_count = 0; i_count < STACK_SIZE; i_count++) /* Clear the processor stack */
      h_processor->stack[i_count] = 0;
   for (i_count = 0; i_count < MEMORY_SIZE; i_count++) /*Clear memory */
      v_reg_copy(h_processor, h_processor->mem[i_count], NULL); /* Copying nothing to a register clears it */
   for (i_count = 0; i_count < STATUS_BITS; i_count++) /* Clear the processor status word */
      h_processor->status[i_count] = False;
   for (i_count = 0; i_count < FLAGS; i_count++) /* Clear the processor flags */
      h_processor->flags[i_count] = False;
   h_processor->opcode = 0;
   h_processor->pc = 0;
   h_processor->sp = 0;
   h_processor->f = 0;
   h_processor->p = 0;
   h_processor->addr = 0;
   h_processor->base = 10;
   h_processor->code = 0;
   h_processor->keypressed = False;
   h_processor->enabled = True;
   h_processor->sleep = False;
#if defined(WOODSTOCK) || defined(SPICE) || defined(HP10) || defined(HP67)
   h_processor->status[5] = True; /* TO DO - Check which flags should be set by default */
#endif
#if defined(HP67)
   for (i_count = 0; i_count < STATES; i_count++) /* Clear the processor flags */
      h_processor->crc[i_count] = False;
   h_processor->crc[READY] = -4;
#endif
#if defined(HP10)
   h_processor->position = BUFSIZE;
   for (i_count = 0; i_count < BUFSIZE; i_count++) /* Reset the character buffer contents */
      h_processor->buffer[i_count] = 0x3f;
#endif
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
   h_processor->q = 0;
   h_processor->ptr = False;
   h_processor->flags[CARRY] = True;
#else
   h_processor->rom_number = 0;
#endif
}

oprocessor *h_processor_create(int *h_rom) /* Create a new processor 'object' */
{
   oprocessor *h_processor;
   int i_count;
   if ((h_processor = malloc(sizeof(*h_processor)))==NULL) v_error(errno, h_err_memmory_alloc, __FILE__, __LINE__);
   for (i_count = 0; i_count < REGISTERS; i_count++)
      h_processor->reg[i_count] = h_register_create((i_count + 1) * -1); /* Allocate storage for the registers */
   for (i_count = 0; i_count < MEMORY_SIZE; i_count++)
      h_processor->mem[i_count] = h_register_create(i_count); /* Allocate storage for the RAM */
   h_processor->rom = h_rom ; /* Address of ROM */
   h_processor->mode = False;
   h_processor->timer = False;
   h_processor->trace = False;
   h_processor->step = False;
   v_processor_reset(h_processor);
#if defined(HP10)
   h_processor->print = MANUAL;
#endif
   return(h_processor);
}

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
static unsigned char *h_active_pointer (oprocessor *h_processor) /* Return address of active pointer */
{
   if (h_processor->ptr)
      return &h_processor->q;
   else
      return &h_processor->p;
}

static void v_op_inc_pt(oprocessor *h_processor) /* Increment active pointer */
{
   if (*h_active_pointer(h_processor) == REG_SIZE - 1)
      *h_active_pointer(h_processor) = 0;
   else
   {
      if (*h_active_pointer(h_processor) > 0)
         *h_active_pointer(h_processor) = *h_active_pointer(h_processor) + 1;
      else
      {
         if (h_processor->opcode != h_processor->rom[h_processor->pc - 1]) /* Literally the only way to work out if the pointer */
            *h_active_pointer(h_processor) = *h_active_pointer(h_processor) + 1; /* should be incremented when it is zero is to check the previous opcode ! */
      }
   }
}

static void v_op_dec_pt(oprocessor *h_processor) /* Decrement active pointer */
{
   if (*h_active_pointer(h_processor) == 0) *h_active_pointer(h_processor) = REG_SIZE - 1; else *h_active_pointer(h_processor) = *h_active_pointer(h_processor) - 1;
}

int i_translate_addr(int i_addr) /* Translate address to a memory register */
{
      /* HP10c ?
      /*    0x00 to 0x0A - Registers */
      /*    0xE0 to 0xFF - RAM (31 registers)*/
      /* HP15c
      /*    0x00 to 0x1A - Registers */
      /*    0xC0 to 0xFF - RAM */
   return i_addr;
}

#else
static void v_op_inc_p(oprocessor *h_processor) /* Increment p register */
{
#if defined(HP10) || defined(HP31e) || defined(HP32e) || defined(HP33e) || defined(HP33c) || defined(HP34c) || defined(HP37e) || defined(HP38e) || defined(HP38c) || defined(HP67)
   if (h_processor->p == REG_SIZE - 1)
      h_processor->p = 0;
   else
   {
      if (h_processor->p > 0)
         h_processor->p++;
      else
      {
         if (h_processor->opcode != h_processor->rom[h_processor->pc - 1]) /* Literally the only way to work out */
            h_processor->p++; /* if 'P' should be incremented when it is zero is to check the previous opcode !! */
      }
   }
#elif defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
   h_processor->p++;
   h_processor->p &= 15;
#else
   if (h_processor->p == REG_SIZE) h_processor->p = 0; else h_processor->p++;
#endif
}

static void v_op_dec_p(oprocessor *h_processor) /* Decrement p register */
{
#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
   h_processor->p--;
   h_processor->p &= 15;
#else
   if (h_processor->p == 0) h_processor->p = REG_SIZE - 1; else h_processor->p--;
#endif
}
#endif

static void v_op_inc_pc(oprocessor *h_processor) /* Increment program counter */
{
#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
   h_processor->pc = ((h_processor->pc >> 8) << 8) | ((h_processor->pc + 1) & 0xff); /* Address wraps round at end of ROM */
#else
   if (h_processor->pc >= (ROM_SIZE - 1))
      h_processor->pc = 0; /* Address wraps round at end of memory */
   else
      h_processor->pc = (h_processor->pc & 0xf000) | ((h_processor->pc + 1) & 0xfff); /* Address wraps round at end of bank */
#endif
   h_processor->flags[PREV_CARRY] = h_processor->flags[CARRY];
   h_processor->flags[CARRY] = False;
}

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)

void op_jsb(oprocessor *h_processor, int i_address) /* Call to subroutine */
{
   h_processor->stack[h_processor->sp] = h_processor->pc; /* Push current address on the stack */
   h_processor->sp = (h_processor->sp + 1) & (STACK_SIZE - 1); /* Update stack pointer */
   h_processor->pc = i_address; /* Long address */
}

#else

static void v_delayed_rom(oprocessor *h_processor) /* Delayed ROM select */
{
   if (h_processor->flags[DELAYED_ROM])
   {
      h_processor->pc = (h_processor->rom_number << 8 | (h_processor->pc & 0xf0ff));
      h_processor->flags[DELAYED_ROM] = False; /* Clear flag */
   }
   if (h_processor->pc < 0x1400) h_processor->pc &= 0xfff; /* The first ROM chip is mapped to all ROM banks, access implies a switch to bank 0 */
}

void op_jsb(oprocessor *h_processor, int i_address) /* Jump to subroutine */
{
   h_processor->stack[h_processor->sp] = h_processor->pc; /* Push current address on the stack */
   h_processor->sp = (h_processor->sp + 1) & (STACK_SIZE - 1); /* Update stack pointer */
   h_processor->pc = ((h_processor->pc & 0xff00) | i_address); /* Note - Uses an eight bit address */
   v_delayed_rom(h_processor);
}
#endif


void v_op_goto(oprocessor *h_processor) /* Conditional go to */
{
   if (h_processor->trace)
   {
      fprintf(stdout, "\n"); fprintf(stdout,h_msg_opcode, (h_processor->pc >> 12), (h_processor->pc & 0x0fff), h_processor->rom[h_processor->pc]);
      fprintf(stdout,"  then go to ");
   }
   h_processor->flags[PREV_CARRY] = h_processor->flags[CARRY];
   h_processor->flags[CARRY] = False;
#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
   if (h_processor->trace) fprintf(stdout, h_msg_address, (h_processor->pc & 0xf00) | (h_processor->rom[h_processor->pc]) >> 2); /* Mask off the bank number and least significant 8 bits*/
   if (h_processor->flags[PREV_CARRY])  /* Do if True */
      h_processor->pc = (h_processor->pc & 0xff00) | h_processor->rom[h_processor->pc] >> 2; /* Classic CPU uses a _eight_ bit address */
#else
   if (h_processor->trace) fprintf(stdout, h_msg_address, ((h_processor->pc & 0xc00) | h_processor->rom[h_processor->pc] )); /* Mask off the bank number and least significant 10 bits */
   if (h_processor->flags[PREV_CARRY])  /* Do if True */
      h_processor->pc = ((h_processor->pc & 0xfc00) | h_processor->rom[h_processor->pc]); /* Use a _ten_ bit address */
#endif
   else
      v_op_inc_pc(h_processor);
}

void v_processor_tick(oprocessor *h_processor) /* Decode and execute a single instruction */
{
#if defined(WOODSTOCK) || defined(SPICE) || defined(HP10) || defined(HP67)
   static const int i_set_p[16] = { 14,  4,  7,  8, 11,  2, 10, 12,  1,  3, 13,  6,  0,  9,  5, 14 };
   static const int i_tst_p[16] = { 4 ,  8, 12,  2,  9,  1,  6,  3,  1, 13,  5,  0, 11, 10,  7,  4 };
#endif

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
/* static const int n_map_i[16] = {  3,  4,  5, 10,  8,  6, 11, -1,  2,  9,  7, 13,  1, 12,  0, -1 }; /* map nnnn to index */
   static const int n_map_i[16] = {  3,  4,  5, 10,  8,  6, 11, 15,  2,  9,  7, 13,  1, 12,  0, 15 }; /* map nnnn to index */
#endif

   unsigned int i_last; /* Save the current PC */
   unsigned int i_opcode;
   unsigned int i_field; /* Field modifier */
   const char *s_field; /* Holds pointer to field name */

   if (h_processor->enabled && !h_processor->sleep)
   {

#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
      /* TIMER : status[11] = 1, status[3] = 0
       * PRGM  : status[11] = 0, status[3] = 1
       * RUN   : status[11] = 0, status[3] = 0 */
      if (h_processor->keypressed) h_processor->status[0] = True; /* Set status bit 0 if key pressed */
      if (h_processor->mode) h_processor->status[3] = True; /* Set status bits based on switch position */
      if (h_processor->timer) h_processor->status[11] = True;
#endif

#if defined(HP21) || defined(HP22) || defined(HP25) || defined(HP25c) || defined(HP27) || defined(HP29c)
      if (h_processor->keypressed) h_processor->status[15] = True; /* Set status bit if key pressed */
      if (h_processor->mode) h_processor->status[3] = True; /* Set status bits based on switch position */
      h_processor->status[5] = True; /* Low Power */
#endif

#if defined(HP67) /* Seems to use a flag rather then the status word for the switch position */
      if (h_processor->keypressed) h_processor->status[15] = True; /* Set status bit 15 if key pressed */
      h_processor->flags[MODE] = h_processor->mode; /* Set the program mode flag based on switch position */
#endif

#if defined(HP31e) || defined(HP32e) || defined(HP33e) || defined(HP33c) || defined(HP34c) || defined(HP37e) || defined(HP38e) || defined(HP38c) /* Setting S(5) breaks the self test */
      if (h_processor->keypressed) h_processor->status[15] = True; /* Set status bit if key pressed */
      if (h_processor->mode) h_processor->status[3] = True; /* Set status bits based on switch position */
      h_processor->status[5] = False; /* Self Test */
#endif

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
      if (h_processor->keypressed) h_processor->kyf = True; /* Set keyboard flag if key pressed */
#endif

      i_opcode = h_processor->rom[h_processor->pc]; /* Get next instruction */
      i_last = h_processor->pc;
      if (h_processor->trace)
         fprintf(stdout, h_msg_opcode, (i_last >> 12), (i_last & 0x0fff), h_processor->rom[i_last]);
      v_op_inc_pc(h_processor); /* Increment program counter _before_ decoding the opcode */
      switch (i_opcode & 03)
      {

#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
      case 00: /* Type 0 - Special operations */
         switch ((i_opcode >> 2) & 03)
         {
         case 00: /* Op-Codes matching x xxx xx 00 00 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00:  /* Op-Codes matching x xxx 000 000 */
               switch (i_opcode)
               {
               case 00000: /* nop */
                  if (h_processor->trace) fprintf(stdout, "nop");
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 01: /* Op-Codes matching x xxx 010 000 */
               switch ((i_opcode >> 6) & 01)
               {
               case 00: /* Op-Codes matching x xx0 010 000 */ /* select rom */
                  if (h_processor->trace) fprintf(stdout, "select rom %02o", i_opcode >> 7); /* Note - Not the same as the Woodstock CPU */
                  h_processor->pc = ((i_opcode >> 7) << 8) + ((h_processor->pc) & 0xff);
                  break;
               case 01: /* keys -> rom address */
                  if (h_processor->trace) fprintf(stdout, "keys -> rom address");
                  h_processor->pc &= 0xff00;
                  v_delayed_rom(h_processor);
                  h_processor->pc += h_processor->code;
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 02: /* Op-Codes matching x xxx 100 000 */
               switch (i_opcode)
               {
               case 0:
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 03: /* Op-Codes matching x xxx 110 000 */
               switch (i_opcode)
               {
               case 00060: /* return */
                  if (h_processor->trace) fprintf(stdout, "return");
                  h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
                  h_processor->pc = (h_processor->pc & (~0xff)) + (h_processor->stack[h_processor->sp] & 0xff); /* Pop program counter from the stack */
                  break;
               case 01160: /* c -> data address */
                  {
                     int i_addr;
                     if (h_processor->trace) fprintf(stdout, "c -> data address\t");
                     i_addr = h_processor->reg[C_REG]->nibble[12];
                     h_processor->addr = i_addr;
                     if (i_addr < MEMORY_SIZE)
                        h_processor->addr = i_addr;
                     else {
                        h_processor->addr = MEMORY_SIZE - 1;
                     }
                  }
                  if (h_processor->trace) fprintf(stdout, "addr = %d", h_processor->addr);
                  break;
               case 01360: /* c -> data */
                  if (h_processor->trace) fprintf(stdout, "c -> data\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  if (h_processor->addr < MEMORY_SIZE)
                     v_reg_copy(h_processor, h_processor->mem[h_processor->addr], h_processor->reg[C_REG]);
                  else
                  {
                     if (h_processor->trace) fprintf(stdout, "\n");
                     v_error(errno, h_err_invalid_register, h_processor->addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                  }
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->mem[h_processor->addr]);
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            }
            break;
         case 01: /* Op-Codes matching x xxx xx 01 00 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* 1 -> s(n) */
               if (h_processor->trace) fprintf(stdout, "1 -> s(%d)\t\t", i_opcode >> 6);
               h_processor->status[i_opcode >> 6] = True;
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
               break;
            case 01: /* if 0 = s(n) */
               if (h_processor->trace) fprintf(stdout, "if 0 = s(%d) ", i_opcode >> 6);
               h_processor->flags[CARRY] = !h_processor->status[i_opcode >> 6];
               v_op_goto(h_processor);
               break;
            case 02: /* 0 -> s(n) */
               if (h_processor->trace) fprintf(stdout, "0 -> s(%d)\t\t", i_opcode >> 6);
               h_processor->status[i_opcode >> 6] = False;
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
               break;
            case 03: /* delayed select rom n */
               switch (i_opcode)
               {
               case 00064: /* clear status */
                  if (h_processor->trace) fprintf(stdout, "clear status\t");
                  {
                     int i_count;
                     for (i_count = 0; i_count < sizeof(h_processor->status) / sizeof(*h_processor->status); i_count++)
                        h_processor->status[i_count] = False; /* Clear all bits */
                  }
                  if (h_processor->trace) v_fprint_status(stdout, h_processor);
                  break;
               case 01064: /*delayed select */
               case 01264: /*delayed select */
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                  break;
               default: /* delayed select rom n */
                  if (h_processor->trace) fprintf(stdout, "delayed select rom %d", i_opcode >> 7); /* Note - Not the same as the Woodstock CPU */
                  h_processor->rom_number = i_opcode >> 7;
                  h_processor->flags[DELAYED_ROM] = True;
               }
               break;
            }
            break;

         case 02: /* Op-Codes matching x xx xx 10 00 */
            switch ((i_opcode >> 4) & 03)
            {
            case 01: /* Op-Codes matching xx xx 01 10 00 */ /* load constant n */
               if (h_processor->trace) fprintf(stdout, "load constant %d\t", i_opcode >> 6);
               h_processor->reg[C_REG]->nibble[h_processor->p] = i_opcode >> 6;
               v_op_dec_p(h_processor);
               if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
               break;
            case 02: /* Op-Codes matching x xx 10 10 00 */
               switch (i_opcode)
               {
               case 00050: /* display toggle */
                  if (h_processor->trace) fprintf(stdout, "display toggle");
                  h_processor->flags[DISPLAY_ENABLE] = (!h_processor->flags[DISPLAY_ENABLE]);
                  break;
               case 00250: /* m exch c */
                  if (h_processor->trace) fprintf(stdout, "m exch c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_exch(h_processor, h_processor->reg[M_REG], h_processor->reg[C_REG]);
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                     v_fprint_register(stdout,h_processor->reg[M_REG]);
                  }
                  break;
               case 00450: /* c -> stack */
                  if (h_processor->trace) fprintf(stdout, "c -> stack\t\t");
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     v_reg_copy(h_processor, h_processor->reg[T_REG], h_processor->reg[Z_REG]); /* T = Z */
                     v_reg_copy(h_processor, h_processor->reg[Z_REG], h_processor->reg[Y_REG]); /* T = Z */
                     v_reg_copy(h_processor, h_processor->reg[Y_REG], h_processor->reg[C_REG]); /* T = Z */
                     if (h_processor->trace)
                     {
                        v_fprint_register(stdout,h_processor->reg[Y_REG]);
                        v_fprint_register(stdout,h_processor->reg[Z_REG]);
                        v_fprint_register(stdout,h_processor->reg[T_REG]);
                     }
                  break;
               case 00650: /* stack -> a */
                  if (h_processor->trace) fprintf(stdout, "stack -> a\t\t");
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[Y_REG]); /* T = Z */
                     v_reg_copy(h_processor, h_processor->reg[Y_REG], h_processor->reg[Z_REG]); /* T = Z */
                     v_reg_copy(h_processor, h_processor->reg[Z_REG], h_processor->reg[T_REG]); /* T = Z */
                     if (h_processor->trace)
                     {
                        v_fprint_register(stdout,h_processor->reg[A_REG]);
                        v_fprint_register(stdout,h_processor->reg[Y_REG]);
                        v_fprint_register(stdout,h_processor->reg[Z_REG]);
                     }
                  break;
               case 01050: /* display off */
                  if (h_processor->trace) fprintf(stdout, "display off");
                  h_processor->flags[DISPLAY_ENABLE] = False;
                  break;
               case 01250: /* m -> c */
                  if (h_processor->trace) fprintf(stdout, "m -> c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[M_REG]);
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
                  break;
               case 01450: /* down rotate */
                  if (h_processor->trace) fprintf(stdout, "down rotate\t\t");
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     v_reg_exch(h_processor, h_processor->reg[T_REG], h_processor->reg[C_REG]); /* T <> C - C to T */
                     v_reg_exch(h_processor, h_processor->reg[C_REG], h_processor->reg[Y_REG]); /* C <> Y - Y to C */
                     v_reg_exch(h_processor, h_processor->reg[Y_REG], h_processor->reg[Z_REG]); /* Y <> Z - Z to Y and T ends up in Z */
                     if (h_processor->trace)
                     {
                        v_fprint_register(stdout,h_processor->reg[C_REG]);
                        v_fprint_register(stdout,h_processor->reg[Y_REG]);
                        v_fprint_register(stdout,h_processor->reg[Z_REG]);
                        v_fprint_register(stdout,h_processor->reg[T_REG]);
                     }
                  break;
               case 01650: /* clear registers */
                  if (h_processor->trace) fprintf(stdout, "clear registers");
                  {
                     int i_count;
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     for (i_count = 0; i_count < REGISTERS - 2; i_count++) /* Don't clear M or N */
                        v_reg_copy(h_processor, h_processor->reg[i_count], NULL); /* Copying nothing to a register clears it */
                  }
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 03: /* Op-Codes matching x xx 10 10 00 */
               switch (i_opcode)
               {
               case 01360: /* c -> data */
                  if (h_processor->trace) fprintf(stdout, "c -> data\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  if (h_processor->addr < MEMORY_SIZE)
                     v_reg_copy(h_processor, h_processor->mem[h_processor->addr], h_processor->reg[C_REG]);
                  else
                  {
                     if (h_processor->trace) fprintf(stdout, "\n");
                     v_error(errno, h_err_invalid_register, h_processor->addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                  }
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->mem[h_processor->addr]);
                  break;
               case 01370: /* data -> c */
                  if (h_processor->trace) fprintf(stdout, "data -> c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->mem[h_processor->addr]);
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;

         case 03: /* Op-Codes matching xx xx xx 11 00 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* Op-Codes matching xx xx 00 11 00 */ /* n -> p */
               if (h_processor->trace) fprintf(stdout, "%d -> p", i_opcode >> 6);
               h_processor->p = i_opcode >> 6;
               break;
            case 01: /* Op-Codes matching xx xx 01 11 00 */ /* p - 1 -> p */
               if (h_processor->trace) fprintf(stdout, "p - 1 -> p");
               v_op_dec_p(h_processor);
               break;
            case 02: /* Op-Codes matching xx xx 10 11 00 */ /* if p != n */
               if (h_processor->trace) fprintf(stdout, "if p != %d", i_opcode >> 6);
               h_processor->flags[CARRY] = (h_processor->p != i_opcode >> 6);
               v_op_goto(h_processor);
               break;
            case 03: /* Op-Codes matching xx xx 11 11 00 */ /* p + 1 -> p */
               if (h_processor->trace) fprintf(stdout, "p + 1 -> p");
               v_op_inc_p(h_processor);
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
#endif

#if defined(WOODSTOCK) || defined(SPICE) || defined(HP10) || defined(HP67)
      case 00: /* Type 0 - Special operations */
         switch ((i_opcode >> 2) & 03)
         {
         case 00: /* Op-Codes matching x xxx xx0 000 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* Op-Codes matching x xxx 000 000 */
               switch (i_opcode)
               {
               case 00000: /* nop */
                  if (h_processor->trace) fprintf(stdout, "nop");
                  break;
#if defined(HP67)
               /*
                * 00100   Test ready
                * 00300   Test W/PGM switch
                * 00400   Set key pressed
                * 00500   Test if a key was pressed
                * 01000   Set default function keys
                * 01100   Test if default function keys set
                * 01200   Set merge flag
                * 01300   Test merge flag
                * 01400   Set waiting for card side 2 flag
                * 01500   Test waiting for card side 2 flag
                * 01700   Read/Write data to/from card via RAM $99 and $9B
                */
               case 00100: /* test motor on */
                  if (h_processor->trace) fprintf(stdout, "test motor on");
                  h_processor->status[3] = True; /* device always ready */
                  h_processor->crc[CARD] = False;
                  break;
               case 00300: /* test mode flag */
                  if (h_processor->trace) fprintf(stdout, "test mode flag (%d)", !h_processor->flags[MODE] );
                  h_processor->status[3] = !h_processor->flags[MODE]; /* Test the PRGM/RUN switch */
                  break;
               case 00400: /* set key pressed flag */
                  if (h_processor->trace) fprintf(stdout, "set key pressed flag");
                  h_processor->crc[ANYKEY] = True; /* Sets the any key pressed flag */
                  break;
               case 00500: /* test key pressed flag */ /* f -x- */
                  if (h_processor->trace) fprintf(stdout, "test key pressed flag");
                  h_processor->status[3] = h_processor->crc[ANYKEY];
                  if (h_processor->crc[ANYKEY]) h_processor->crc[ANYKEY] = False;
                  break;
               case 01000: /* set default function flag */
                  if (h_processor->trace) fprintf(stdout, "set flag 4");
                  h_processor->crc[FUNCTION] = True;
                  break;
               case 01100: /* test default function key flag */
                  if (h_processor->trace) fprintf(stdout, "test flag 4");
                  h_processor->status[3] = h_processor->crc[FUNCTION];
                  if (h_processor->crc[FUNCTION]) h_processor->crc[FUNCTION] = False;
                  break;
               case 01200: /* set merge flag */
                  if (h_processor->trace) fprintf(stdout, "set merge flag");
                  h_processor->crc[MERGE] = True;
                  break;
               case 01300: /* test merge flag */
                  if (h_processor->trace) fprintf(stdout, "clear flag 0");
                  h_processor->status[3] = h_processor->crc[MERGE];
                  if (h_processor->crc[MERGE]) h_processor->crc[MERGE] = False;
                  break;
               case 01400: /* set waiting flag */
                  if (h_processor->trace) fprintf(stdout, "clear waiting flag");
                  h_processor->crc[PAUSE] = True;
                  break;
               case 01500: /* test pause flag ? */
                  if (h_processor->trace) fprintf(stdout, "clear flag 1");
                  h_processor->status[3] = h_processor->crc[PAUSE];
                  if (h_processor->crc[PAUSE]) h_processor->crc[PAUSE] = False;
                  break;
               case 01700: /* read from or write to card */
                  if (h_processor->trace) fprintf(stdout, "card read write");
                  h_processor->status[3] = h_processor->crc[PAUSE];
                  if (h_processor->crc[PAUSE]) h_processor->crc[PAUSE] = False;
                  break;
#endif
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 01: /* Op-Codes matching x xxx 010 000 */
               switch (i_opcode)
               {
               case 00020: /* keys -> rom address (0 000 010 000) */
                  if (h_processor->trace) fprintf(stdout, "keys -> rom address");
                  h_processor->pc &= 0xff00;
                  v_delayed_rom(h_processor);
                  h_processor->pc += h_processor->code;
                  break;
               case 00120: /* keys -> a[2:1] (0 001 010 000) */
                  if (h_processor->trace) fprintf(stdout, "keys -> a\t\t");
                  /* The HP10 and HP19C use this to get the state of the printer mode switch */
#if defined(HP10)
                  /* HP10 - All = 1, Print = 2 (print with display off), Display = 4 */
                  /* HP19C/97 - Trace = 1, Normal = 2 (print with display off), Manual = 4 */
                  h_processor->reg[A_REG]->nibble[1] = h_processor->print;
#else
                  h_processor->reg[A_REG]->nibble[2] = (h_processor->code >> 4); /* Put keycode in A_REG */
                  h_processor->reg[A_REG]->nibble[1] = (h_processor->code & 0x0f);
#endif
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
                  break;
               case 00220: /* a -> rom address */
                  {
                     int i_addr;
                     if (h_processor->trace) fprintf(stdout, "a -> rom address");
                     h_processor->pc &= 0x0ff00;
                     i_addr = h_processor->pc + (h_processor->reg[A_REG]->nibble[2] << 4) + h_processor->reg[A_REG]->nibble[1];
                     if (i_addr < ROM_SIZE)
                         h_processor->pc = i_addr;
                     else
                     {
                        if (h_processor->trace) fprintf(stdout, "\n");
                        v_error(errno, h_err_invalid_address, i_addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                     }
                     v_delayed_rom(h_processor);
                  }
                  break;
               case 00320: /* reset twf */
                  if (h_processor->trace) fprintf(stdout, "reset twf");
                  break;
               case 00420: /* binary */
                  if (h_processor->trace) fprintf(stdout, "binary");
                  h_processor->base = 16;
                  break;
               case 00520: /* rotate left a */
                  if (h_processor->trace) fprintf(stdout, "rotate left a");
                  {
                     int c_temp, i_count;
                     c_temp = h_processor->reg[A_REG]->nibble[REG_SIZE - 1];
                     for (i_count = REG_SIZE - 1; i_count > 0; i_count--)
                        h_processor->reg[A_REG]->nibble[i_count] = h_processor->reg[A_REG]->nibble[i_count - 1];
                     h_processor->reg[A_REG]->nibble[0] = c_temp;
                     h_processor->flags[PREV_CARRY] = h_processor->flags[CARRY] = False;
                  }
                  break;
               case 00620: /* p - 1 -> p */
                  if (h_processor->trace) fprintf(stdout, "p - 1 -> p\t\t\t");
                  v_op_dec_p(h_processor);
                  if (h_processor->trace) fprintf(stdout, "p = %d", h_processor->p);
                  break;
               case 00720: /* p + 1 -> p */
                  if (h_processor->trace) fprintf(stdout, "p + 1 -> p\t\t\t");
                  v_op_inc_p(h_processor);
                  if (h_processor->trace) fprintf(stdout, "p = %d", h_processor->p);
                  break;
               case 01020: /* return */
                  if (h_processor->trace) fprintf(stdout, "return");
                  h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
                  h_processor->pc = h_processor->stack[h_processor->sp]; /* Pop program counter from the stack */
                  break;
#if defined(HP10)
               case 01120: /* pik1120 */
                  if (h_processor->trace) fprintf(stdout, "pik1120\t\t");
                  v_fprint_buffer (stdout, h_processor);
                  h_processor->status[3] = True; /* Set status bit 3 if printer ready (it always will be!) */
                  if (h_processor->trace) v_fprint_status(stdout, h_processor);
                  break;
               case 01220: /* pik1220 */
                  if (h_processor->trace) fprintf(stdout, "pik1220\t\t");
                  v_fprint_buffer (stdout, h_processor);
                  h_processor->status[3] = True; /* Clear status bit 3 if printer is out of paper */
                  if (h_processor->trace) v_fprint_status(stdout, h_processor);
                  break;
               case 01320: /* pik1320 */
                  if (h_processor->trace) fprintf(stdout, "pik1320\t\t");
                  v_fprint_buffer (stdout, h_processor);
                  if (h_processor->keypressed && h_processor->code) h_processor->status[3] = True; /* Set status bit 3 if key is pressed and a key code is pending */
                  if (h_processor->trace) v_fprint_status(stdout, h_processor);
                  break;
               case 01720: /* pik1720 print numeric (4 bit data)*/
                  if (h_processor->trace) fprintf(stdout, "pik1720\t\t");
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
                  if (h_processor->buffer[h_processor->position] == 0x3f) h_processor->position++;
                  {
                     int i_count;
                     for (i_count = 0; (i_count < REG_SIZE)  && (h_processor->reg[C_REG]->nibble[i_count] != 0xf); i_count++)
                     {
                        h_processor->position--;
                        h_processor->buffer[h_processor->position] = (h_processor->reg[C_REG]->nibble[i_count] << 2) | 0x3;
                     }
                  }
                  break;
#endif
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 02: /* Op-Codes matching x xxx 100 000 */ /* select rom */
               if (h_processor->trace) fprintf(stdout, "select rom %02o", i_opcode >> 6);
               h_processor->pc = ((i_opcode >> 6) << 8) + ((h_processor->pc) & 0xff);
               break;
            case 03: /* Op-Codes matching x xxx 110 000 */
               switch (i_opcode)
               {
#if defined(HP67)
               /*
                * 00060   Set display digits
                * 00160   Test display digits
                * 00260   Motor on
                * 00360   Motor off
                * 00560   Test if card inserted
                * 00660   Set card write mode
                * 00760   Set card read mode
                */
               case 00060: /* set display digits */
                  if (h_processor->trace) fprintf(stdout, "set display digits");
                  h_processor->crc[DISPLAY] = True;
                  break;
               case 00160: /* test display digits */
                  if (h_processor->trace) fprintf(stdout, "test display digits");
                  h_processor->status[3] = h_processor->crc[DISPLAY];
                  if (h_processor->crc[DISPLAY]) h_processor->crc[DISPLAY] = False;
                  break;
               case 00260: /* card reader motor on */
                  if (h_processor->trace) fprintf(stdout, "motor on");
                  break;
               case 00360: /* card reader motor off */
                  if (h_processor->trace) fprintf(stdout, "motor off");
                  break;
               case 00560: /* test card inserted */
                  if (h_processor->trace) fprintf(stdout, "test card inserted");
                  h_processor->status[3] = h_processor->crc[CARD]; /* Test if card is inserted */
                  break;
               case 00660: /* card reader set write mode */
                  if (h_processor->trace) fprintf(stdout, "set write mode");
                  break;
               case 00760: /* card reader set read mode */
                  if (h_processor->trace) fprintf(stdout, "set read mode");
                  break;
#endif
               case 01060: /* bank switch */
                  if (h_processor->trace) fprintf(stdout, "bank switch");
                  h_processor->flags[BANK_SWITCH] = (!h_processor->flags[BANK_SWITCH]); /* Toggle bank switch */
                  h_processor->pc ^=  0x01000; /* Toggle bank */
                  break;
               case 01160: /* c -> data address */
                  {
                     int i_addr;
                     if (h_processor->trace) fprintf(stdout, "c -> data address\t");
                     i_addr = (h_processor->reg[C_REG]->nibble[1] << 4) + h_processor->reg[C_REG]->nibble[0];
#if defined(HP10)
                     if ((i_addr < MEMORY_SIZE) || (i_addr == 0xFF)) /* Address 0xFF tells the PIK chip to put the key code on the data bus */
                        h_processor->addr = i_addr;
                     else
                     {
                        h_processor->addr = MEMORY_SIZE - 1;
                        if (h_processor->trace) fprintf(stdout, "\n");
                        v_error(errno, h_err_invalid_register, i_addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                     }
#else
                     if (i_addr < MEMORY_SIZE)
                        h_processor->addr = i_addr;
                     else
                     {
                        h_processor->addr = MEMORY_SIZE - 1;
                     }
#endif
                  }
                  if (h_processor->trace) fprintf(stdout, "\taddr = %d", h_processor->addr);
                  break;
               case 01260: /* clear data registers */
                  {
#if (defined(HP25) || defined(HP25c)) && defined(CONTINIOUS)
                     /* Ignore the instruction */
#else
                     int i_count;
                     if (h_processor->trace) fprintf(stdout, "clear data registers");
#if (defined(HP67)) && defined(CONTINIOUS)
                     if (h_processor->crc[READY])
                        h_processor->crc[READY]++;
                     else
#endif
                     {
                        h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                        for (i_count = h_processor->addr & ~0x0f; i_count < (h_processor->addr & ~0x0f) + 16; i_count++)
                        {
                           if (i_count < MEMORY_SIZE) /* Check memory size */
                              v_reg_copy(h_processor, h_processor->mem[i_count], NULL); /* Copying nothing to a register clears it */
                        }
                     }
#endif
                  }
                  break;
               case 01360: /* c -> data */
                  if (h_processor->trace) fprintf(stdout, "c -> data\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->mem[h_processor->addr], h_processor->reg[C_REG]);
                  if (h_processor->trace)
                     v_fprint_register(stdout, h_processor->mem[h_processor->addr]);
                  break;
#if defined(HP10)
               case 01660: /* pik1660 print alpha (6 bit data)*/
                  if (h_processor->trace) fprintf(stdout, "pik1660");
                  {
                     int i_counter = 0;
                     int i_count;
                     for (i_count = 0; (i_count < REG_SIZE - 1) && (h_processor->position >= 0); i_count++)
                     {
                        h_processor->position--;
                        if((i_counter & 1) == 0) /* Even numbered nibble */
                        {
                           h_processor->buffer[h_processor->position] = (h_processor->reg[C_REG]->nibble[i_count]) | ((h_processor->reg[C_REG]->nibble[i_count + 1]) & 0x3) << 4;
                        }
                        else
                        {
                           h_processor->buffer[h_processor->position] = ((h_processor->reg[C_REG]->nibble[i_count]) >> 2) | (h_processor->reg[C_REG]->nibble[i_count + 1]) << 2;
                           i_count++;
                        }
                        i_counter++;
                        if (h_processor->buffer[h_processor->position] == 0x3f) i_count = REG_SIZE;
                     }
                  }
                  break;
#endif
               case 01460: /* rom checksum */
                  if (h_processor->trace) fprintf(stdout, "rom checksum");
                  h_processor->status[5] = False;
                  h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
                  h_processor->pc = h_processor->stack[h_processor->sp]; /* Pop program counter on the stack */
                  break;
               case 01760: /* hi I'm woodstock */
                  if (h_processor->trace) fprintf(stdout, "hi I'm woodstock");
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
            }
            break;
         case 01: /* Op-Codes matching x xxx xx0 100 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* 1 -> s(n) */
               if (h_processor->trace) fprintf(stdout, "1 -> s(%d)\t\t", i_opcode >> 6);
               h_processor->status[i_opcode >> 6] = True;
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
               break;
            case 01: /* if 1 = s(n) */
               if (h_processor->trace) fprintf(stdout, "if 1 = s(%d)", i_opcode >> 6);
               h_processor->flags[CARRY] = h_processor->status[i_opcode >> 6];
               v_op_goto(h_processor);
               break;
            case 02: /* if p = n */
               if (h_processor->trace) fprintf(stdout, "if p = %d", i_tst_p[i_opcode >> 6]);
               h_processor->flags[CARRY] = (h_processor->p == i_tst_p[i_opcode >> 6]);
               v_op_goto(h_processor);
               break;
            case 03: /* delayed select rom n */
               if (h_processor->trace) fprintf(stdout, "delayed select rom %d", i_opcode >> 6);
               h_processor->rom_number = i_opcode >> 6;
               h_processor->flags[DELAYED_ROM] = True;
            }
            break;
         case 02: /* Op-Codes matching x xxx xx1 000 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* Op-Codes matching x xxx 001 000 */
               switch (i_opcode)
               {
               case 00010: /* clear registers */
                  if (h_processor->trace) fprintf(stdout, "clear registers");
                  {
                     int i_count;
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     for (i_count = 0; i_count < REGISTERS - 2; i_count++) /* Don't clear M or N */
                        v_reg_copy(h_processor, h_processor->reg[i_count], NULL); /* Copying nothing to a register clears it */
                  }
                  break;
               case 00110: /* clear status */
                  if (h_processor->trace) fprintf(stdout, "clear status\t");
                  {
                     int i_count;
                     for (i_count = 0; i_count < sizeof(h_processor->status) / sizeof(*h_processor->status); i_count++)
                        switch (i_count) {
                           case 1:  /* Scientific notation */
                           case 2:  /* Auto Enter (if set entering digit will push 'X') */
#if (!defined(HP10))
                           case 5:  /* Low power (Woodstock) / Self test (Spice) */
#endif
                           case 15: /* Set if any key is pressed */
                              break;
                           default:
                              h_processor->status[i_count] = False; /* Clear all bits except bits 1, 2, 5, 15 */
                        }
                  }
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
                  break;
               case 00210: /* display toggle */
                  if (h_processor->trace) fprintf(stdout, "display toggle");
                  h_processor->flags[DISPLAY_ENABLE] = (!h_processor->flags[DISPLAY_ENABLE]);
                  break;
               case 00310: /* display off */
                  if (h_processor->trace) fprintf(stdout, "display off");
                  h_processor->flags[DISPLAY_ENABLE] = False;
                  break;
               case 00410: /* m exch c */
                  if (h_processor->trace) fprintf(stdout, "m exch c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_exch(h_processor, h_processor->reg[M_REG], h_processor->reg[C_REG]);
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                     v_fprint_register(stdout,h_processor->reg[M_REG]);
                  }
                  break;
               case 00510: /* m -> c */
                  if (h_processor->trace) fprintf(stdout, "m -> c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[M_REG]);
                  if (h_processor->trace)
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                  break;
               case 00610: /* n exch c */
                  if (h_processor->trace) fprintf(stdout, "n exch c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_exch(h_processor, h_processor->reg[N_REG], h_processor->reg[C_REG]);
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                     v_fprint_register(stdout,h_processor->reg[N_REG]);
                  }
                  break;
               case 00710: /* n -> c */
                  if (h_processor->trace) fprintf(stdout, "n -> c\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[N_REG]);
                  if (h_processor->trace)
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                  break;
               case 01010: /* stack -> a */
                  if (h_processor->trace) fprintf(stdout, "stack -> a");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[Y_REG]); /* A = Y */
                  v_reg_copy(h_processor, h_processor->reg[Y_REG], h_processor->reg[Z_REG]); /* Y = Z */
                  v_reg_copy(h_processor, h_processor->reg[Z_REG], h_processor->reg[T_REG]); /* Z = T */
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[A_REG]);
                     v_fprint_register(stdout,h_processor->reg[Y_REG]);
                     v_fprint_register(stdout,h_processor->reg[Z_REG]);
                  }
                  break;
               case 01110: /* down rotate */
                  if (h_processor->trace) fprintf(stdout, "down rotate\t");
                     h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                     v_reg_exch(h_processor, h_processor->reg[T_REG], h_processor->reg[C_REG]); /* T <> C - C to T */
                     v_reg_exch(h_processor, h_processor->reg[C_REG], h_processor->reg[Y_REG]); /* C <> Y - Y to C */
                     v_reg_exch(h_processor, h_processor->reg[Y_REG], h_processor->reg[Z_REG]); /* Y <> Z - Z to Y and T ends up in Z */
                     if (h_processor->trace)
                     {
                        v_fprint_register(stdout,h_processor->reg[C_REG]);
                        v_fprint_register(stdout,h_processor->reg[Y_REG]);
                        v_fprint_register(stdout,h_processor->reg[Z_REG]);
                        v_fprint_register(stdout,h_processor->reg[T_REG]);
                     }
                  break;
               case 01210: /* y -> a */
                  if (h_processor->trace) fprintf(stdout, "y -> a\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[Y_REG]);
                  if (h_processor->trace)
                     v_fprint_register(stdout,h_processor->reg[A_REG]);
                  break;
               case 01310: /* c _> stack */
                  if (h_processor->trace) fprintf(stdout, "c -> stack\t\t");
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->reg[T_REG], h_processor->reg[Z_REG]); /* T = Z */
                  v_reg_copy(h_processor, h_processor->reg[Z_REG], h_processor->reg[Y_REG]); /* T = Z */
                  v_reg_copy(h_processor, h_processor->reg[Y_REG], h_processor->reg[C_REG]); /* T = Z */
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[C_REG]);
                     v_fprint_register(stdout,h_processor->reg[Y_REG]);
                     v_fprint_register(stdout,h_processor->reg[Z_REG]);
                     v_fprint_register(stdout,h_processor->reg[T_REG]);
                  }
                  break;
               case 01410: /* decimal */
                  if (h_processor->trace) fprintf(stdout, "decimal");
                  h_processor->base = 10;
                  break;
               case 01610: /* f -> a */
                  h_processor->reg[A_REG]->nibble[0] = h_processor->f;
                  if (h_processor->trace) fprintf(stdout, "f -> a\t\t");
                  if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
                  break;
               case 01710: /* f exch a */
                  if (h_processor->trace) fprintf(stdout, "f exch a\t\t");
                  {
                     int i_temp;
                     i_temp = h_processor->reg[A_REG]->nibble[0];
                     h_processor->reg[A_REG]->nibble[0] = h_processor->f;
                     h_processor->f = i_temp;
                  }
                  if (h_processor->trace)
                  {
                     v_fprint_register(stdout,h_processor->reg[A_REG]);
                     fprintf(stdout, "\tf = %02x", h_processor->f);
                  }
                  break;
               default:
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 01: /* load constant n */
               if (h_processor->trace) fprintf(stdout, "load constant %d\t", i_opcode >> 6);
               h_processor->reg[C_REG]->nibble[h_processor->p] = i_opcode >> 6;
               v_op_dec_p(h_processor);
               if (h_processor->trace)
               {
                  v_fprint_register(stdout,h_processor->reg[C_REG]);
                  fprintf(stdout, "\tp = %d", h_processor->p);
               }
               break;
            case 02: /* c -> data register(n) */
               h_processor->addr &= 0xfff0;
               h_processor->addr += (i_opcode >> 6);
               if (h_processor->trace) fprintf(stdout, "c -> data register(%d)", h_processor->addr);
               if ((h_processor->addr) < MEMORY_SIZE)
               {
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->mem[h_processor->addr], h_processor->reg[C_REG]); /* C -> reg(n) */
               }
               else
               {
                  if (h_processor->trace) fprintf(stdout, "\n");
                  v_error(errno, h_err_invalid_address, i_opcode >> 6, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               }
               break;
            case 03: /* data -> c or data register(n)-> c */
               h_processor->first = 0; h_processor->last = REG_SIZE - 1;
               if ((i_opcode >> 6) == 0)
               {
                  if (h_processor->trace) fprintf(stdout, "data -> c\t\t");
                  v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->mem[h_processor->addr]);
               }
#if defined(HP10)
               else if (((i_opcode >> 6) == 0xf) && (h_processor->addr == 0xff))
               {
                  if (h_processor->trace) fprintf(stdout, "data -> c\t\t");
                  h_processor->reg[C_REG]->nibble[2] = (h_processor->code >> 4);
                  h_processor->reg[C_REG]->nibble[1] = (h_processor->code & 0x0f);
                  h_processor->reg[C_REG]->nibble[0] = 0;
                  h_processor->code = 0; /* Clear the key code (so it isn't read twice if the key is held down) */
               }
#else
               else
               {
                  h_processor->addr &= 0xfff0;
                  h_processor->addr += (i_opcode >> 6);
                  if (h_processor->trace) fprintf(stdout, "data register(%d) -> c", h_processor->addr);
                  if ((h_processor->addr) < MEMORY_SIZE)
                     v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->mem[h_processor->addr]);
                  else
                  {
                     if (h_processor->trace) fprintf(stdout, "\n");
                     v_error(errno, h_err_invalid_address, h_processor->addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                  }
               }
#endif
               if (h_processor->trace)
                  v_fprint_register(stdout,h_processor->reg[C_REG]);
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;

         case 03: /* Op-Codes matching x xxx xx1 100 */
            switch ((i_opcode >> 4) & 03)
            {
            case 00: /* 0 -> s(n) */
               if (h_processor->trace) fprintf(stdout, "0 -> s(%d)\t\t", i_opcode >> 6);
               h_processor->status[i_opcode >> 6] = False;
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
               break;
            case 01: /* if 0 = s(n) */
               if (h_processor->trace) fprintf(stdout, "if 0 = s(%d) ", i_opcode >> 6);
               h_processor->flags[CARRY] = !h_processor->status[i_opcode >> 6];
               v_op_goto(h_processor);
               break;
            case 02: /* if p != n */
               /* 01354 if p !=  0  00554 if p !=  1  00354 if p !=  2
                * 00754 if p !=  3  00054 if p !=  4  01254 if p !=  5
                * 00654 if p !=  6  01654 if p !=  7  00154 if p !=  8
                * 00454 if p !=  9  01554 if p != 10  01454 if p != 11
                * 00254 if p != 12  01154 if p != 13  N/A   if p != 14
                * N/A   if p != 15
               */
               if (h_processor->trace) fprintf(stdout, "if p != %d", i_tst_p[i_opcode >> 6]);
               h_processor->flags[CARRY] = (h_processor->p != i_tst_p[i_opcode >> 6]);
               v_op_goto(h_processor);
               break;
            case 03: /* n -> p*/
               /* 01474  0 -> p  01074  1 -> p  00574  2 -> p
                * 01174  3 -> p  00174  4 -> p  01674  5 -> p
                * 01374  6 -> p  00274  7 -> p  00374  8 -> p
                * 01574  9 -> p  00674 10 -> p  00474 11 -> p
                * 00774 12 -> p  01274 13 -> p  N/A   14 -> p
                * N/A   15 -> p
                * */
               if (h_processor->trace) fprintf(stdout, "%d -> p\t\t\t", i_set_p[i_opcode >> 6]);
               h_processor->p = i_set_p[i_opcode >> 6];
               if (h_processor->trace) fprintf(stdout, "p = %d", h_processor->p);
               break;
            }
            break;
         }
         break;
#endif

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
      case 0x00: /* Type 0 - Special operations */
         switch ((i_opcode >> 2) & 0xf)
         {
         case 0x00:
            switch (i_opcode >> 6)
            {
            case 0x00: /* No operation (00 0000 0000) */
               if (h_processor->trace) fprintf(stdout, "nop");
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;

         case 0x01: /* 0 -> status[7:0] - Clear status (11 1100 0100) or 0 -> status[d] - Clear status bit (dd dd00 0100) */
            if ((i_opcode >> 6) == 7) /* Reserved opcode (01 1100 0100) */
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            if ((i_opcode >> 6) < 15)
            {
               if (h_processor->trace) fprintf(stdout, "st = 0 %-2d\t\t", n_map_i[i_opcode >> 6]);
               h_processor->status[n_map_i[i_opcode >> 6]] = (unsigned char)False;
            }
            else
            {
               int i_count;
               if (h_processor->trace) fprintf(stdout, "clrst\t\t");
               for (i_count = 7; i_count >= 0; i_count--) /* Note only clears bits 7-0 of the status register */
                  h_processor->status[n_map_i[i_opcode >> 6]] = (unsigned char)False;
            }
            if (h_processor->trace) v_fprint_status(stdout, h_processor);
            break;
         case 0x02: /* 0 -> kyf  - Reset keyboard (11 1100 1000) or 1 -> status[d] - Set status bit (dd dd00 0100) */
            if ((i_opcode >> 6) == 7) /* Reserved opcode (01 1100 1000) */
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            if ((i_opcode >> 6) < 15)
            {
               if (h_processor->trace) fprintf(stdout, "st = 1 %-2d\t\t", n_map_i[i_opcode >> 6]);
               h_processor->status[n_map_i[i_opcode >> 6]] = (unsigned char)True;
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
            }
            else
            {
               if (h_processor->trace) fprintf(stdout, "rstkb");
               if (!h_processor->keypressed) h_processor->flags[CARRY] = h_processor->kyf = False; /* Clear keyboard flag if no key pressed */
            }
            break;
         case 0x03: /* Test status bit [dddd] if 0 <= dddd <= 14 (dd dd00 1100) or test keyboard (11 1100 1100) */
            if ((i_opcode >> 6) == 7) /* Reserved opcode (01 1100 1100) */
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            if ((i_opcode >> 6) < 15)
            {
               if (h_processor->trace) fprintf(stdout, "? st = 1 %-2d", n_map_i[i_opcode >> 6]);
               h_processor->flags[CARRY] = h_processor->status[n_map_i[i_opcode >> 6]];
            }
            else
            {
               if (h_processor->trace) fprintf(stdout, "chkkb ");
               h_processor->flags[CARRY] = h_processor->kyf;
            }
            break;
         case 0x04: /* nn -> c[pt] - Load constant n (nn nn01 0000) */
            if (h_processor->trace) fprintf(stdout, "lc %1x\t\t", i_opcode >> 6);
            h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor)] = i_opcode >> 6;
            v_op_dec_pt(h_processor);
            if (h_processor->trace)
               v_fprint_register(stdout, h_processor->reg[C_REG]);
            break;
         case 0x05: /* Test pointer equal to dddd if 0 <= dddd <= 14 (dd dd00 1100) or decrement pointer (11 1101 0100) */
            if ((i_opcode >> 6) == 7) /* Reserved opcode (01 1101 0100) */
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            if ((i_opcode >> 6) < 15)
            {
               if (h_processor->trace) fprintf(stdout, "? pt = %d", n_map_i[i_opcode >> 6]);
               h_processor->flags[CARRY] = (*h_active_pointer(h_processor) == n_map_i[i_opcode >> 6]);
            }
            else
            {
               if (h_processor->trace) fprintf(stdout, "dec pt");
               v_op_dec_pt(h_processor);
            }
            break;

         case 0x06:
            switch (i_opcode >> 6)
            {
            case 0x0: /* Reserved opcode (00 0001 1000) */
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
               break;
            case 0x01: /* c[pt + 1:pt] -> g - Load g from c (00 0101 1000) */
               if (h_processor->trace) fprintf(stdout, "g = c\t\t");
               h_processor->g[0]  = h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor)] & 0x0f; /* c[pt] -> g[0] */
               if (*h_active_pointer(h_processor) < (REG_SIZE - 1))  /* Check that pt + 1 is valid */
                  h_processor->g[1] = h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor) + 1] & 0x0f; /* c[pt + 1] -> g[1] */
               if (h_processor->trace)
                  fprintf(stdout, "g = 0x%02x", h_processor->g[0] | (h_processor->g[1] << 4));
               break;
            case 0x02: /* g -> c[pt + 1:pt] - Load c from g (00 0101 1000) */
               if (h_processor->trace) fprintf(stdout, "c = g\t\t");
               h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor)] = h_processor->g[0]; /* g[0] -> c[pt] */
               if (*h_active_pointer(h_processor) < (REG_SIZE - 1))  /* Check that pt + 1 is valid */
                  h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor) + 1] = h_processor->g[1]; /* g[1] -> c[pt + 1] */
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x03: /* g -> c[pt + 1:pt], c[pt + 1:pt] -> g - Exchange c and g (00 1101 1000 */
               if (h_processor->trace) fprintf(stdout, "cgex\t\t");
               {
                  int i_temp[2];
                  i_temp[0] = h_processor->g[0]; i_temp[1] = h_processor->g[1];
                  h_processor->g[0] = h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor)] & 0x0f; /* c[pt] -> g[0] */
                  h_processor->g[1] = 0; /* (0 -> g[1]) */
                  h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor)] = i_temp[0]; /* g[0] -> c[pt] */
                  if (*h_active_pointer(h_processor) < (REG_SIZE - 1))  /* Check that pt + 1 is valid */
                  {
                     h_processor->g[1] = h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor) + 1] & 0x0f; /* c[pt + 1] -> g[1] */
                     h_processor->reg[C_REG]->nibble[*h_active_pointer(h_processor) + 1] = i_temp[1]; /* g[1] -> c[pt + 1] */
                  }
               }
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               if (h_processor->trace) fprintf(stdout, "g = 0x%02x", h_processor->g[0] | (h_processor->g[1] << 4));
               break;
            case 0x05: /* c -> m - Copy C register to M register (01 0101 1000) */
               if (h_processor->trace) fprintf(stdout, "m = c\t\t");
               h_processor->first = 0; h_processor->last = REG_SIZE - 1;
               v_reg_copy(h_processor, h_processor->reg[M_REG], h_processor->reg[C_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[M_REG]);
               break;
            case 0x06: /* m -> c - Copy M register to C register (01 1001 1000) */
               if (h_processor->trace) fprintf(stdout, "c = m\t\t");
               h_processor->first = 0; h_processor->last = REG_SIZE - 1;
               v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[M_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x07: /* c -> m, m -> c - Exchange C register and M register (01 1101 1000) */
               if (h_processor->trace) fprintf(stdout, "cmex\t\t");
               h_processor->first = 0; h_processor->last = REG_SIZE - 1;
               v_reg_exch(h_processor, h_processor->reg[M_REG], h_processor->reg[C_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[M_REG]);
               break;
            case 0x0e: /* st[0:7] -> c[0:1] - Load c register from the status byte (11 1001 1000) */
               if (h_processor->trace) fprintf(stdout, "c = st\t\t");
               {
                  int i_status, i_count;
                  i_status = 0;
                  for (i_count = 7; i_count >= 0 ; i_count--)
                  {
                     i_status <<= 1;
                     if (h_processor->status[i_count]) i_status |= 0x1;
                  }
                  h_processor->reg[C_REG]->nibble[0] = i_status & 0xf;
                  h_processor->reg[C_REG]->nibble[1] = i_status >> 4;
               }
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x0f: /* c[0:1] -> st[0:7], st[0:7] -> c[0:1] - Exchange c register and the status byte (11 1101 1000) */
               if (h_processor->trace) fprintf(stdout, "cstex\t\t");
               {
                  int i_temp, i_status, i_count;
                  i_status = 0; /* Save st[0:7] */
                  for (i_count = 7; i_count >= 0 ; i_count--)
                  {
                     i_status <<= 1;
                     if (h_processor->status[i_count]) i_status |= 0x1;
                  }
                  i_temp = h_processor->reg[C_REG]->nibble[0] | h_processor->reg[C_REG]->nibble[1] << 4;
                  for (i_count = 0; i_count <=7; i_count++)
                  {
                     if (i_temp & 0x1)
                        h_processor->status[i_count] = True;
                     else
                        h_processor->status[i_count] = False;
                     i_temp >>= 1;
                  }
                  h_processor->reg[C_REG]->nibble[0] = i_status & 0xf;
                  h_processor->reg[C_REG]->nibble[1] = i_status >> 4;
               }
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               if (h_processor->trace) v_fprint_status(stdout, h_processor);
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               /** debug(fprintf(stderr,"%02x\n", (i_opcode >> 6) & 0xf)); */
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 0x07: /* Load pointer with dddd if 0 <= dddd <= 14 (dd dd00 1100) or decrement pointer (11 1101 0100) */
            if ((i_opcode >> 6) == 7) /* Reserved opcode (01 1101 0100)*/
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            if ((i_opcode >> 6) < 15)
            {
               if (h_processor->trace) fprintf(stdout, "pt = %d", n_map_i[i_opcode >> 6]);
               *h_active_pointer(h_processor) = n_map_i[i_opcode >> 6];
               if (h_processor->trace) fprintf(stdout, "\t\tpt = %02d  ", *h_active_pointer(h_processor));
            }
            else
            {
               if (h_processor->trace) fprintf(stdout, "inc pt");
               v_op_inc_pt(h_processor);
            }
            break;

         case 0x08: /* xxxx 10 0000 */
            switch ((i_opcode >> 6) & 0xf)
            {
            case 0x01: /* Power off (00 0110 0000) */
               if (h_processor->trace) fprintf(stdout, "powoff");
               if (h_processor->flags[DISPLAY_ENABLE]) /* Sleep */
               {
                  h_processor->sleep = True;
               }
               else  /* Poweroff */
               {
                  h_processor->flags[CARRY] = True; /* Carry set on deep sleep */
                  h_processor->enabled = False;
               }
               h_processor->pc = 0;
               break;
            case 0x02: /* pt = p - Select pointer p (00 1010 0000) */
               if (h_processor->trace) fprintf(stdout, "sel p\t\t");
               h_processor->ptr = False;
               break;
            case 0x03: /* pt = q - Select pointer q (00 1110 0000) */
               if (h_processor->trace) fprintf(stdout, "sel q\t\t");
               h_processor->ptr = True;
               break;
            case 0x04: /* ? p == q - Test p equal to q  (01 0010 0000) */
               if (h_processor->trace) fprintf(stdout, "? p = q\t\t");
               if (h_processor->p == h_processor->q) h_processor->flags[CARRY] = True;
               break;
            case 0x05: /* ? low level detect - Test battery state (01 0110 0000) */
               if (h_processor->trace) fprintf(stdout, "? lld\t\t");
               /* Do nothing - battery can never go flat */
               break;
            case 0x06: /* 0 -> a, 0 -> b, 0 -> c - Clear registers a, b and c (01 1010 0000) */
               if (h_processor->trace) fprintf(stdout, "clrabc\t\t");
               h_processor->first = 0; h_processor->last = REG_SIZE - 1;
               v_reg_copy(h_processor, h_processor->reg[A_REG], NULL);
               v_reg_copy(h_processor, h_processor->reg[B_REG], NULL);
               v_reg_copy(h_processor, h_processor->reg[C_REG], NULL);
               if (h_processor->trace)
               {
                  v_fprint_register(stdout, h_processor->reg[A_REG]);
                  v_fprint_register(stdout, h_processor->reg[B_REG]);
                  v_fprint_register(stdout, h_processor->reg[C_REG]);
               }
               break;
            case 0x07: /* c[6:3] -> pc - Branch using C register (01 1110 0000) */
               if (h_processor->trace) fprintf(stdout, "goto c\t\t");
               h_processor->pc = (((h_processor->reg[C_REG]->nibble[6])<< 12) |
                  (h_processor->reg[C_REG]->nibble[5] << 8) |
                  (h_processor->reg[C_REG]->nibble[4] << 4) |
                  (h_processor->reg[C_REG]->nibble[3]));
               break;
            case 0x08: /* keys -> c[4:3] - Load c from keys (10 0010 0000) */
               if (h_processor->trace) fprintf(stdout, "c = keys\t\t");
               h_processor->reg[C_REG]->nibble[3] = h_processor->code & 0xf;
               h_processor->reg[C_REG]->nibble[4] = (h_processor->code >> 4) & 0xf;
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x09: /* 0 -> decimal - Set hex mode(10 0110 0000) */
               if (h_processor->trace) fprintf(stdout, "sethex");
               h_processor->base = 16;
               break;
            case 0x0a: /* 1 -> decimal - Set decimal mode (10 0110 0000) */
               if (h_processor->trace) fprintf(stdout, "setdec");
               h_processor->base = 10;
               break;
            case 0x0b: /* 0 -> display - Display off (10 1110 0000) */
               if (h_processor->trace) fprintf(stdout, "disoff");
               h_processor->flags[DISPLAY_ENABLE] = False;
               break;
            case 0x0c: /* ~display -> display - Display toggle (10 1110 0000) */
               if (h_processor->trace) fprintf(stdout, "distog");
               h_processor->flags[DISPLAY_ENABLE] = !h_processor->flags[DISPLAY_ENABLE];
               break;
            case 0x0d: /* if carry stack[0] -> pc, stack[1] -> stack[0], stack[2] -> stack[1], stack[3] -> stack[2], 0 -> stack[3] - Return if carry (11 0110 0000) */
               if (h_processor->trace) fprintf(stdout, "rtn c");
               if (h_processor->flags[PREV_CARRY])
               {
                  h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
                  h_processor->pc = h_processor->stack[h_processor->sp]; /* Pop program counter from the stack */
               }
               break;
            case 0x0e: /* if !carry stack[0] -> pc, stack[1] -> stack[0], stack[2] -> stack[1], stack[3] -> stack[2], 0 -> stack[3] - Return if no carry (11 1010 0000) */
               if (h_processor->trace) fprintf(stdout, "rtn nc");
               if (!h_processor->flags[PREV_CARRY])
               {
                  h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
                  h_processor->pc = h_processor->stack[h_processor->sp]; /* Pop program counter from the stack */
               }
               break;
            case 0x0f: /* stack[0] -> pc, stack[1] -> stack[0], stack[2] -> stack[1], stack[3] -> stack[2], 0 -> stack[3] - Return (11 1110 0000) */
               if (h_processor->trace) fprintf(stdout, "rtn");
               h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
               h_processor->pc = h_processor->stack[h_processor->sp]; /* Pop program counter from the stack */
               break;
            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               /** debug(fprintf(stderr,"%02x\n", (i_opcode >> 6) & 0xf)); */
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 0x0a: /* {addr[11:4], nnnn} -> addr, c -> reg[addr] - Load register from c (nn nn10 1000) */
            if (h_processor->trace) fprintf(stdout, "regn = c %-2d", (i_opcode >> 6));
            h_processor->addr = (h_processor->addr & 0xff0) | (i_opcode >> 6);
            h_processor->first = 0;
            h_processor->last = REG_SIZE - 1;
            if (i_translate_addr(h_processor->addr) < MEMORY_SIZE)
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
               if ((h_processor->addr != 0x08) && (h_processor->addr != 0x18))  /* Non existent registers */
#endif
               {
                  v_reg_copy(h_processor, h_processor->mem[i_translate_addr(h_processor->addr)], h_processor->reg[C_REG]);
                  if (h_processor->trace) v_fprint_register(stdout, h_processor->mem[i_translate_addr(h_processor->addr)]);
               }
            break;
         case 0x0c:
            switch ((i_opcode >> 6) & 0xf)
            {
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
            case 0x00: /* display blink- Display blink (00 0011 0000) */
               if (h_processor->trace) fprintf(stdout, "blink\t\t");
               h_processor->flags[DISPLAY_ENABLE] = True;
               break;
#endif
            case 0x01: /* c -> n - Load n from c (00 0111 0000) */
               if (h_processor->trace) fprintf(stdout, "n = c\t\t");
               h_processor->first = 0;
               h_processor->last = REG_SIZE - 1;
               v_reg_copy(h_processor, h_processor->reg[N_REG], h_processor->reg[C_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[N_REG]);
               break;
            case 0x02: /* n -> c - Load C from N (00 1011 0000) */
               if (h_processor->trace) fprintf(stdout, "c = n\t\t");
               h_processor->first = 0;
               h_processor->last = REG_SIZE - 1;
               v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[N_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x03: /* c -> n, n-> c - Exchange c and n (00 1111 0000) */
               h_processor->first = 0;
               h_processor->last = REG_SIZE - 1;
               if (h_processor->trace) fprintf(stdout, "cnex\t\t");
               v_reg_exch(h_processor, h_processor->reg[N_REG], h_processor->reg[C_REG]);
               if (h_processor->trace)
               {
                  v_fprint_register(stdout, h_processor->reg[C_REG]);
                  v_fprint_register(stdout, h_processor->reg[N_REG]);
               }
            break;
            case 0x04: /* rom[pc++] -> c[2:0] - Load immediate (01 0011 0000) */
               {
                  int i_next;
                  i_next = h_processor->rom[h_processor->pc]; /* Get next opcode */
                  h_processor->reg[C_REG]->nibble[2] = (i_next >> 8) & 0xf;
                  h_processor->reg[C_REG]->nibble[1] = (i_next >> 4) & 0xf;
                  h_processor->reg[C_REG]->nibble[0] = i_next & 0xf;
                  if (h_processor->trace)
                  {
                     fprintf(stdout, "ldi\n");
                     fprintf(stdout, h_msg_opcode, (h_processor->pc >> 12), (h_processor->pc & 0x0fff), h_processor->rom[h_processor->pc]);
                     fprintf(stdout,"  ");
                     fprintf(stdout, h_msg_address, i_next);
                     fprintf(stdout, "\t\t");
                     v_fprint_register(stdout, h_processor->reg[C_REG]);
                  }
                  v_op_inc_pc(h_processor); /* Increment program counter */
               }
               break;
            case 0x05: /* stack[2] -> stack[3], stack[1] -> stack[2], stack[0] -> stack[1], c -> stack[0] - Push c[6:3] onto the stack (01 0111 0000) */
               if (h_processor->trace) fprintf(stdout, "stk = c\t\t");
               h_processor->stack[h_processor->sp] = (h_processor->reg[C_REG]->nibble[6] << 12) | (h_processor->reg[C_REG]->nibble[5] << 8) | (h_processor->reg[C_REG]->nibble[4] << 4) | (h_processor->reg[C_REG]->nibble[3]);
               h_processor->sp = (h_processor->sp + 1) & (STACK_SIZE - 1); /* Update stack pointer */
               break;
            case 0x06: /* stack[2] -> stack[3], stack[1] -> stack[2], stack[0] -> stack[1], c -> stack[0] - Pop c[6:3] from the stack (01 0111 0000) */
               if (h_processor->trace) fprintf(stdout, "c = stk\t\t");
               h_processor->sp = (h_processor->sp - 1) & (STACK_SIZE - 1); /* Update stack pointer */
               h_processor->reg[C_REG]->nibble[3] = h_processor->stack[h_processor->sp] & 0xf;
               h_processor->reg[C_REG]->nibble[4] = (h_processor->stack[h_processor->sp] >> 4) & 0xf;
               h_processor->reg[C_REG]->nibble[5] = (h_processor->stack[h_processor->sp] >> 8) & 0xf;
               h_processor->reg[C_REG]->nibble[6] = (h_processor->stack[h_processor->sp] >> 12) & 0xf;
               break;
            case 0x09: /* {addr[11:4], nnnn} -> addr, c -> reg[addr] - Load register address from c (10 0111 1000) */
                  if (h_processor->trace) fprintf(stdout, "dadd = c\t\t");
                  h_processor->addr = ((h_processor->reg[C_REG]->nibble[2] << 8) |
                     (h_processor->reg[C_REG]->nibble[1] << 4) |
                     (h_processor->reg[C_REG]->nibble[0])) & 0x3ff; /* Load 12 bit address into address register from c */
                  if (h_processor->trace) fprintf(stdout, "addr = %d", h_processor->addr);
               break;
            case 0x0b: /* data = c - Load register from c (10 1111 0000) */
               if (h_processor->trace) fprintf(stdout, "data = c\t\t");
               if ((i_translate_addr(h_processor->addr) < MEMORY_SIZE) && (h_processor->addr != 0x08) && (h_processor->addr != 0x18))
               {
                  h_processor->first = 0; h_processor->last = REG_SIZE - 1;
                  v_reg_copy(h_processor, h_processor->mem[i_translate_addr(h_processor->addr)], h_processor->reg[C_REG]);
                  if (h_processor->trace) v_fprint_register(stdout, h_processor->mem[i_translate_addr(h_processor->addr)]);
               }
               break;
            case 0x0c: /* c[6:3] -> addr, ram[addr] -> c[2:0] - Exchange c and memory (11 0011 0000) */
               if (h_processor->trace) fprintf(stdout, "cxisa\t\t");
               {
                  int i_addr;
                  i_addr = (((h_processor->reg[C_REG]->nibble[6])<< 12) | (h_processor->reg[C_REG]->nibble[5] << 8) |
                     (h_processor->reg[C_REG]->nibble[4] << 4) | (h_processor->reg[C_REG]->nibble[3]));
                  if (i_addr > ROM_SIZE)
                     {
                        if (h_processor->trace) fprintf(stdout, "\n");
                        v_error(errno, h_err_invalid_address, i_addr, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
                     }
                  else
                  {
                     h_processor->reg[C_REG]->nibble[2] = (h_processor->rom[i_addr] >> 8) & 0xf;
                     h_processor->reg[C_REG]->nibble[1] = (h_processor->rom[i_addr] >> 4) & 0xf;
                     h_processor->reg[C_REG]->nibble[0] = (h_processor->rom[i_addr]) & 0xf;
                  }
               }
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x0d: /* c | a -> c - Load c with c or a (11 0111 0000) */
               if (h_processor->trace) fprintf(stdout, "c = c or a\t\t");
               h_processor->first = 0;
               h_processor->last = REG_SIZE - 1;
               v_reg_or(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[A_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;
            case 0x0e: /* c | a -> c - Load c with c and a (11 0111 0000) */
               if (h_processor->trace) fprintf(stdout, "c = c and a");
               h_processor->first = 0;
               h_processor->last = REG_SIZE - 1;
               v_reg_and(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[A_REG]);
               if (h_processor->trace) v_fprint_register(stdout, h_processor->reg[C_REG]);
               break;

            default:
               if (h_processor->trace) fprintf(stdout, "\n");
               /** debug(fprintf(stderr,"%02x\n", (i_opcode >> 6) & 0xf)); */
               v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 0x0e: /* {addr[11:4], nnnn} -> addr, reg[addr] -> c - Load c from register (nn nn11 1000) */
            if (i_opcode >> 6)
            {
               if (h_processor->trace) fprintf(stdout, "c = regn %-2d", i_opcode >> 6);
               h_processor->addr = (h_processor->addr & 0xff0) | (i_opcode >> 6);
            }
            else
               if (h_processor->trace) fprintf(stdout, "c = data\t\t");
            h_processor->first = 0;
            h_processor->last = REG_SIZE - 1;
            if ((i_translate_addr(h_processor->addr) < MEMORY_SIZE) && (h_processor->addr != 0x08) && (h_processor->addr != 0x18)) /* Treat registers 8 and 18 as if they dont exist */
               v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->mem[i_translate_addr(h_processor->addr)]);
            else
               v_reg_copy(h_processor, h_processor->reg[C_REG], NULL); /* Return zeros if memory doesnt' exist */
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[C_REG]);
               fprintf(stdout, "\taddr = %d", h_processor->addr);
            }
            break;
         case 0x0f: /* Rotate c register right (dddd 1111 00) */
            if (h_processor->trace) fprintf(stdout, "rcr %-3d\t\t", n_map_i[i_opcode >> 6]);
            {
               int i_count; ;
               for (i_count = n_map_i[i_opcode >> 6]; i_count > 0; i_count--)
               {
                  int c_temp, i_count;
                  c_temp = h_processor->reg[C_REG]->nibble[0];
                  for (i_count = 0; i_count < REG_SIZE; i_count++)
                     h_processor->reg[C_REG]->nibble[i_count] = h_processor->reg[C_REG]->nibble[i_count + 1];
                  h_processor->reg[C_REG]->nibble[REG_SIZE - 1] = c_temp;
                  h_processor->flags[PREV_CARRY] = h_processor->flags[CARRY] = False;
               }
            }
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;

         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            /** debug(fprintf(stderr,"%02x\n", (i_opcode >> 2) & 0xf)); */
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
#endif

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
      case 01: /* Type 1 - Branch instruction (two words) */
         {
            int i_address, i_next;
            i_address = i_opcode >> 2;
            i_next = h_processor->rom[h_processor->pc]; /* Get next opcode */
            i_address |= ((i_next & 0x3fc) << 6);
            if (h_processor->trace)
            {
               switch (i_next & 0x3)
               {
               case 0x0:
                  fprintf(stdout, "? nc gsb\t\t");
                  break;
               case 0x1:
                  fprintf(stdout, "? c gsb\t\t");
                  break;
               case 0x2:
                  fprintf(stdout, "? nc goto\t\t");
                  break;
               case 0x3:
                  fprintf(stdout, "? c goto\t\t");
                  break;
               }
               fprintf(stdout, "\n");
               fprintf(stdout, h_msg_opcode, (h_processor->pc >> 12), (h_processor->pc & 0x0fff), h_processor->rom[h_processor->pc]);
               fprintf(stdout,"  ");
               fprintf(stdout, h_msg_address, i_address);
            }
            h_processor->flags[CARRY] = h_processor->flags[PREV_CARRY]; /* Save carry */
            v_op_inc_pc(h_processor); /* Increment program counter */
            switch (i_next & 0x3)
            {
            case 0x0: /* if no carry then stack[2] -> stack[3], stack[1] -> stack[2], stack[0] -> stack[1], pc -> stack[0], a -> pc - Branch to subroutine on no carry (aa aaaa aa00, aa aaaa aa00) */
               if (i_address < ROM_SIZE) /* Check that the target address exists otherwise treat as a 'nop' */
                  if (!h_processor->flags[PREV_CARRY]) op_jsb(h_processor, i_address);
               break;
            case 0x1: /* if carry then stack[2] -> stack[3], stack[1] -> stack[2], stack[0] -> stack[1], pc -> stack[0], a -> pc - Branch to subroutine on no carry (aa aaaa aa00, aa aaaa aa01) */
               if (i_address < ROM_SIZE) /* Check that the target address exists otherwise treat as a 'nop' */
                  if (h_processor->flags[PREV_CARRY]) op_jsb(h_processor, i_address);
               break;
            case 0x2:  /* if no carry then a -> pc - Branch on no carry (aa aaaa aa00, aa aaaa aa10) */
               if (!h_processor->flags[PREV_CARRY])
                  h_processor->pc = i_address;
               break;
            case 0x3: /* if carry then a -> pc - Branch on carry (aa aaaa aa00, aa aaaa aa11) */
               if (h_processor->flags[PREV_CARRY])
                  h_processor->pc = i_address;
               break;
            }
         }
         break;
#else
      case 01: /* Type 1 - Jump subroutine */
         if (h_processor->trace) {fprintf(stdout, "jsb "); fprintf(stdout, h_msg_address, ((h_processor->pc & 0x0f00) | i_opcode >> 2));}
         op_jsb(h_processor, (i_opcode >> 2)); /* Note - uses and eight bit address */
         break;
#endif

#if defined(HP35) || defined(HP80) || defined(HP45) || defined(HP70) || defined(HP55)
      case 02: /* Type 2 - Arithmetic operations */
         i_field = (i_opcode >> 2) & 7;
         switch (i_field) /* Select field
            000   P  : determined by P                   [P]
            001   M  : mantissa                          [3 .. 12]
            010   X  : exponent                          [0 ..  1]
            011   W  : word                              [0 .. 13]
            100  WP  : word up to and including P        [0 ..  P]
            101  MS  : mantissa and sign                 [3 .. 13]
            110  XS  : exponent sign                     [2]
            111   S  : sign                              [13] */
         {
         case 00: /* P */
            s_field = "p";
            h_processor->first = h_processor->p; h_processor->last = h_processor->p;
            if (h_processor->p >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 01: /* M */
            s_field = "m";
            h_processor->first = EXP_SIZE; h_processor->last = REG_SIZE - 2;
            break;
         case 02: /* X */
            s_field = "x";
            h_processor->first = 0; h_processor->last = EXP_SIZE - 1;
            break;
         case 03: /* W */
            s_field = "w";
            h_processor->first = 0; h_processor->last = REG_SIZE - 1;
            break;
         case 04: /* WP */
            s_field = "wp";
            h_processor->first =  0; h_processor->last =  h_processor->p; /* break; bug in orig??? */
            if (h_processor->p >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 05: /* MS */
            s_field = "ms";
            h_processor->first = EXP_SIZE; h_processor->last = REG_SIZE - 1;
            break;
         case 06: /* XS */
            s_field = "xs";
            h_processor->first = EXP_SIZE - 1; h_processor->last = EXP_SIZE - 1;
            break;
         case 07: /* S */
            s_field = "s";
            h_processor->first = REG_SIZE - 1; h_processor->last = REG_SIZE - 1;
            break;
         }

         switch (i_opcode >> 5)
         {
         case 000: /* if b[f] = 0 */
            if (h_processor->trace) fprintf(stdout, "if b[%s] = 0", s_field);
            v_reg_test_eq(h_processor, h_processor->reg[B_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 001: /* 0 -> b[f] */
            if (h_processor->trace)fprintf(stdout, "0 -> b[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 002: /* if a >= c[f] */
            if (h_processor->trace) fprintf(stdout, "if a >= c[%s]", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[C_REG]); /* Less than */
            h_processor->flags[CARRY] = !h_processor->flags[CARRY];
            v_op_goto(h_processor);
            break;
         case 003: /* if c[f] != 0 */
            if (h_processor->trace) fprintf(stdout, "if c[%s] != 0", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[C_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 004: /* b -> c[f] */
            if (h_processor->trace) fprintf(stdout, "b -> c[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 005: /* 0 - c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 - c -> c[%s]", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 006: /* 0 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 -> c[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 007: /* 0 - c - 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 - c - 1 -> c[%s]\t\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 010: /* shift left a[f] */
            if (h_processor->trace) fprintf(stdout, "shift left a[%s]\t\t", s_field);
            v_reg_shl(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 011: /* a -> b[f] */
            if (h_processor->trace) fprintf(stdout, "a -> b[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 012: /* a - c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "a - c -> c[%s]\t\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 013: /* c - 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c - 1 -> c[%s]\t\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 014: /* c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "c -> a[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 015: /* if c[f] = 0 */
            if (h_processor->trace) fprintf(stdout, "if c[%s] = 0", s_field);
            v_reg_test_eq(h_processor, h_processor->reg[C_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 016: /* a + c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "a + c -> c[%s]\t\t", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 017: /* c + 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c + 1 -> c[%s]\t\t", s_field);
            v_reg_inc(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 020: /* if a >= b[f] */
            if (h_processor->trace) fprintf(stdout, "if a >= b[%s]", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[B_REG]); /* Less than */
            h_processor->flags[CARRY] = !h_processor->flags[CARRY];
            v_op_goto(h_processor);
            break;
         case 021: /* b exchange c[f] */
            if (h_processor->trace) fprintf(stdout, "b exch c[%s]\t\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[B_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[B_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 022: /* shift right c[f] */
            if (h_processor->trace) fprintf(stdout, "shift right c[%s]\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 023: /* if a[f] != 0 */
            if (h_processor->trace) fprintf(stdout, "if a[%s] != 0", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[A_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 024: /* shift right b[f] */
            v_reg_shr(h_processor, h_processor->reg[B_REG]);
            if (h_processor->trace) fprintf(stdout, "shift right b[%s]\t", s_field);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 025: /* c + c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c + c -> c[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 026: /* shift right a[f] */
            if (h_processor->trace) fprintf(stdout, "shift right a[%s]\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 027: /* 0 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "0 -> a[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 030: /* a - b -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - b -> a[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 031: /* a exch b[f] */
            if (h_processor->trace) fprintf(stdout, "a exch b[%s]\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[B_REG]);
            }
            break;
         case 032: /* a - c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - c -> a[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 033: /* a - 1 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - 1 -> a[%s]\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 034: /* a + b -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + b -> a[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 035: /* a exch c[f] */
            if (h_processor->trace) fprintf(stdout, "a exch c[%s]\t\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 036: /* a + c -> a[a + c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + c -> a[%s]", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 037: /* a + 1 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + 1 -> a[%s]", s_field);
            v_reg_inc(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
#endif

#if defined(WOODSTOCK) || defined(SPICE) || defined(HP10) || defined(HP67)
      case 02: /* Type 2 - Arithmetic operations */
         i_field = (i_opcode >> 2) & 7;
         switch (i_field) /* Select field
            000   P  : determined by P                   [P]
            001  WP  : word up to and including P        [0 ..  P]
            010  XS  : exponent sign                     [2]
            011   X  : exponent                          [0 ..  1]
            100   S  : sign                              [13]
            101   M  : mantissa                          [3 .. 12]
            110   W  : word                              [0 .. 13]
            111  MS  : mantissa and sign                 [3 .. 13] */
         {
         case 00: /* P */
            s_field = "p";
            h_processor->first = h_processor->p; h_processor->last = h_processor->p;
            if (h_processor->p >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 01: /* WP */
            s_field = "wp";
            h_processor->first =  0; h_processor->last =  h_processor->p; /* break; bug in orig??? */
            if (h_processor->p >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 02: /* XS */
            s_field = "xs";
            h_processor->first = EXP_SIZE - 1; h_processor->last = EXP_SIZE - 1;
            break;
         case 03: /* X */
            s_field = "x";
            h_processor->first = 0; h_processor->last = EXP_SIZE - 1;
            break;
         case 04: /* S */
            s_field = "s";
            h_processor->first = REG_SIZE - 1; h_processor->last = REG_SIZE - 1;
            break;
         case 05: /* M */
            s_field = "m";
            h_processor->first = EXP_SIZE; h_processor->last = REG_SIZE - 2;
            break;
         case 06: /* W */
            s_field = "w";
            h_processor->first = 0; h_processor->last = REG_SIZE - 1;
            break;
         case 07: /* MS */
            s_field = "ms";
            h_processor->first = EXP_SIZE; h_processor->last = REG_SIZE - 1;
            break;
         }

         switch (i_opcode >> 5)
         {
         case 000: /* 0 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "0 -> a[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 001: /* 0 -> b[f] */
            if (h_processor->trace)fprintf(stdout, "0 -> b[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 002: /* a exch b[f] */
            if (h_processor->trace) fprintf(stdout, "a exch b[%s]\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[B_REG]);
            }
            break;
         case 003: /* a -> b[f] */
            if (h_processor->trace) fprintf(stdout, "a -> b[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 004: /* a exch c[f] */
            if (h_processor->trace) fprintf(stdout, "a exch c[%s]\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 005: /* c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "c -> a[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 006: /* b -> c[f] */
            if (h_processor->trace) fprintf(stdout, "b -> c[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 007: /* b exchange c[f] */
            if (h_processor->trace) fprintf(stdout, "b exch c[%s]\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[B_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[B_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 010: /* 0 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 -> c[%s]\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 011: /* a + b -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + b -> a[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 012: /* a + c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + c -> a[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 013: /* c + c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c + c -> c[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 014: /* a + c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "a + c -> c[%s]\t", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 015: /* a + 1 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a + 1 -> a[%s]\t", s_field);
            v_reg_inc(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 016: /* shift left a[f] */
            if (h_processor->trace) fprintf(stdout, "shift left a[%s]\t", s_field);
            fflush(stdout);
            v_reg_shl(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 017: /* c + 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c + 1 -> c[%s]\t", s_field);
            v_reg_inc(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 020: /* a - b -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - b -> a[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 021: /* a - c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "a - c -> c[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 022: /* a - 1 -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - 1 -> a[%s]\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 023: /* c - 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "c - 1 -> c[%s]\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 024: /* 0 - c -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 - c -> c[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 025: /* 0 - c - 1 -> c[f] */
            if (h_processor->trace) fprintf(stdout, "0 - c - 1 -> c[%s]\t", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 026: /* if b[f] = 0 */
            if (h_processor->trace) fprintf(stdout, "if b[%s] = 0", s_field);
            v_reg_test_eq(h_processor, h_processor->reg[B_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 027: /* if c[f] = 0 */
            if (h_processor->trace) fprintf(stdout, "if c[%s] = 0", s_field);
            v_reg_test_eq(h_processor, h_processor->reg[C_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 030: /* if a >= c[f] */
            if (h_processor->trace) fprintf(stdout, "if a >= c[%s]", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[C_REG]); /* Less than */
            h_processor->flags[CARRY] = !h_processor->flags[CARRY];
            v_op_goto(h_processor);
            break;
         case 031: /* if a >= b[f] */
            if (h_processor->trace) fprintf(stdout, "if a >= b[%s]", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[B_REG]); /* Less than */
            h_processor->flags[CARRY] = !h_processor->flags[CARRY];
            v_op_goto(h_processor);
            break;
         case 032: /* if a[f] != 0 */
            if (h_processor->trace) fprintf(stdout, "if a[%s] != 0", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[A_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 033: /* if c[f] != 0 */
            if (h_processor->trace) fprintf(stdout, "if c[%s] != 0", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[C_REG], NULL);
            v_op_goto(h_processor);
            break;
         case 034: /* a - c -> a[f] */
            if (h_processor->trace) fprintf(stdout, "a - c -> a[%s]\t", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 035: /* shift right a[f] */
            if (h_processor->trace) fprintf(stdout, "shift right a[%s]\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 036: /* shift right b[f] */
            v_reg_shr(h_processor, h_processor->reg[B_REG]);
            if (h_processor->trace) fprintf(stdout, "shift right b[%s]\t", s_field);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 037: /* shift right c[f] */
            if (h_processor->trace) fprintf(stdout, "shift right c[%s]\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
#endif

#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
      case 0x02: /* Type 2 - Arithmetic operations */
         i_field = (i_opcode >> 2) & 7;
         switch (i_field) /* Select field
            000   R  : determined by active pointer            [R]
            001 S+X  : exponent and sign                       [0 .. 2]
            010  WR  : word up to and including active pointer [0 ..  P]
            011   W  : word                                    [0 .. 13]
            100 P-Q  : word from P up to and including Q       [P .. Q]
            101  XS  : exponent sign only                      [2]
            110   M  : mantissa                                [3 .. 12]
            111   S  : sign                                    [13] */
         {
         case 00: /* Active pointer */
            s_field = "pt";
            if (h_processor->ptr)
               h_processor->first = h_processor->last = h_processor->q;
            else
               h_processor->first = h_processor->last = h_processor->p;
            if (h_processor->p >= REG_SIZE || h_processor->q >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 01: /* Exponent and sign */
            s_field = "x";
            h_processor->first = 0; h_processor->last = EXP_SIZE - 1;
            break;
         case 02: /* Word up to the active pointer */
            s_field = "wpt";
            h_processor->first =  0;
            if (h_processor->ptr)
               h_processor->last = h_processor->q;
            else
               h_processor->last = h_processor->p;
            if (h_processor->p >= REG_SIZE || h_processor->q >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 03: /* All nibbles in the word */
            s_field = "all";
            h_processor->first = 0; h_processor->last = REG_SIZE - 1;
            break;
         case 04: /* Selects range based on pointers */
            s_field = "pq";
            h_processor->first = h_processor->p;
            if (h_processor->p > h_processor->q)
               h_processor->last = REG_SIZE - 1;
            else
               h_processor->last = h_processor->q;
            if (h_processor->p >= REG_SIZE || h_processor->q >= REG_SIZE)
            {
               if (h_processor->trace) fprintf(stdout, "\n");
               v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            }
            break;
         case 05: /* XS */
            s_field = "xs";
            h_processor->first = EXP_SIZE - 1; h_processor->last = EXP_SIZE - 1;
            break;
         case 06: /* M */
            s_field = "m";
            h_processor->first = EXP_SIZE; h_processor->last = REG_SIZE - 2;
            break;
         case 07: /* S */
            s_field = "s";
            h_processor->first = REG_SIZE - 1; h_processor->last = REG_SIZE - 1;
            break;
         }

         switch (i_opcode >> 5)
         {
         case 0x00: /* 0 -> a[f] - Clear a (00 000f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = 0 %s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x01: /* 0 -> b[f] - Clear b (00 001f ff10) */
            if (h_processor->trace)fprintf(stdout, "b = 0 %s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 0x02: /* 0 -> c[f] - Clear c (00 010f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = 0 %s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x03: /* a exch b[f] - Exchange a and b (00 011f ff10) */
            if (h_processor->trace) fprintf(stdout, "abex %s\t\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[B_REG]);
            }
            break;
         case 0x04: /* a -> b[f] - Load b from a (00 100f ff10) */
            if (h_processor->trace) fprintf(stdout, "b = a %s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[B_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 0x05: /* a exch c[f] - Exchange a and c (00 101f ff10) */
            if (h_processor->trace) fprintf(stdout, "acex %s\t\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[A_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 0x06: /* b -> c[f] - Load c from b (00 110f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = b %s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[C_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x07: /* b exch c[f] - Exchange b and c (00 111f ff10) */
            if (h_processor->trace) fprintf(stdout, "bcex %s\t\t", s_field);
            v_reg_exch(h_processor, h_processor->reg[B_REG], h_processor->reg[C_REG]);
            if (h_processor->trace)
            {
               v_fprint_register(stdout,h_processor->reg[B_REG]);
               v_fprint_register(stdout,h_processor->reg[C_REG]);
            }
            break;
         case 0x08: /* c -> a[f] - Load a from c (01 000f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = c %-3s\t\t", s_field);
            v_reg_copy(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x09: /* a + b -> a[f] - Load a with a plus b (01 001f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a + b %-3s", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0a: /* a + c -> a[f] - Load a with a plus c (01 010f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a + c %-3s", s_field);
            v_reg_add(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0b: /* a + 1 -> a[f] - Load a with a plus 1 (01 011f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a + 1 %-3s", s_field);
            v_reg_inc(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0c: /* a - b -> a[f] - Load a with a minus b (01 100f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a - b %-3s", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[B_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0d: /* a - 1 -> a[f] - Load a with a minus 1 (01 101f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a - 1 %-3s", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0e: /* a - c -> a[f] - Load a with a minus c (01 110f ff10) */
            if (h_processor->trace) fprintf(stdout, "a = a - c %-3s", s_field);
            v_reg_sub(h_processor, h_processor->reg[A_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x0f: /* c + c -> c[f] - Load c with c plus c (01 111f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = c + c %-3s", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x10: /* a + c -> c[f] - Load c with a plus c (10 000f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = c + a %-3s", s_field);
            v_reg_add(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x11: /* c + 1 -> c[f] - Load c with c plus 1 (10 001f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = c + 1 %-3s", s_field);
            v_reg_inc(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x12: /* c - a -> c[f] - Load c with c minus a (10 010f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = a - c %-3s", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[A_REG], h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x13: /* c - 1 -> c[f] - Load c with c minus 1 (10 011f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = c - 1 %-3s", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], h_processor->reg[C_REG], NULL);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x14: /* 0 - c -> c f - Load c with 0 minus c (10 100f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = 0 - c %-3s", s_field);
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x15: /* 0 - c - 1 -> c[f] - Complement c (10 101f ff10) */
            if (h_processor->trace) fprintf(stdout, "c = - c - 1 %-3s", s_field);
            h_processor->flags[CARRY] = True; /* Set carry */
            v_reg_sub(h_processor, h_processor->reg[C_REG], NULL, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x16: /* ? b[f] != 0 - Test b not equal to zero (10 110f ff10) */
            if (h_processor->trace) fprintf(stdout, "? b != 0 %-3s", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[B_REG], NULL);
            break;
         case 0x17: /* ? c != 0 - Test c not equal to zero (10 111f ff10) */
            if (h_processor->trace) fprintf(stdout, "? c != 0 %-3s", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[C_REG], NULL);
            break;
         case 0x18: /* ? a < c - Test a less than c (11 000f ff10) */
            if (h_processor->trace) fprintf(stdout, "? a < c %-3s", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[C_REG]);/* Less than */
            break;
         case 0x19: /* ? a < b - Test a less than b (11 001f ff10) */
            if (h_processor->trace) fprintf(stdout, "? a < b %-3s", s_field);
            v_reg_sub(h_processor, NULL, h_processor->reg[A_REG], h_processor->reg[B_REG]); /* Less than */
            break;
         case 0x1a: /* ? a != 0 - Test a not equal to zero  (11 010f ff10) */
            if (h_processor->trace) fprintf(stdout, "? a != 0 %-3s", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[A_REG], NULL);
            break;
         case 0x1b: /* ? a != c - Test a not equal to c (11 011f ff10) */
            if (h_processor->trace) fprintf(stdout, "? a != c %-3s", s_field);
            v_reg_test_ne(h_processor, h_processor->reg[A_REG], h_processor->reg[C_REG]);
            break;
         case 0x1c: /* a >> 1 -> a[f] - Shift right a (11 100f ff10) */
            if (h_processor->trace) fprintf(stdout, "shr a %s\t\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         case 0x1d: /* b >> 1 -> b[f] - Shift right b (11 101f ff10) */
            v_reg_shr(h_processor, h_processor->reg[B_REG]);
            if (h_processor->trace) fprintf(stdout, "shr b %s\t\t", s_field);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[B_REG]);
            break;
         case 0x1e: /* c >> 1 -> c[f] - Shift right c (11 110f ff10) */
            if (h_processor->trace) fprintf(stdout, "shr c %s\t\t", s_field);
            v_reg_shr(h_processor, h_processor->reg[C_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[C_REG]);
            break;
         case 0x1f: /* a << 1 -> a[f] - Shift left a (11 111f ff10) */
            if (h_processor->trace) fprintf(stdout, "shl a %s\t\t", s_field);
            fflush(stdout);
            v_reg_shl(h_processor, h_processor->reg[A_REG]);
            if (h_processor->trace) v_fprint_register(stdout,h_processor->reg[A_REG]);
            break;
         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
#endif

      case 03:/* Type 3 - Conditional branch */
         switch (i_opcode & 03)
         {
         case 00:
            if (h_processor->trace) {fprintf(stdout, "call "); fprintf(stdout, h_msg_address, i_opcode >> 2);}
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            break;
         case 01:
            if (h_processor->trace) {fprintf(stdout, "call "); fprintf(stdout, h_msg_address, i_opcode >> 2);}
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            break;
         case 02:
            if (h_processor->trace) {fprintf(stdout, "jump "); fprintf(stdout, h_msg_address, i_opcode >> 2);}
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_opcode, i_opcode, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
            break;
#if defined(HP10c) || defined(HP11c) || defined(HP12c) || defined(HP15c) || defined(HP16c)
         case 03: /* Relative jump */
            {
               int i_offset;
               i_offset = i_opcode >> 3;
               if (i_offset >= 0x40) i_offset = i_offset - 128; /* Fixed relative jump offset */
               if (i_opcode & 00004)
               {
                  if (h_processor->trace) fprintf(stdout, "jc ");
                  if (h_processor->flags[PREV_CARRY])
                     h_processor->pc = ((i_last + i_offset) & 0xffff);
               }
               else
               {
                  if (h_processor->trace) fprintf(stdout, "jnc ");
                  if (!h_processor->flags[PREV_CARRY])
                     h_processor->pc = ((i_last + i_offset) & 0xffff);
               }
               if (h_processor->trace)
               {
                  if (i_offset < 0)
                     fprintf(stdout, h_msg_negative_offset, abs(i_offset));
                  else
                     fprintf(stdout, h_msg_positive_offset , abs(i_offset));
               }
            }
            break;
#else
         case 03: /* if nc go to */
            if (h_processor->trace) {fprintf(stdout, "if nc go to "); fprintf(stdout, h_msg_address, ((h_processor->pc & 0x0f00) | i_opcode >> 2));} /* Note - uses and eight bit address */
            if (!h_processor->flags[PREV_CARRY])
            {
               h_processor->pc = (h_processor->pc & 0xff00) | i_opcode >> 2;
               v_delayed_rom(h_processor);
            }
            break;
#endif
         default:
            if (h_processor->trace) fprintf(stdout, "\n");
            v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
         }
         break;
      default:
         if (h_processor->trace) fprintf(stdout, "\n");
         v_error(errno, h_err_unexpected_error, (i_last >> 12), (i_last & 0xfff), __FILE__, __LINE__);
      }
      if (h_processor->trace) fprintf(stdout, "\n");
      h_processor->opcode = i_opcode; /* Keep track of the previous opcode so you know when to increment 'P' */
   }
}
