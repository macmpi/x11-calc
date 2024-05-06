/*
 * gcc-wait.c
 *
 * Copyright(C) 2019 - MT
 *
 * Cross platform delay function.
 *
 * Waits for the specified number of milliseconds.
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
 * https://stackoverflow.com/questions/10053788
 *
 * 16 Aug 20         - Initial  verson (derived from original code used  in
 *                     gcc-cat.c) - MT
 * 30 Aug 21         - Changed  format statement to specify a long  integer
 *                     in the debug output (fixed warning) - MT
 * 03 Jan 21         - Changed debug() macro so that debug code is executed
 *                     when DEBUG is defined (doesn't need to be true) - MT
 * 31 Mar 22         - Modified to use usleep() on NetBSD - MT
 * 07 Feb 24         - Removed any windows specific or debug code, so don't
 *                     need to include stdio.h - MT
 * 27 Feb 24         - Fixed busy loop 'bug' on newer compilers that do not
 *                     set 'linux' when compiling on Linux - MT
 * 22 Apr 22         - Moved  compiler feature macro definitions to generic
 *                     busy loop - MT
 *
 */

#define NAME           "gcc-wait"
#define BUILD          "0005"
#define DATE           "07 Feb 24"
#define AUTHOR         "MT"

#if defined(linux) || defined(__linux__) || defined(__NetBSD__)
#include <unistd.h>
#include <sys/types.h>
#elif defined(VMS)
#include <timeb.h>
#include <lib$routines.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif

/*
 * wait (milliseconds)
 *
 * Waits for the specified number of milliseconds
 *
 * 16 Aug 20         - Initial version taken from gcc-cat.c - MT
 * 04 Sep 21         - Fixed formatting in debug code - MT
 * 07 Feb 24         - Removed any windows specific or debug code - MT
 *
 */
int i_wait(long l_delay)
{
#if defined(linux) || defined(__linux__) || defined(__NetBSD__)
return (usleep(l_delay * 1000)); /* Use usleep() function */
#elif defined(VMS)
float f_seconds;
f_seconds = l_delay / 1000.0;
return (lib$wait(&f_seconds)); /* Use VMS LIB$WAIT */
#else
/** #define _DEFAULT_SOURCE /* Possibly required for busy loop more testing needed */
/** #define _BSD_SOURCE /* Possibly required for busy loop more testing needed */
struct timeb o_start, o_end;
ftime(&o_start);
ftime(&o_end);
while ((1000 * (o_end.time - o_start.time) + o_end.millitm - o_start.millitm) < l_delay) /* Use a portable but very inefficent busy loop */
{
   ftime(&o_end);
}
return(0);
#endif
}
