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
#include <qsortedlist.h>

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
	Tab::TabType		t,
	int			sel
    );

    /*These exist so that they can be passed on to the parent DilateArea.*/
    void			contentsMousePressEvent(QMouseEvent* e);
    void			contentsMouseMoveEvent(QMouseEvent* e);
    void			contentsMouseReleaseEvent(QMouseEvent* e);


    double			toX(double value);
    double			toY(double value);
    void			plotPartials();
    void			hilight(int p);
    void			clearAll();
    void			clearHilighted();

    bool			isEmpty();
    void			updatePlot();
    void			setSelected(int sel);
    int				getSelected();
    void			setType(Tab::TabType t);
    Tab::TabType		getType();

    QPixmap*			getPixmap();


/*
  public signals:
    void			press();
    void			move();
    void			release();
*/

  public slots:
    void			rePlot();
  

  private:
    QCanvas*			canvas;
    QList<QCanvasItem>		highlines;
    SoundList*			soundList;
    SoundPixmap*		pixmap;
    Axis*			lAxis;
    Axis*			bAxis;
    Tab::TabType		type;

    int				selected;

    int				leftMargin;
    int				rightMargin;
    int				topMargin;
    int				bottomMargin;
    double			verticalIndex;
    double			horizontalIndex;
    QString			text;
};
#endif //SOUND_PLOT_H
