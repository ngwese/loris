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
 * partialsPixmap.c++
 *
 * The PartialsPixmap class is the base class for AmplitudePixmap, FrequencyPixmap, 
 * NoisePixmap, and EmptyPixmap classes.
 * It provides an area with x and y axis where breakpoints of a collection of partials
 * can get plotted over time. Subclasses have to implement plotPartials().
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "partialsPixmap.h"

// ---------------------------------------------------------------------------
//      PartialsPixmap constructor
// ---------------------------------------------------------------------------
// Takes a list of partials to be plotted over time, and the maximum x and y value

PartialsPixmap::PartialsPixmap(list<Loris::Partial>* p, double x, double y):QPixmap(800,450){
  maxTime  = x;
  maxY     = y;
  partialList = p;
  text = "";

  leftMargin   = 40;
  rightMargin  = 30;
  topMargin    = 30;
  bottomMargin = 20;
  fill(QColor("white"));
}

// ---------------------------------------------------------------------------
//      addAxis
// ---------------------------------------------------------------------------
// adds axis to the area, given start x and y positions, the length and width of the axis, 
// number of tickmarks, minimum and maximum value, and if the axis should be horisontal
// or vertical.

void PartialsPixmap::addAxis(double startX, double startY, QString label, double length, int width, double ticks, double minVal, double maxVal, bool vertical){
  
  maxVal = adjustValue(maxVal);  // rounds up the maximum value  0.006 -> 0.01
  double stepValue  = (maxVal - minVal)/ticks; // the actual value between two consecutive
  double stepLength = length/ticks;            // tickmarks
  double textX      = 0;       // start position of text
  double numberX    = 0;       // start position of axis numbers 
  double number     = 0;       // axis numbers
  int thicker       = 0;       // indicates which tickmark should be bigger
  QFont f( "helvetica", 10);
  QPainter painter(this);
  painter.setFont(f);

  if(vertical){ 
    textX   = startX - 20;
    numberX = startX - 30;
      
    painter.setFont(f);
    painter.setPen(Qt::black);
    painter.drawLine(startX, startY, startX, startY - length); 
    painter.drawText(textX - label.length(), startY - length - 20, label);
    
    number = minVal;
    thicker = 0;
   
    for(double y = startY; y > startY - length - stepLength; y = y - stepLength){
      painter.drawLine(startX-2, y, startX+2, y); 
      
      if(thicker%10==0){
	painter.drawLine(startX - 5, y, startX + 2, y); 
	painter.drawText(numberX, y + stepLength, QString("%1 ").arg(number));
      }
      number = number + stepValue;
      thicker++;
    }

    verticalIndex = stepValue/stepLength;
  }
 
  else{  // horisontal
    
    painter.drawLine(startX, startY, startX + length, startY);
    painter.drawText(startX + length, startY + 20, label);
    
    number = minVal;
    thicker = 0;
    
    for(double x = startX; x < length + startX + stepLength; x = x + stepLength){
      painter.drawLine(x, startY-2, x, startY+2); 
      if(thicker%10==0){
	painter.drawLine(x, startY - 5, x, startY + 2); 
	painter.drawText(x-5,startY+12, QString("%1").arg(number));
      }
      number = number + stepValue;
      thicker++;
    }
  }
  horizontalIndex = stepValue/stepLength;
}

// ---------------------------------------------------------------------------
//      adjustValue
// ---------------------------------------------------------------------------
// Rounds up the given value. Example 0.006 would become 0.01.
// This is used to know what the maximum value should be on the axis.

double PartialsPixmap::adjustValue(double value){
  double i      = 1;
  double result = 0;
  
  if(value != 0){
    
    if(value >= 1){           // else 0.006 would become 1   
      result = ceil(value);         
    }
    
    else{
      for(value; value < 1; value = value * 10){ // i for 0.006 = 100
	i = i * 10;                              
      }
      result = ceil(value)/i;  // 1/100 = 0.01                      
    } 
    return result;
  }
}

