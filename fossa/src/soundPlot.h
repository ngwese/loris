#ifndef SOUND_PLOT_H
#define SOUND_PLOT_H

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
 * soundPlot.h
 *
 * This all started when I noticed that Axis reimplements code that is used by
 * soundPixmap (formerly PartialsPixmap) to draw its axes. I decided that this
 * class should be used in both places. The problem is, the Pixmap likes to draw
 * things once, then save a bitmap of them while a QCanvasView likes to keep 
 * track of graphical objects in real time in case you want to dynamically redraw
 * them. Of course, Fossa redraws plots of partials from time to time, like when
 * you scale them or distill them etc. The problem is that QCanvasItems need a
 * QCanvasView, which is where soundPlot comes in. This is a view class with 2
 * axes and a pixmap for its graphical components.
 * 
 *
 * Chris Hinrichs 2/27/2006
 *
 *
 */
#include <qpixmap.h> 
#include <qcanvas.h> 
#include <qlist.h>

#include <Partial.h>
#include "axis.h"
#include "currentSoundView.h"
#include "soundPixmap.h"

#include <list>
#include <math.h> 

class Pixmap;

using namespace Loris;

// ---------------------------------------------------------------------------
// class SoundPlot
class SoundPlot:public QCanvasView{
  public:
    SoundPlot(
	QCanvas*		c,
	QWidget*		parent,
	char*			name,
	SoundList*		pList,
	Tab::TabType		t
    );

  bool				isEmpty();
  void				updatePixmap();
  void				setType(Tab::TabType t);
  QPixmap*			getPixmap();
  
  private:
    QCanvas*			canvas;
    SoundList*			soundList;
    SoundPixmap*		pixmap;
    Axis*			lAxis;
    Axis*			bAxis;
    Tab::TabType		type;
  
    int				leftMargin;
    int				rightMargin;
    int				topMargin;
    int				bottomMargin;
    QString			text;
};
#endif //SOUND_PLOT_H
