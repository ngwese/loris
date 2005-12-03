#ifndef POINT_WITH_TEXT_H
#define POINT_WITH_TEXT_H

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
 * pointWithText.h
 *
 * Definition of the PointWithText class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

// ---------------------------------------------------------------------------
// class PointWithText
//
// A PointWithText consists of a line, percent and time text, and a drawn rectangle, 
// which from now on is referred to as a point. The class inherits QCanvasRectangle,
// and it can be placed in a canvas, shown to users by a QCanvasView. The 
// QCanvasRectangle itself is invisible, what is shown is the texts, line and 
// the point which is placed inside the invisible rectangle.  A PointWithText is 
// inserted to the canvas everytime the user clicks on an empty spot in the MorphArea 
// and its components gets viewable. Points can be inserted between two points as 
// well after. If the user clicks on an already inserted PointWithText, the point 
// gets movable and the user can drag it, between its neighbours, on the MorphArea.
// A PointWithText can be erased by right clicking on it.

#include "morphArea.h"

class AmplitudePoint;
class FrequencyPoint;
class NoisePoint;
class MorphArea;
class PointWithText;

class PointWithText:public QCanvasRectangle{ 
public:

  const static int size = 4; 
  PointWithText(MorphArea* morphArea, int ix, int iy, QCanvas* canvas, int id, QString& morph1, QString& morph2);
  ~PointWithText();
  virtual void show() = 0;
  virtual void hide() = 0;
  virtual int rtti() const = 0;
  virtual void moveLeftLine(int lastX, int lastY) = 0;
  virtual void setLeftLine(int toX, int toY) = 0; 
  int x() const;
  int y() const;
  void move(int x, int y);
  void drawShape(QPainter & painter);
  void setGui();
  void setPointText();
  void setMorph1(QString& name);
  void setMorph2(QString& name);
  int operator==(PointWithText& right);
  int operator!=(PointWithText& right);
  int operator<(PointWithText& right);
  int operator>(PointWithText& right);

protected:
  QCanvasText* percentLabel; 
  QCanvasText* timeLabel;
  QCanvasLine* lineLeft;
  MorphArea* m;
  QString partial1;
  QString partial2;
}; 

// ---------------------------------------------------------------------------
// class AmplitudePoint
//
// Inherits PointWithText and represents an amplitude point on the canvas.

class AmplitudePoint:public PointWithText{

public:
  const static int rttiNr = 2000;  
  QPen pen;
  AmplitudePoint(MorphArea* m, int ix, int iy, QCanvas* canvas, int newPointIndex, QString& morph1,QString& morph2);
  int rtti() const;
  void hide();  
  void show();
  void setLeftLine(int toX, int toY);
  void moveLeftLine(int toX, int toY);
};

// ---------------------------------------------------------------------------
// class FrequencyPoint
//
// Inherits PointWithText and represents a frequency point on the canvas.

class FrequencyPoint:public PointWithText{

public:
  QPen pen;
  const static int rttiNr = 2001;
  FrequencyPoint(MorphArea* m, int ix, int iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2);
  int rtti() const;
  void hide();
  void show();  
  void setLeftLine(int toX, int toY);
  void moveLeftLine(int toX, int toY);
};

// ---------------------------------------------------------------------------
// class NoisePoint
//
// Inherits PointWithText and represents a noise point on the canvas.

class NoisePoint:public PointWithText{

public:
  const static int rttiNr = 2002;
  QPen pen;

  NoisePoint(MorphArea* m, int ix, int iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2 );
  int rtti() const;
  void hide();
  void show();
  void setLeftLine(int toX, int toY);
  void moveLeftLine(int toX, int toY);
};

#endif // POINT_WITH_TEXT_H
