// ---------------------------------------------------------------------------
//      toX
// ---------------------------------------------------------------------------
// Translates an actual time value into the corresponding value on the pixmap.  

double PartialsPixmap::toX(double time){
  return ((time / horizontalIndex)  + leftMargin);
}

// ---------------------------------------------------------------------------
//      toY
// ---------------------------------------------------------------------------
// Translates an actual y value into the corresponding value on the pixmap.  

double PartialsPixmap::toY(double value){
  return-((value / verticalIndex) - height() + bottomMargin);
}

// ---------------------------------------------------------------------------
//      inArea
// ---------------------------------------------------------------------------
// Check if a point is in the area between the margins (is the axis) 

bool PartialsPixmap::inArea(int x, int y){
  bool xIn = x >= leftMargin && x<= width()-rightMargin; 
  bool yIn = y >= topMargin &&  y<= height()-bottomMargin;
  return(xIn && yIn);
}

// ---------------------------------------------------------------------------
//      AmplitudePixmap constructor
// ---------------------------------------------------------------------------

AmplitudePixmap::AmplitudePixmap(list<Loris::Partial>* p, double x, double y):PartialsPixmap(p, x, y){
  PartialsPixmap::text = "amplitude";
   // y axis
  addAxis(leftMargin, height() - bottomMargin, text, 
	  height()-bottomMargin-topMargin, 30, 100,  0, maxY, true); 
  
  //x axis
  addAxis(leftMargin, height() - bottomMargin, "time", 
	  width()-rightMargin-leftMargin, 30, 100,  0, maxTime, false);
  
  plotPartials();
}

// ---------------------------------------------------------------------------
//      plotPartials
// ---------------------------------------------------------------------------
// have to be implemented when class is subclassing PartialsPixmap.

void AmplitudePixmap::plotPartials(){ 
  
  double x;
  double y;
  double lastX;
  double lastY;
  QPainter painter(this);
  painter.setPen(Qt::black);
  
  // loop through all partials in the list 
  for(list<Loris::Partial>::const_iterator it = partialList->begin(); it != partialList->end(); it++){
    PartialConstIterator pIt = it->begin();  // no line should be added to the
    x = toX(pIt.time());                     // first breakpoint in a partial
    y = toY(pIt->amplitude());
    lastX = x;                   // to know where to connect the line.
    lastY = y;
    
    painter.drawRect (x-1, y-1, 2, 2);  // the rect is painted around x and y values.
      
    pIt++;
  
    // loop through the breakpoints in a partial (except from the first) and connect
    // them with a line.
    for(pIt; pIt != it->end(); pIt++){
      x = toX(pIt.time());
      y = toY(pIt->amplitude());
      painter.setPen(Qt::black);
      painter.drawRect (x-1, y-1, 2, 2); 
      painter.setPen(Qt::red);                      
      painter.drawLine (lastX-1, lastY-1, x-1, y-1); // draw a read line connecting breakpoints
      lastX = x; 
      lastY = y;
    } 
  }
}

// ---------------------------------------------------------------------------
//      FrequencyPixmap constructor
// ---------------------------------------------------------------------------

FrequencyPixmap::FrequencyPixmap(list<Loris::Partial>* p, double x, double y):PartialsPixmap(p, x, y){
  PartialsPixmap::text = "frequency";
  
  //y axis
  addAxis(leftMargin, height() - bottomMargin, text, 
	  height()-bottomMargin-topMargin, 30, 100,  0, maxY, true); 
  
  //x axis
  addAxis(leftMargin, height() - bottomMargin, "time", 
	  width()-rightMargin-leftMargin, 30, 100,  0, maxTime, false);
 
  plotPartials();
}

// ---------------------------------------------------------------------------
//      plotPartials
// ---------------------------------------------------------------------------
// have to be implemented when class is subclassing PartialsPixmap.

