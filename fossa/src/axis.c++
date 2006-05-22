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
 * axis.c++
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "axis.h"
#include <qpen.h> 
#include <math.h>
#include <qcanvas.h>
#include <qpainter.h>


/*
---------------------------------------------------------------------------
	Axis constructor
---------------------------------------------------------------------------
*/
Axis::Axis(
	QCanvas*	canvas,
	int		x,
	int		y,
	QString		text,
	int		l,
	int		width,
	int		nbOfTicks,
	double		min,
	double		max,
	bool		VH,
	bool		LR
):QCanvasRectangle(canvas){
  ticks		= nbOfTicks;
  label		= text;
  startX	= x;
  startY	= y;
  length	= l;
  minVal	= min;
  maxVal	= max; //adjustValue(max);
  stepLength	= length/ticks;		//Value in terms of unit pixel.
  stepValue	= double((maxVal - minVal)/ticks);
  vertical	= VH;
  left		= LR;

  if( vertical ){
    setX(x - width/2);
    setY(y - length);
    setSize(width, length);
  
    textX   = startX - 20;

    if(left) numberX = maxVal > 10 ? startX - 45 : startX - 40;
    else numberX = startX + 5;

  } else {
    setX(x);
    setY(y - width/2);
    setSize(length, width);
  }
}
  
/*
---------------------------------------------------------------------------
	adjustValue
---------------------------------------------------------------------------
Rounds up a value to the next largest order of magnitude.
*/
double Axis::adjustValue(double value){
  double i      = 1;
  double result = 0;
 
  if(value != 0){
    if(value >= 1){
      result = ceil(value);
    }else{
      for(value; value < 1; value = value * 10){
        // for 0.021 i = 100
	i = i * 10;
      }

      result = ceil(value)/i;
    } 

    return result;
  }
}

/*
---------------------------------------------------------------------------
	rtti
---------------------------------------------------------------------------
Returns a Run Time Type Identification value to make it possible to 
distinguish between objects returned by QCanvas::at(). 
*/
int Axis::rtti() const{
  return 2020;
}

/*
---------------------------------------------------------------------------
	getIndex
---------------------------------------------------------------------------
Returns actual value per canvas unit.
*/
double Axis::getIndex() const{
  return stepValue/stepLength;
}

/* ---------------------------------------------------------------------------
	getLength
---------------------------------------------------------------------------
Retruns length of axis
*/
double Axis::getLength() const{
  return length;
}

/*
---------------------------------------------------------------------------
	drawShape
---------------------------------------------------------------------------
Should be implemented by classes inheriting QCanvasItems. Draws the axis.

The way this works is that you call axis->show() which is
inherited from (probably) QCanvasItem which then calls drawShape; Fossa
does not call this directly, it just implements it as a call-back.
*/
void Axis::drawShape(QPainter & painter){
  QFont f( "helvetica", 10);
  QString numberText;
  painter.setFont(f);
  painter.setPen(Qt::black);

  double number = minVal;
  int thicker = 0;
  int x, y;
  double xVal, yVal;

  if( vertical ){
    painter.drawLine(startX, startY, startX, startY - length); 
    painter.drawText(textX - label.length(), startY - length - 20, label);

    for(
	yVal = minVal;
	yVal <= maxVal;
	yVal += stepValue
    ){
      y = startY - (int)((yVal/(maxVal-minVal)) * length);
      painter.drawLine(startX-2, y, startX+2, y); 
      
      /*Every 10 ticks draw a long tick with a number by it.*/
      if(thicker%10 == 0){
        //Helps make sure tick vals are shown to the right # of sig figs. -Chris H.
        if( number < 10 ) numberText = QString("%1").arg(number, 5, 'f', 3);
        else numberText = QString("%1").arg(number, 5, 'g', 5);

        painter.drawLine(startX - 5, y, startX + 2, y); 
        painter.drawText( numberX, y + stepLength, numberText );
      }

      number = number + stepValue;
      thicker++;
    }
  } else {
    painter.drawLine(startX, startY, startX + length, startY);
    painter.drawText(startX + length - 40, startY + 20, label);

    for(
	xVal = minVal;
	xVal <= maxVal;
	xVal += stepValue
    ){
      x = startX + (int)((xVal/(maxVal-minVal)) * length);
      painter.drawLine(x, startY-2, x, startY+2); 

      /*Every 10 ticks draw a long tick with a number by it.*/
      if(thicker%10==0){
        painter.drawLine(x, startY - 2, x, startY + 5); 
        painter.drawText( x-8, startY+12, QString("%1").arg(number, 3, 'f', 3) );
      }

      number += stepValue;
      thicker++;
    }
  }

}
