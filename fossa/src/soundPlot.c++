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

  hilighted	= -1;

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
    this->updatePlot();
  }

  setBackgroundMode(Qt::FixedPixmap);
}

// ---------------------------------------------------------------------------
//	getType
// ---------------------------------------------------------------------------
Tab::TabType SoundPlot::getType(){
  return type;
}

// ---------------------------------------------------------------------------
//	setType
// ---------------------------------------------------------------------------
// If the type changes, then redraw the plot. As of 4/9/06, this is the
// only entry point to updatePlot...
void SoundPlot::setType(Tab::TabType t){
  type = t;
  updatePlot();
}

// ---------------------------------------------------------------------------
//	getPixmap
// ---------------------------------------------------------------------------
// return the SountPixmap object.
QPixmap* SoundPlot::getPixmap(){
  return pixmap;
}

// ---------------------------------------------------------------------------
//	clearAll
// ---------------------------------------------------------------------------
// Clear all QCanvasItems from the canvas.
void SoundPlot::clearAll(){
  QCanvasItemList list = canvas->allItems();
  QCanvasItemList::Iterator  it = list.begin();
  for (; it != list.end(); ++it) {
    if ( *it )
      delete *it;
  }
}

// ---------------------------------------------------------------------------
//	clearHilighted
// ---------------------------------------------------------------------------
// Redraw the previously hilighted partial with regular lines.
void SoundPlot::clearHilighted(){
  QCanvasItemList list = canvas->allItems();
  QCanvasItemList::Iterator it = list.at(hilighted);

  if(*it ) delete *it;
}

// ---------------------------------------------------------------------------
//	hilight
// ---------------------------------------------------------------------------
// Use the QSlider to select a partial to hilight.
void SoundPlot::hilight(int p){
  std::list<Loris::Partial>*  partialList = soundList->getCurrentPartials();
  QCanvasLine* hLine;

  list<Loris::Partial>::const_iterator it;
  Partial_ConstIterator pIt;
  int i;

  double x, y;
  double lastX, lastY;

//  if( hilighted >= 0 ) clearHilighted();

  if( p >= 0 && type != Tab::empty && ! soundList->isEmpty() ){
    QPen ampLinePen(Qt::red);
    QPen freqLinePen(Qt::green);
    QPen bwLinePen(Qt::blue);

    /*Have to manually advance the iterator because std::list does not support at()
     * like QList does.*/
    it = partialList->begin();
    for(i=0; i<p; i++)it++;
    pIt = it->begin();

    switch( type ){
      case Tab::amplitude:
        lastX = toX(pIt.time());
        lastY = toY(pIt->amplitude());

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->amplitude());

          hLine = new QCanvasLine(canvas);
          hLine->setPen(ampLinePen);
          hLine->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
          hLine->show();

          lastX = x;
          lastY = y;
        }
        break;

      case Tab::frequency:
        lastX = toX(pIt.time());
        lastY = toY(pIt->frequency());

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->frequency());

          hLine = new QCanvasLine(canvas);
          hLine->setPen(freqLinePen);
          hLine->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
          hLine->show();

          lastX = x;
          lastY = y;
        }
        break;

      case Tab::noise:
        lastX = toX(pIt.time());
        lastY = toY(pIt->bandwidth());

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->bandwidth());

          hLine = new QCanvasLine(canvas);
          hLine->setPen(bwLinePen);
          hLine->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
          hLine->show();

          lastX = x;
          lastY = y;
        }
        break;

    }

    canvas->update();
  }
  
}

// ---------------------------------------------------------------------------
//	updatePlot
// ---------------------------------------------------------------------------
// Create a pixmap, have it plot its partials, and save it for later.
void SoundPlot::updatePlot(){
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
    clearAll();
    canvas->update();
    return;
  }

  /*Update the SoundPixmap.*/
/*
  pixmap = new SoundPixmap(
        soundList->getCurrentPartials(),
        soundList->getCurrentDuration(),
	soundList->getCurrentMax((Sound::ValType)type),
        this->type
  );
  canvas->setBackgroundPixmap( *pixmap );
*/

  double maxX = soundList->getCurrentDuration();
  double maxY = soundList->getCurrentMax((Sound::ValType)type);

  horizontalIndex = maxX / (width() - leftMargin - rightMargin);
  verticalIndex = maxY / (height() - topMargin - bottomMargin);

  clearAll();
  plotPartials();

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
//      toX
// ---------------------------------------------------------------------------
// Translates an actual time value into the corresponding value on the pixmap
// into a pixel coordinate.
double SoundPlot::toX(double time){
  return (time / horizontalIndex)  + (double)leftMargin;
}