void FrequencyPixmap::plotPartials(){ 
 
  QPainter painter(this);
  double x;
  double y;
  double lastX;
  double lastY;
  QPen pen(QColor("black"));
  painter.setPen(pen);

  // loop through all partials in the list and plot them connected with a line
  for(list<Partial>::const_iterator it = partialList->begin(); it != partialList->end(); it++){
    
    PartialConstIterator pIt = it->begin();  // no line should be added to the
    x = toX(pIt.time());                     // first breakpoint in a partial
    y = toY(pIt->frequency());
    lastX = x; 
    lastY = y;
    painter.drawRect (x-1, y-1, 2, 2); 
    
    pIt++;
    
    // loop through the breakpoints in a partial (except from the first) and connect
    // them with a line.
    for(pIt; pIt != it->end(); pIt++){
      x = toX(pIt.time());
      y = toY(pIt->frequency());
      painter.setPen(Qt::black);
      painter.drawRect (x-1, y-1, 2, 2); 
      painter.setPen(Qt::red);
      painter.drawLine (lastX-1, lastY-1, x-1, y-1); // draw a read line connecting breakpoints
      
      lastX = x; 
      lastY = y;
    } 
  }
}

// ---------------------------------------------------------------------------
//      NoisePixmap constructor
// ---------------------------------------------------------------------------

NoisePixmap::NoisePixmap(list<Loris::Partial>* p, double x, double y):PartialsPixmap(p, x, y){
  PartialsPixmap::text = "noise";
  
  //y axis 
  addAxis(leftMargin, height() - bottomMargin, text, 
	  height()-bottomMargin-topMargin, 30, 100,  0, maxY, true); 
  
  //x axis
  addAxis(leftMargin, height() - bottomMargin, "time", 
	  width()-rightMargin-leftMargin, 30, 100,  0, maxTime, false);
  plotPartials();
}

// ---------------------------------------------------------------------------
//      plotPartials
// ---------------------------------------------------------------------------
// have to be implemented when class is subclassing PartialsPixmap.

void NoisePixmap::plotPartials(){ 
  QPainter painter(this);
  double x;
  double y;
  double lastX;
  double lastY;
  painter.setPen(QColor("black"));

  // loop through all partials in the list and plot them connected with a line
  for(list<Partial>::const_iterator it = partialList->begin(); it != partialList->end(); it++){
    PartialConstIterator pIt = it->begin();
    x = toX(pIt.time());
    y = toY(pIt->bandwidth());
    lastX = x; 
    lastY = y;
    painter.drawRect (x+1, y+1, 2, 2); 
    
    pIt++;
    
    // loop through the breakpoints in a partial (except from the first) and connect
    // them with a line.
    for(pIt; pIt != it->end(); pIt++){
      x = toX(pIt.time());
      y = toY(pIt->bandwidth());
      
      painter.setPen(Qt::black);
      painter.drawRect (x-1, y-1, 2, 2); 
      painter.setPen(Qt::red);
      painter.drawLine (lastX-1, lastY-1, x-1, y-1); // draw a read line connecting breakpoints
      
      lastX = x; 
      lastY = y;
    }
  }
}

// ---------------------------------------------------------------------------
//      EmptyPixmap constructor
// ---------------------------------------------------------------------------
// Class used when PartialsList is empty.

EmptyPixmap::EmptyPixmap(list<Loris::Partial>* p, double x, double y):PartialsPixmap(p, x, y){
  PartialsPixmap::text = "";
  plotPartials();
}

// ---------------------------------------------------------------------------
//      plotPartials
// ---------------------------------------------------------------------------
// have to be implemented when class is subclassing PartialsPixmap.
// No partials are plotted, the pixmap is empty.

void EmptyPixmap::plotPartials(){ 
  QPainter painter(this);
  QPen pen(QColor("black"));
  painter.setPen(pen);
}















