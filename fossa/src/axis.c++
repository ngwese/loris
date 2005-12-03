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
 * partialsList.c++
 *
 * The PartialsList class is the main model of the application, ie. changes in 
 * PartialsList will trigger updates in view classes. This class has a list
 * containing imported partials and partials produced from different 
 * manipulations. One partials in the list is always set to current partials, 
 * and modifications can be made only to current partials. 
 * PartialsList also keeps track of two partials which are selected when 
 * the user decides to make a morph between two sounds. The class 
 * communicates with LorisInterface for necessary operations on partials.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "axis.h"
#include <qpen.h> 
#include <math.h>
#include <qpainter.h>


// ---------------------------------------------------------------------------
//      Axis constructor
// ---------------------------------------------------------------------------

Axis::Axis(QCanvas* canvas, int x, int y, QString text,int l, int width, int nbOfTicks, double min, double max)
  :QCanvasRectangle(canvas){
        
  ticks  = nbOfTicks;   
  label  = text;
  startX = x;
  startY = y;
  length = l;
  minVal = min;                  
  maxVal = adjustValue(max);   
  stepLength = length/ticks;
  stepValue  = (maxVal - minVal)/ticks;
}
  
// ---------------------------------------------------------------------------
//      adjustValue
// ---------------------------------------------------------------------------
// Rounds up a value.

double Axis::adjustValue(double value){
  double i      = 1;
  double result = 0;
 
  if(value != 0){
    
    if(value >= 1){
      result = ceil(value);
    }
    
    else{        // for values less than one, for example 0.021
      for(value; value < 1; value = value * 10){
	i = i * 10;                // for 0.021 i = 100
      }
      result = ceil(value)/i;   //ceil(2.1)/100 = 0.03
    } 
    return result;
  }
}

// ---------------------------------------------------------------------------
//      rtti
// ---------------------------------------------------------------------------
// Returns a Run Time Type Identification value to make it possible to 
// distinguish between objects returned by QCanvas::at(). 

int Axis::rtti() const{
  return 2020;
}

// ---------------------------------------------------------------------------
//      getIndex
// ---------------------------------------------------------------------------
// Returns actual value per canvas unit.

double Axis::getIndex() const{
  return stepValue/stepLength;
}

// ---------------------------------------------------------------------------
//      getLength
// ---------------------------------------------------------------------------
// Retruns length of axis

double Axis::getLength() const{
  return length;
}

// ---------------------------------------------------------------------------
//      VerticalAxis constructor
// ---------------------------------------------------------------------------

VerticalAxis::VerticalAxis(QCanvas* canvas, int x, int y, QString text, int l, int width, int nbOfTicks, double min,double max,bool left)
  :Axis(canvas, x, y, text, l, width, nbOfTicks, min, max){
  
  setX(x - width/2);
  setY(y - length);
  setSize(width, length);
  
  if(left){         // axis places on the left side
    textX   = startX - 20;  // has text and numbers
    numberX = startX - 30;  // places different than
  }                         // axis on the right side.
  else{
    textX   = startX - label.length()/2.0; 
    numberX = startX + 5;
  }
}

// ---------------------------------------------------------------------------
//      drawShape
// ---------------------------------------------------------------------------
// Should be implemented by classes inheriting QCanvasItems. Draws the axis.

void VerticalAxis::drawShape(QPainter & painter){
  QFont f( "helvetica", 10);
  painter.setFont(f);
  painter.setPen(Qt::black);
  painter.drawLine(startX, startY, startX, startY - length); 
  painter.drawText(textX - label.length(), startY - length - 20, label);
        
  double number = minVal;
    
  int thicker = 0;
    
  for(double y = startY; y > startY - length - stepLength; y = y - stepLength){
    painter.drawLine(startX-2, y, startX+2, y); 
      
    if(thicker%10==0){
      painter.drawLine(startX - 5, y, startX + 2, y); 
      painter.drawText(numberX, y + stepLength, QString("%1 ").arg(number));
    }
    number = number + stepValue;
    thicker++;
  }
}


// ---------------------------------------------------------------------------
//      HorizontalAxis constructor
// ---------------------------------------------------------------------------

HorizontalAxis::HorizontalAxis(QCanvas* canvas, int x, int y, QString text,int l, int width, int nbOfTicks, double min, double max)
  :Axis(canvas, x, y, text, l, width, nbOfTicks, min, max){
    
  setX(x);
  setY(y - width/2);
  setSize(length, width);
}

// ---------------------------------------------------------------------------
//      drawShape
// ---------------------------------------------------------------------------
// Should be implemented by classes inheriting QCanvasItems. Draws the axis.

void HorizontalAxis::drawShape(QPainter & painter){
  QFont f( "helvetica", 10);
  painter.setFont(f);
  
  painter.drawLine(startX, startY, startX + length, startY);
  painter.drawText(startX + length, startY + 20, label);
  
  double number = minVal;
  int thicker = 0;
  
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












