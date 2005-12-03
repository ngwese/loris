#ifndef MORPH_AREA_H
#define MORPH_AREA_H

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
 * morphArea.h
 *
 * Definition of the MorphArea class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include "BreakpointEnvelope.h"
#include "axis.h"
#include "partialsList.h"
#include "pointWithText.h"

#include <qcanvas.h>
#include <qsortedlist.h> 
#include <qlist.h> 

class QStatusBar;

class PartialsList;

class VerticalAxis;
class HorizontalAxis;
class PointWithText;

class FrequencyPoint;
class AmplitudePoint;
class NoisePoint;

// ---------------------------------------------------------------------------
// class MorphArea
// MorphArea inherits QCanvasView and lets a user insert and direct manipulate 
// amplitude, frequency, and noise breakpoints into the canvas and are visible
// on the QCanvasView. The  breakpoints represents the morphing function between 
// two sounds. The MorphArea has 4 states; amplitude, frequency, noise, and all three 
// states combined, which is the default state. When we are in a certain state, 
// for example amplitude, we can insert, remove, and dragg amplitude points.  

class MorphArea:public QCanvasView{ 
  
  Q_OBJECT

 public:
 
  MorphArea(QCanvas* canvas, QWidget* parent, char* name, PartialsList* partialsList, QStatusBar* statusbar);
  void contentsMousePressEvent(QMouseEvent* e);
  void contentsMouseMoveEvent(QMouseEvent* e);
  void contentsMouseReleaseEvent(QMouseEvent* e);
  void addPoint(int x, int y);
  int rtti() const;
  int toXAxisValue(int x);
  int toYAxisValue(int y);
  const QPoint getOrigo() const;
 
 public slots:
  void clearAll();
  void showHideClear(int buttonId);
  void morph();
  void setMorph1(QString& name);
  void setMorph2(QString& name);

 private:
  enum State {all, amplitude, frequency, noise};
  State state;
  VerticalAxis* lAxis;
  VerticalAxis* rAxis;
  HorizontalAxis* tAxis;
  QStatusBar* statusbar;
  QList<QCanvasItem> moving;
  PartialsList* partialsList;
 
  QString morph1;
  QString morph2;

  int leftMargin;
  int rightMargin;
  int topMargin;
  int bottomMargin;
  int width;
  int height;
  int newPointIndex;
  
  QSortedList<PointWithText> aList;
  QSortedList<PointWithText> fList;
  QSortedList<PointWithText> nList;

  void fillEnvelope(QSortedList<PointWithText>& list, BreakpointEnvelope& envelope);
  bool inArea(int, int);
  void showHideList(QSortedList<PointWithText>& list, bool show);
  void addpointHelp(QSortedList<PointWithText>& list, PointWithText* newPoint);
  void rightButtonHelp(QSortedList<PointWithText>& list, PointWithText* point);
  void moveHelp(QSortedList<PointWithText>& list, PointWithText* movingPoint, int x, int y);
  void setHorizontalAxis();
};
#endif // MORPH_AREA_H






















