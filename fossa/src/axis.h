#ifndef AXIS_H
#define AXIS_H

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
 * axis.h
 *
 * Definition of the Axis class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qcanvas.h>
#include <iostream>

class VerticalAxis;
class HorizontalAxis;
class PointWithText;


// ---------------------------------------------------------------------------
// class Axis
// Acts as a base class for HorizonatlAxis and VerticalAxis. 

class Axis:public QCanvasRectangle{
 public:
  
  Axis(QCanvas* canvas, int x, int y, QString text,int l, int width, int nbOfTicks, double min, double max);
  int rtti() const;
  double getIndex() const;
  double getLength() const;
  
 protected:
  double  ticks;
  QString label;
  double  minVal;
  double  maxVal;
  double  startX;
  double  startY;
  double  length;
  double  stepValue;
  double  stepLength;

 private:
  double adjustValue(double value);
};

// ---------------------------------------------------------------------------
// class VerticalAxis
// Allows for creation of a vertical axis on a canvas. 

class VerticalAxis:public Axis{
  
 public:  
  VerticalAxis(QCanvas* canvas, int x, int y, QString text,int l, int width,int nbOfTicks,double min,double max,bool left);
  
  void drawShape(QPainter & painter);
  
 private:
  double textX; 
  double numberX;
};


// ---------------------------------------------------------------------------
// class HorizontalAxis
// Allows for creation of a horisontal axis on a canvas. 

class HorizontalAxis:public Axis{
  
public:

  HorizontalAxis(QCanvas* canvas, int x, int y, QString text,int l, int width, int nbOfTicks, double min, double max);
  void drawShape(QPainter & painter);
 
};

#endif // AXIS_H






