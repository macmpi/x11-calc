/*
 * x11-calc-button.c - RPN (Reverse Polish) calculator simulator.
 *
 * Copyright(C) 2013   MT
 *
 * Button functions.
 *
 * Implements  the  routines needed to create the data structures  used  to
 * hold the properties and the functions needed to create, draw, and  check
 * if a button has been clicked.
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
 * 03 Jul 13         - Initial verson with new button style - MT
 * 04 Jul 13         - Optimized the number of drawing operations - MT
 * 07 Jul 13         - Tuned position of font - MT
 * 14 Jul 13         - Flush X buffer each time button is drawn - MT
 * 15 Dec 18         - Changed debug macro and added an error macro - MT
 * 08 Aug 21         - Tidied up spelling errors in the comments - MT
 * 30 Sep 21         - Added the additional properties for the label colour
 *                     and label function colour - MT
 * 10 Oct 21         - Allow use of NULL pointers - MT
 * 23 Nov 21         - Added an additional parameter to allow the alternate
 *                     function text to be defined (can't draw it yet) - MT
 * 25 Nov 21         - Can now draw the alternate function text next to the
 *                     function text if it exists - MT
 * 06 Dec 21         - Put the alternate function labels below the key when
 *                     compiling the HP67 simulation - MT
 *                   - Label text colour now explicitly defined to allow it
 *                     to be different from the main text colour- MT
 * 24 Dec 21         - Added the ability for selected models to use a flat
 *                     button style - MT
 * 03 Jan 21         - Changed debug() macro so that debug code is executed
 *                     when DEBUG is defined (doesn't need to be true) - MT
 * 09 Feb 22         - Positions the function text based on the button size
 *                     when drawing it underneath button (instead of  using
 *                     the row height, which does not work when the spacing
 *                     between rows is increased) - MT
 * 12 Feb 22         - Added a style property, currently used to allow flat
 *                     buttons to be drawn - MT
 * 26 Nov 22         - Added support for the original HP10 - MT
 * 24 Feb 24         - Do not need to include "x11-font.h" - MT
 * 03 Mar 24         - Updated error handling (now passes the  error number
 *                     to the error handler) - MT
 * 25 Mar 24         - If the button is pressed the text now moves down one
 *                     pixel if using the classic button style, not up like
 *                     the later models - MT
 * 13 Apr 24         - Uses the key height to draw a button with horizontal
 *                     dividing line in the same place regardless of aspect
 *                     ratio - MT
 * 23 Apr 24         - Separated out prototypes for error handlers - MT
 *
 * To Do             - Add a new style to handle the type of button used by
 *                     the classic series.
 */

#define NAME           "x11-calc-button"
#define BUILD          "0020"
#define DATE           "25 Mar 24"
#define AUTHOR         "MT"

#include <errno.h>     /* errno */

#include <string.h>    /* strlen(), etc. */
#include <stdio.h>     /* fprintf(), etc. */
#include <stdlib.h>    /* malloc(), etc. */

#include <X11/Xlib.h>  /* XOpenDisplay(), etc. */
#include <X11/Xutil.h> /* XSizeHints etc. */

#include "x11-calc-messages.h"
#include "x11-calc-errors.h"

#include "x11-calc-label.h"
#include "x11-calc-switch.h"
#include "x11-calc-button.h"
#include "x11-calc-colour.h"

#include "x11-calc.h"

#include "gcc-debug.h"

/*
 * button_key_pressed (button, key)
 *
 * If the x and y co-ordinate are within the area of the button this
 * function returns a pointer to the button, or a NULL otherwise.
 *
 * 01 Jul 13         - Initial version - MT
 *
 */

obutton *h_button_key_pressed(obutton *h_button, char c_key){

   if (h_button != NULL) {
      if (h_button->key != '\000') {
         if (c_key == h_button->key)
            return(h_button);
      }
   }
   return(NULL);
}
/* button_pressed (button, x, y) */

obutton *h_button_pressed(obutton *h_button, int i_xpos, int i_ypos){

   int i_indent, i_extent, i_upper, i_lower;

   if (h_button != NULL) {
      i_indent = h_button->button_position.x;
      i_extent = h_button->button_position.x + h_button->button_position.width;
      i_upper = h_button->button_position.y;
      i_lower = h_button->button_position.y + h_button->button_position.height;

      if (((i_xpos > i_indent ) && (i_xpos < i_extent)) &&
         ((i_ypos > i_upper ) && (i_ypos < i_lower))) {
         return(h_button);
      }
   }
   return(NULL);
}

/*button_create (index, key, text, label ,label, font, function_font,
 *                label_font, left, top, width, height,
 *                state, colour)
 */

