/*
 * This is Fossa, a grapical control application for analysis, synthesis, 
 * and manipulations of digitized sounds using Loris (Fitz and Haken). 
 *
 * Fossa is Copyright (c) 2001 - 2002 by Susanne Lefvert
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * fossa.c++
 *
 * Contains main method of the application.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H      // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include <qapplication.h> 
#include "fossaWindow.h"

// ---------------------------------------------------------------------------
//	Main method of program, starts the application.
// ---------------------------------------------------------------------------
int main( int argc, char **argv ){
  QApplication a(argc, argv);                //QApplication manages the GUI application's 
  FossaWindow fossaWindow(0,"FossaWindow");  //control flow and main settings
  a.setMainWidget(&fossaWindow);
  fossaWindow.show();
  return a.exec();
}
