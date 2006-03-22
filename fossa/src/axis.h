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


// ---------------------------------------------------------------------------
// class Axis
// Acts as a base class for HorizonatlAxis and VerticalAxis. 
class Axis:public QCanvasRectangle{
  public:
    Axis(
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
    );

    int			rtti() const;
    double		getIndex() const;
    double		getLength() const;

    void		drawShape(QPainter & painter);
  
  protected:
    QString		label;
    int			ticks;
    int			startX;
    int			startY;
    int			length;
    int			stepLength;	//Value in terms of pixels.
    double		minVal;
    double		maxVal;
    double		stepValue;	//Value in terms of real domain f/a/n.

    int			textX;
    int			numberX;

  private:
    double		adjustValue(double value);
    bool		vertical;
    bool		left;
};

#endif // AXIS_H
