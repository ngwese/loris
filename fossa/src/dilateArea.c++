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
 * dilateArea.c++
 *
 * 
 * Chris Hinrichs
*/

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "dilateArea.h"
using namespace Loris;

/*
--------------------------------------------------------------------------------
	DilateArea construtor
--------------------------------------------------------------------------------
*/
DilateArea::DilateArea(
	QCanvas*	canvas,
	QWidget*	parent,
	char*		name,
	PartialsList*	pList,
	QStatusBar*	status
):QCanvasView(canvas, parent, name){
  statusbar = status;
  partialsList = pList;

  newPointIndex = 5;
  leftMargin = 30;
  rightMargin = 30;
  topMargin = 30;
  bottomMargin = 20;

  width = canvas->width();
  height = canvas->height();

  lAxis = new VerticalAxis(
	canvas,
	leftMargin,
	height - bottomMargin,
	"Amplitude",
	height - bottomMargin - topMargin,
	30,
	100,
	0,
	100,
	true
  );

  rAxis = new VerticalAxis(
	canvas,
	width - rightMargin,
	height - bottomMargin,
	"Amplitude",
	height - bottomMargin - topMargin,
	30,
	100,
	0,
	100,
	false
  );

  
  updateTimeAxis();

};


/*
--------------------------------------------------------------------------------
	updateTimeAxis
--------------------------------------------------------------------------------
Instead of just having one axis over the life of the DilateArea, the time axis
might have to grow or shrink depending on which sample is shown.
*/
void MorphArea::updateTimeAxis(){
  int time = 0;
  int time1 = partialsList->at(dilate1Index)->getDuration();
  int time2 = partialsList->at(dilate2Index)->getDuration();

  if(time1 > ime2){
    time = time1;
  }else{
    time = time2;
  }

  if(tAxis){
    delete tAxis;
  }

  tAxis = new HorizontalAxis(
	canvas,
	leftMargin,
	height - bottomMargin,
	"time",
	width - rightMargin - leftMargin,
	30,
	100,
	0,
	time
  );

  tAxis->show();
  canvas->update();
}
/*
--------------------------------------------------------------------------------
	setSample1
--------------------------------------------------------------------------------
Tells the DilateArea to update itself to the first sample.
*/
void DilateArea::setSample1(QString& name){
  sample1 = name;

  if(!dilateList.isEmpty()){
    for(point = dilateList.first();
	point != 0;
	point = dilateList.next()
    ){
      point->setSampe1(name);
    }
  }

  updateTimeAxis();
}

/*
--------------------------------------------------------------------------------
	setSample2
--------------------------------------------------------------------------------
Tells the DilateArea to update itself to the first sample.
*/
void DilateArea::setSample2(QString& name){}


