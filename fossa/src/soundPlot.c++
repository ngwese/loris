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
 * soundPlot.c++
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

 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "soundList.h"
#include "axis.h"
#include "soundPlot.h"
#include "soundPixmap.h"
#include "currentSoundView.h"

using std::list;

// ---------------------------------------------------------------------------
//      SoundPlot constructor
// ---------------------------------------------------------------------------
// Takes a of sound to be plotted over time, and the maximum x and y value
SoundPlot::SoundPlot(
	QCanvas*		c,
	QWidget*		parent,
	char*			name,
	SoundList*		pList,
	Tab::TabType		t
):QCanvasView(c, parent, name){
  canvas	= c;
  soundList	= pList;
  type		= t;

  /*For some reason, without this delete is called on NULL resulting in SIGSEGV.*/
  pixmap	= 0;
  lAxis		= 0;
  bAxis		= 0;

  leftMargin	= 45;
  rightMargin	= 10;
  topMargin	= 10;
  bottomMargin	= 30;

  switch( type ){
    case Tab::amplitude:
      text = "amplitude";
      break;

    case Tab::frequency:
      text = "frequency";
      break;

    case Tab::noise:
      text = "noise";
      break;

    default:
      type = Tab::empty;
    case Tab::empty:
      text = "";
      break;

  }

  //On startup there has to be an empty Plot. Just in case, though try updating.
  if( type != Tab::empty ){
    this->updatePixmap();
  }

  setBackgroundMode(Qt::FixedPixmap);
}

// ---------------------------------------------------------------------------
//	setType
// ---------------------------------------------------------------------------
void SoundPlot::setType(Tab::TabType t){
  type = t;
}

QPixmap* SoundPlot::getPixmap(){
  return pixmap;
}

// ---------------------------------------------------------------------------
//	updatePixmap
// ---------------------------------------------------------------------------
// Create a pixmap, have it plot its partials, and save it for later.
void SoundPlot::updatePixmap(){
  /*Start off by getting rid of the graphical items.*/
  if(lAxis){
    delete lAxis;
    lAxis = 0;
  }
  if(bAxis){
    delete bAxis;
    bAxis = 0;
  }
  if( pixmap ){
    delete pixmap;
    pixmap = 0;
  }

  if( type == Tab::empty ){
    canvas->update();
    return;
  }

  /*Update the SoundPixmap.*/
  pixmap = new SoundPixmap(
        soundList->getCurrentPartials(),
        soundList->getCurrentDuration(),
	soundList->getCurrentMax((Sound::ValType)type),
        this->type
  );

  canvas->setBackgroundPixmap( *pixmap );


  double maxX = soundList->getCurrentDuration();
  double maxY = soundList->getCurrentMax((Sound::ValType)type);

  //Left Axis
  lAxis = new Axis (
        canvas,
        leftMargin,
        height() - bottomMargin,
        text,
        height()-bottomMargin-topMargin,
        30,
        101,
        0.0,
        maxY,
        true,
        true
  );

  //Bottom Axis
  bAxis = new Axis (
        canvas,
        leftMargin,
        height() - bottomMargin,
        "time",
        width()-rightMargin-leftMargin,
        30,
        101,
        0.0,
        maxX,
        false,
        false
  );

  lAxis->show();
  bAxis->show();

  canvas->update();
}


// ---------------------------------------------------------------------------
//	isEmpty
// ---------------------------------------------------------------------------
bool SoundPlot::isEmpty(){
  return type == Tab::empty;
}
