#ifndef PARTIALS_PIXMAP_H
#define PARTIALS_PIXMAP_H

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
 * partialsPixmap.h
 *
 * Definition of the PartialsPixmap class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */
#include <qpixmap.h> 
#include <qpainter.h> 

#include <Partial.h>

#include <list>
#include <math.h> 

class AmplitudePixmap;
class FrequencyPixmap;
class NoisePixmap;
class EmptyPixmap;
class Axis;

using namespace Loris;

// ---------------------------------------------------------------------------
// class PartialsPixmap
// Base class for AmplitudePixmap, FrequencyPixmap, and NoisePixmap classes.
// Provides an area with x and y axis  where breakpoints of a collection of partials
// can get plotted over time. Subclasses have to implement plotPartials().

class PartialsPixmap:public QPixmap{
  
 public:
  PartialsPixmap(list<Loris::Partial>* p, double x, double y);
  
 private:
  double verticalIndex;
  double horizontalIndex;
  double adjustValue(double value);
  
 protected:
  double maxTime;
  double maxY;
  int leftMargin;
  int rightMargin;
  int topMargin;
  int bottomMargin;
  QString text;
  list<Loris::Partial>* partialList;

  bool inArea(int x, int y);
  double toX(double time);
  double toY(double value);
  virtual void plotPartials() = 0;
  void addAxis(double startX, double startY, QString label, double length, int width, double ticks, double minVal, double maxVal, bool vertical);
};

// ---------------------------------------------------------------------------
// class AmplitudePixmap
// Inherits PartialsPixmap.

class AmplitudePixmap:public PartialsPixmap{
  
public:
   AmplitudePixmap(list<Loris::Partial>* p, double x, double y);
   void plotPartials();
};

// ---------------------------------------------------------------------------
// class FrequencyPixmap
// Inherits PartialsPixmap.

class FrequencyPixmap:public PartialsPixmap{
  
public:
  FrequencyPixmap(list<Loris::Partial>* p, double x, double y);
  void plotPartials();
};

// ---------------------------------------------------------------------------
// class NoisePixmap
// Inherits PartialsPixmap.

class NoisePixmap:public PartialsPixmap{
  
public:
  NoisePixmap(list<Loris::Partial>* p, double x, double y);
  void plotPartials();
};

// ---------------------------------------------------------------------------
// class EmptyPixmap
// Is useful when partialsList is empty and we want to display a pixmap that is 
// empty. Inherits ParialsPixmap.

class EmptyPixmap:public PartialsPixmap{
  
public:
  
  EmptyPixmap(list<Loris::Partial>* p, double x, double y);
  void plotPartials();
};


#endif //PARTIALS_PIXMAP_H







