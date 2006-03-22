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
 * soundPixmap.c++
 *
 * SoundPixmap provides an area with x and y axes where breakpoints of a
 * collection of sound are plotted over time.
 *
 * Susanne Lefvert, 1 March 2002
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "axis.h"
#include "soundPixmap.h"
#include "currentSoundView.h"

using std::list;

// ---------------------------------------------------------------------------
//      SoundPixmap constructor
// ---------------------------------------------------------------------------
// Takes a list of sound to be plotted over time, and the maximum x and y value
SoundPixmap::SoundPixmap(
	list<Loris::Partial>*	p,
	double			x,
	double			y,
	Tab::TabType		t
):QPixmap(800, 450){
  maxTime	= x;
  maxY		= y;
  partialList	= p;
  text		= "";
  type		= t;

  leftMargin	= 45;
  rightMargin	= 5;	//This MUST be 5 less than SoundPlot::rightMargin
  topMargin	= 10;
  bottomMargin	= 25;	//This MUST be 5 less than SoundPlot::bottomMargin
  fill(QColor("white"));

  horizontalIndex = maxTime/(width() - leftMargin - rightMargin);
  verticalIndex = maxY/(height() - topMargin - bottomMargin);

  //On startup there has to be an empty Pixmap.
  if( type != Tab::empty )
    plotPartials();
}

// ---------------------------------------------------------------------------
//	plotPartials
// ---------------------------------------------------------------------------
// Actually draw the partials.
void SoundPixmap::plotPartials(){
  list<Loris::Partial>::const_iterator it;
  Partial_ConstIterator pIt;

  int x;
  int y;
  int lastX;
  int lastY;
  QPainter bPainter(this);
  QPainter painter(this);

  //Have 2 painters, one black and one red/green/blue.
  bPainter.setPen(Qt::black);

  if( type == Tab::empty ) return;

  // loop through all partials in the list 
  for( it = partialList->begin(); it != partialList->end(); it++){
    pIt = it->begin();
    //As an optimization, do the test for type outside of the loop, and hard-
    //code each of the three loops to plot amp/freq/bandw so that the test
    //is not done for each breakpoint, only for each partial.
    //The best way to do this is really with a function pointer...
    switch( type ){
      case Tab::amplitude:
        //Draw the first Breakpoint, then loop through the rest, drawing lines
        //between them.
        x = toX(pIt.time());
        y = toY(pIt->amplitude());
        lastX = x;
        lastY = y;
        bPainter.drawRect (x, y, 2, 2);
      
        painter.setPen(Qt::red);                      
        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->amplitude());
          bPainter.drawRect (x, y, 2, 2); 

          // draw a read line connecting breakpoints
          painter.drawLine (lastX, lastY, x, y); 
          lastX = x; 
          lastY = y;
        } 
        break;

      case Tab::frequency:
        x = toX(pIt.time());
        y = toY(pIt->frequency());
        lastX = x;
        lastY = y;
        bPainter.drawRect (x, y, 2, 2);
      
        painter.setPen(Qt::green);                      
        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->frequency());
          bPainter.drawRect (x, y, 2, 2); 

          // draw a read line connecting breakpoints
          painter.drawLine (lastX, lastY, x, y); 
          lastX = x; 
          lastY = y;
        } 
        break;

      case Tab::noise:
        x = toX(pIt.time());
        y = toY(pIt->bandwidth());
        lastX = x;
        lastY = y;
        bPainter.drawRect (x, y, 2, 2);
      
        painter.setPen(Qt::blue);                      
        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->bandwidth());
          bPainter.drawRect (x, y, 2, 2); 

          // draw a read line connecting breakpoints
          painter.drawLine (lastX, lastY, x, y); 
          lastX = x; 
          lastY = y;
        } 
        break;

      case Tab::empty:
        break;
    }	//switch

  }	// for

  painter.flush();
}

// ---------------------------------------------------------------------------
//      toX
// ---------------------------------------------------------------------------
// Translates an actual time value into the corresponding value on the pixmap
// into a pixel coordinate.
int SoundPixmap::toX(double time){
  return (int)((time / horizontalIndex)  + leftMargin);
}

// ---------------------------------------------------------------------------
//      toY
// ---------------------------------------------------------------------------
// Translates an actual y value into the corresponding value on the pixmap
// into a pixel coordinate.
int SoundPixmap::toY(double value){
  return (int)(height() - (value / verticalIndex) - bottomMargin);
}

// ---------------------------------------------------------------------------
//      inArea
// ---------------------------------------------------------------------------
// Check if a point is in the area between the margins.
bool SoundPixmap::inArea(int x, int y){
  bool xIn = x >= leftMargin && x <= width()-rightMargin; 
  bool yIn = y >= topMargin &&  y <= height()-bottomMargin;
  return(xIn && yIn);
}