// ---------------------------------------------------------------------------
//      toY
// ---------------------------------------------------------------------------
// Translates an actual y value into the corresponding value on the pixmap
// into a pixel coordinate.
double SoundPlot::toY(double value){
  return height() - (value / verticalIndex) - (double)bottomMargin;
}

// ---------------------------------------------------------------------------
//	plotPartials
// ---------------------------------------------------------------------------
// Do the work of plotting the partials as QCanvasRectangles with lines 
// between them.
void SoundPlot::plotPartials(){
  std::list<Loris::Partial>*
                        partialList = soundList->getCurrentPartials();

  list<Loris::Partial>::const_iterator it;
  Partial_ConstIterator pIt;

  QCanvasRectangle* rect;
  QCanvasLine* line;
  QPen ampPen(Qt::darkRed);
  QPen ampLinePen(Qt::red);

  QPen freqPen(Qt::darkGreen);
  QPen freqLinePen(Qt::green);

  QPen bwPen(Qt::darkBlue);
  QPen bwLinePen(Qt::blue);

  double x;
  double y;
  double lastX;
  double lastY;

  if( type == Tab::empty ) return;

  // loop through all partials in the list
  for( it = partialList->begin(); it != partialList->end(); it++){
    pIt = it->begin();
    //As an optimization, do the test for type outside of the loop, and hard-
    //code each of the three loops to plot amp/freq/bandw so that the test
    //is not done for each breakpoint, only for each partial.
    //The best way to do this is really with a function pointer...
    switch( type ){
      case Tab::amplitude:
        //Draw the first Breakpoint, then loop through the rest, drawing lines
        //between them.
        x = toX(pIt.time());
        y = toY(pIt->amplitude());
        lastX = x;
        lastY = y;
        rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
        rect->setPen(ampPen);
        rect->show();

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->amplitude());
          rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
          rect->setPen(ampPen);
          rect->show();

          // draw a read line connecting breakpoints
          if(! soundList->isCurrentDistilled() ){
            line = new QCanvasLine(canvas);
            line->setPen(ampLinePen);
            line->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
            line->show();
          }

          lastX = x;
          lastY = y;
        }
        break;

      case Tab::frequency:
        //Draw the first Breakpoint, then loop through the rest, drawing lines
        //between them.
        x = toX(pIt.time());
        y = toY(pIt->frequency());
        lastX = x;
        lastY = y;
        rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
        rect->setPen(freqPen);
        rect->show();

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->frequency());
          rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
          rect->setPen(freqPen);
          rect->show();

          // draw a read line connecting breakpoints
          if(! soundList->isCurrentDistilled() ){
            line = new QCanvasLine(canvas);
            line->setPen(freqLinePen);
            line->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
            line->show();
          }

          lastX = x;
          lastY = y;
        }
        break;

      case Tab::noise:
        //Draw the first Breakpoint, then loop through the rest, drawing lines
        //between them.
        x = toX(pIt.time());
        y = toY(pIt->bandwidth());
        lastX = x;
        lastY = y;
        rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
        rect->setPen(bwPen);
        rect->show();

        for(pIt++; pIt != it->end(); pIt++){
          x = toX(pIt.time());
          y = toY(pIt->bandwidth());
          rect = new QCanvasRectangle((int)x, (int)y, 2, 2, canvas);
          rect->setPen(bwPen);
          rect->show();

          // draw a read line connecting breakpoints
          if(! soundList->isCurrentDistilled() ){
            line = new QCanvasLine(canvas);
            line->setPen(bwLinePen);
            line->setPoints((int)lastX, (int)lastY, (int)x, (int)y);
            line->show();
          }

          lastX = x;
          lastY = y;
        }
        break;

    }
  }
}

// ---------------------------------------------------------------------------
//	isEmpty
// ---------------------------------------------------------------------------
bool SoundPlot::isEmpty(){
  return type == Tab::empty;
}