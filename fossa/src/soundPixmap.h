#ifndef SOUND_PIXMAP_H
#define SOUND_PIXMAP_H

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
 * soundPixmap.h
 *
 * Definition of the SoundPixmap class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */
#include <qpixmap.h> 
#include <qpainter.h> 

#include <Partial.h>
#include "axis.h"
#include "currentSoundView.h"

#include <list>
#include <math.h> 

class Pixmap;

using namespace Loris;

// ---------------------------------------------------------------------------
// class SoundPixmap
// Provides an area with x and y axis where breakpoints of a collection of partials
// are plotted over time.
class SoundPixmap:public QPixmap{
  public:
    SoundPixmap(
	std::list<Loris::Partial>*	p,
	double				x,
	double				y,
	Tab::TabType			t
    );
  
  private:
    double		verticalIndex;
    double		horizontalIndex;
    double		adjustValue(double value);
    Tab::TabType	type;
  
  protected:
    double		maxTime;
    double		maxY;
    int			leftMargin;
    int			rightMargin;
    int			topMargin;
    int			bottomMargin;
    QString		text;
    std::list<Loris::Partial>*
			partialList;

    bool		inArea(int x, int y);
    int			toX(double time);
    int			toY(double value);
    void		plotPartials();
};


// ---------------------------------------------------------------------------
// class EmptyPixmap
// Is useful when soundList is empty and we want to display a pixmap that is 
// empty. Inherits ParialsPixmap.
class EmptyPixmap:public SoundPixmap{
  public:
    EmptyPixmap(
	std::list<Loris::Partial>*	p,
	double				x,
	double				y
    );
    void plotPartials();
};

#endif //SOUND_PIXMAP_H
