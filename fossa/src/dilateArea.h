#ifndef DILATE_AREA_H
#define DILATE_AREA_H

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
 * DilateArea.h
 *
 * Chris Hinrichs, 1/10/2006
 *
 */

#include "LinearEnvelope.h"
#include "axis.h"
#include "soundList.h"
#include "pointWithText.h"
#include "soundPlot.h"

#include <qcanvas.h>
#include <qlist.h>

#include <list>

using std::list;

class QStatusBar;

class SoundList;
class Axis;

//Use this class instead of QCanvasLine for breakpoints.
class DilatePoint:public QCanvasLine{
  public:
    DilatePoint(QCanvas* canvas, int x, int height, int bottomMargin);
    int rtti() const;
    const static int rttiNr = 2003;
};


/*
--------------------------------------------------------------------------------
class DilateArea
DilateArea, like MorphArea, inherits from QCanvasView and lets a user choose
which points to match between 2 sounds for dilation.
*/
class DilateArea:public QCanvasView{
  Q_OBJECT

  public:
    DilateArea(
	QCanvas*	canvas,
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	statusbar,
	int		w
    );

    ~DilateArea();

    void		contentsMousePressEvent(QMouseEvent* e);
    void		contentsMouseMoveEvent(QMouseEvent* e);
    void		contentsMouseReleaseEvent(QMouseEvent* e);
    void		addBreakPoint(int x, int y);
    int			rtti() const;
    int			toXAxisValue(int x);
    int			toYAxisValue(int y);

    list<double>*	getTimes();

    void		resetAxis(double max);
    void		updatePlot();

  public slots:
    void		dilate();
    void		setSound(QString& name, int pos);

  private:
    SoundList*		soundList;
    SoundPlot*		dilatePlot;


    //Identifiers for the sound to be plotted.
    QString             sound;
    int			dilateIndex;
    int			which;		//Which of the 2 DilateAreas is this one?

    //Dimensions
    int                 leftMargin;
    int                 rightMargin;
    int                 topMargin;
    int                 bottomMargin;
    int                 width;
    int                 height;

    //List of all Breakpoints and a sublist of moving points.
    std::list<QCanvasItem*>  dilateList;
    std::list<QCanvasItem*>  moving;

    //Helpers
    bool	inArea(int, int);
    void	fillEnvelope(
			std::list<DilatePoint*>	list,
			LinearEnvelope&			envelope
    );

    //For future use.
    QStatusBar*		statusbar;
};

#endif // DILATE_AREA_H
