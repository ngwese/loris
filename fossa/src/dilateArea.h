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
	PartialsList*	partialsList,
	QStatusBar*	statusbar
    );
    void		contentsMousePressEvent(QMouseEvent* e);
    void		contentsMouseMoveEvent(QMouseEvent* e);
    void		contentsMouseReleaseEvent(QMouseEvent* e);
    void		addPoint(int x, int y);
    int			rtti() const;
    int			toXAxisValue(int x);
    int			toYAxisValue(int y);
    const QPoint	getOrigo() const;

  public slots:
    void		dilate();
    void		setDilate1(QString& name);
    void		setDilate2(QString& name);

  private:
    VerticalAxis*       lAxis;
    VerticalAxis*       rAxis;
    HorizontalAxis*     tAxis;
    QStatusBar*         statusbar;
    QList<QCanvasItem>  moving;
    PartialsList*       partialsList;

    QString             sample1;
    QString             sample2;
    int			dilate1Index;
    int			dilate2Index;

    int                 leftMargin;
    int                 rightMargin;
    int                 topMargin;
    int                 bottomMargin;
    int                 width;
    int                 height;
    int                 newPointIndex;

    QSortedList<PointWithText>  dilateList;

    bool	inArea(int, int);

    void	fillEnvelope(
			QSortedList<PointWithText>&	list,
			LinearEnvelope&			envelope
    );

    void	setHorizontalAxis();
}

#endif // DILATE_AREA_H