obutton *h_button_create(int i_index, char c_key,
   char* s_text, char* s_function ,char* s_alternate ,char* s_label,
   XFontStruct *h_normal_font, XFontStruct *h_shift_font, XFontStruct *h_label_font,
   int i_left, int i_top, int i_width, int i_height,
   int i_state, int i_style,
   unsigned int i_colour, unsigned int i_function_colour,
   unsigned int i_shifted_colour, unsigned int i_label_colour) {

   obutton *h_button; /* Ponter to button. */

   /* Attempt to allocate memory for a button. */
   if ((h_button = malloc (sizeof(*h_button)))==NULL) v_error(errno, h_err_memmory_alloc, __FILE__, __LINE__);

   h_button->index = i_index;
   h_button->key = c_key;
   h_button->text = s_text;
   h_button->function = s_function;
   h_button->alternate = s_alternate;
   h_button->label = s_label;
   h_button->text_font = h_normal_font;
   h_button->function_font = h_shift_font;
   h_button->label_font = h_label_font;

   h_button->button_position.x = i_left;
   h_button->button_position.y = i_top;
   h_button->button_position.width = i_width;
   h_button->button_position.height = i_height;

   h_button->button_geometry = h_button->button_position; /* Save position */

   h_button->state = i_state;
   h_button->style = i_style;
   h_button->colour = i_colour;
   h_button->function_colour = i_function_colour;
   h_button->shifted_colour = i_shifted_colour;
   h_button->label_colour = i_label_colour;
   return(h_button);
}

/*
 * button_resize (display, scale)
 *
 * Resize button based on original geometery
 *
 */

int i_button_resize(obutton *h_button, float f_scale)
{
   h_button->button_position.x = h_button->button_geometry.x * f_scale;
   h_button->button_position.y = h_button->button_geometry.y * f_scale;
   h_button->button_position.width = h_button->button_geometry.width * f_scale;
   h_button->button_position.height = h_button->button_geometry.height * f_scale;

   return 0;
}


/* button_draw (display, window, screen, button) */

int i_button_draw(Display *h_display, int x_application_window, int i_screen, obutton *h_button) {

   int i_indent, i_extent, i_upper, i_lower;
   int i_offset;

   if (h_button != NULL) {

      /* Draw the button background on the window. */
      XSetForeground(h_display, DefaultGC(h_display, i_screen), DARK_TEXT); /* Set button background colour - as foreground colour! */
      i_upper = h_button->button_position.y;
      i_lower = h_button->button_position.y + h_button->button_position.height - 2;
      i_indent = h_button->button_position.x + 1;
      i_extent = h_button->button_position.x + h_button->button_position.width - 2;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , h_button->button_position.y , i_extent, h_button->button_position.y); /* Top of background */
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_lower, i_extent, i_lower); /* Bottom of background */
      i_upper++;
      XFillRectangle(h_display, x_application_window, DefaultGC(h_display, i_screen), h_button->button_position.x, i_upper , h_button->button_position.width, h_button->button_position.height - 3); /* Fill in background */
      i_indent = i_indent + 2;
      i_extent = i_extent - 2;
      i_lower++;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_lower, i_extent, i_lower); /* Extend bottom of background to make it look more curved */

      /* Draw the button face on the background.  */
      XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->colour); /* Set the foreground colour. */
      i_lower = i_lower - 3;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_lower, i_extent, i_lower); /* Bottom of button */
      i_upper = i_upper + 2;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_upper, i_extent, i_upper); /* Top edge of button */
      i_indent--; i_extent++; i_upper++; i_lower--;
      XFillRectangle(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_upper, h_button->button_position.width - 4, h_button->button_position.height - 8 ); /* Fill in button face */
      i_lower = i_lower + 2;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent + 3, i_lower, i_extent -3 , i_lower); /* Extra padding at bottom of button intended to make it look more curved */

      /* Select appropriate colour for high-light (or low-light) depending on button state. */
      if ((h_button->state)) { /* Set the foreground colour to darker tint of the base colour. */
         if (((h_button->colour & 0xff) + (h_button->colour >> 8 & 0xff) + (h_button->colour >> 16 & 0xff)) > 384)
            XSetForeground(h_display, DefaultGC(h_display, i_screen), i_shade(h_button->colour));
      }
      else  /* Set the foreground colour to lighter tint of the base colour. */
         XSetForeground(h_display, DefaultGC(h_display, i_screen), i_tint(h_button->colour));
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_lower - 2, i_indent , i_upper); /* Left hand highlight */
      i_indent++; i_extent--; i_upper--;
      XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_upper, i_extent , i_upper); /* Top highlight */

      XSetForeground(h_display, DefaultGC(h_display, i_screen), i_tint(h_button->colour)); /* Set the foreground colour to lighter tint of the base colour. */

      if (h_button->style == 0)
      {
         i_offset = h_button->button_position.y + 2 + (h_button->button_position.height - (KEY_HEIGHT / 2 * h_button->button_position.height / h_button->button_geometry.height));
         i_upper = i_upper + (1 + i_offset - i_upper + h_button->text_font->ascent + h_button->text_font->descent) / 2 - h_button->text_font->descent; /* Find vertical position of text */
      }
      else
      {
         i_offset = h_button->button_position.y + 2 + h_button->button_position.height / 2; /* Find middle of button. */
         i_upper = i_offset + (h_button->text_font->ascent + h_button->text_font->descent) / 2 - h_button->text_font->descent - 2; /* Find vertical position of text */
      }
      i_lower = i_offset + (i_lower - i_offset + h_button->label_font->ascent + h_button->label_font->descent) / 2 - h_button->label_font->descent;

      if (h_button->style == 0)
      {
         if ((h_button->state)){ /* Is the button pressed? */
            i_offset--;
            i_upper--;
         }
         else {
            i_lower++;
         }
      }
      else
      {
         if ((h_button->state)){ /* Is the button pressed? */
            i_lower++;
         }
         else {
            i_offset--;
            i_upper--;
         }
      }

      if (h_button->style == 0)
         XDrawLine(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent , i_offset, i_extent , i_offset); /* Horizontal highlight. */

      /* Set text foreground colour based on the the brightness of the button colour and font. */
      if (((h_button->colour & 0xff) + (h_button->colour >> 8 & 0xff) + (h_button->colour >> 16 & 0xff))  > 384)
         XSetForeground(h_display, DefaultGC(h_display, i_screen), DARK_TEXT);
      else
         XSetForeground(h_display, DefaultGC(h_display, i_screen), LIGHT_TEXT);

      XSetFont(h_display, DefaultGC(h_display, i_screen), h_button->text_font->fid); /* Set the text font. */
      if (h_button->button_position.width < h_button->button_position.height)
      {
         int i_count;
         i_indent = 1 + h_button->button_position.x + (h_button->button_position.width - XTextWidth(h_button->text_font, h_button->text, 1)) / 2; /* Find position of the text. */
         if (strlen(h_button->text) > 1)
            i_upper = i_upper - ((h_button->text_font->ascent ) * (strlen(h_button->text) - 1)) / 2;

         for (i_count = 0; i_count < (strlen(h_button->text)); i_count++)
         {
            XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_upper ,&h_button->text[i_count], 1); /* Draw the main text. */
            i_upper += h_button->text_font->ascent;
         }
      }
      else
      {
         i_indent = 1 + h_button->button_position.x + (h_button->button_position.width - XTextWidth(h_button->text_font, h_button->text, strlen(h_button->text))) / 2; /* Find position of the text. */
         XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_upper ,h_button->text, strlen(h_button->text)); /* Draw the main text. */
      }

      XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->label_colour); /* Draw label text */
      if (!strlen(h_button->alternate))
         XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->shifted_colour); /* No alternate function defined so use the alternate function colour for the label */
      XSetFont(h_display, DefaultGC(h_display, i_screen), h_button->label_font->fid); /* Select the label text font */
      i_indent = 1 + h_button->button_position.x + (h_button->button_position.width - XTextWidth(h_button->label_font, h_button->label, strlen(h_button->label))) / 2; /* Find position of the label text */
      XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_lower, h_button->label, strlen(h_button->label)); /* Draw the label text */

      /* Draw function */
      if (strlen(h_button->alternate))
      {
         XSetFont(h_display, DefaultGC(h_display, i_screen), h_button->function_font->fid); /* Select the function text font */
         XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->shifted_colour); /* Use the function text colour */
         i_indent = 3 + h_button->button_position.x + (h_button->button_position.width / 2)
            + (XTextWidth(h_button->function_font, h_button->function, strlen(h_button->function)) + XTextWidth(h_button->function_font, h_button->alternate, strlen(h_button->alternate))) / 2
            - XTextWidth(h_button->function_font, h_button->alternate, strlen(h_button->alternate)); /* Find position of the alternate text. */
#ifdef HP67
         i_upper = h_button->button_position.y + h_button->button_position.height + h_button->function_font->ascent + 1; /* Draw the function text 1 pixel below button */
#else
         i_upper = h_button->button_position.y - (h_button->function_font->descent + 1); /* Draw the function text 1 pixel above button */
#endif
         XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_upper ,h_button->alternate, strlen(h_button->alternate)); /* Draw the function text */
         XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->function_colour); /* Use the function text colour */
         i_indent = i_indent - 2 - XTextWidth(h_button->function_font, h_button->function, strlen(h_button->function)); /* Find position of the function text. */
      }
      else
      {
         XSetForeground(h_display, DefaultGC(h_display, i_screen), h_button->function_colour); /* Use the function text colour */
         XSetFont(h_display, DefaultGC(h_display, i_screen), h_button->function_font->fid); /* Select the function text font */
         i_indent = 1 + h_button->button_position.x + (h_button->button_position.width - XTextWidth(h_button->function_font, h_button->function, strlen(h_button->function))) / 2; /* Find position of the function text. */
#ifdef HP67
         i_upper = h_button->button_position.y + h_button->button_position.height + h_button->function_font->ascent + 1; /* Draw the function text 1 pixel below button */
#else
         i_upper = h_button->button_position.y - (h_button->function_font->descent + 1); /* Draw the function text 1 pixel above button */
#endif
      }
      XDrawString(h_display, x_application_window, DefaultGC(h_display, i_screen), i_indent, i_upper ,h_button->function, strlen(h_button->function)); /* Draw the function text */

   }
   return(True);
}
