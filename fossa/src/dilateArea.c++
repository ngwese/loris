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
 * The DilateArea is a QCanvasView that shows the plot of a sound, and lets the
 * user set and move breakpoints by clicking in it.
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
#include "currentSoundView.h"
#include <qcanvas.h>

using namespace Loris;

DilatePoint::DilatePoint(
	QCanvas* canvas,
	int x,
	int height,
	int bottomMargin
):QCanvasLine(canvas){
  setPoints(0, height, 0, bottomMargin);
  move(x, 0);
}

int DilatePoint::rtti() const {return 2003;}

/*
--------------------------------------------------------------------------------
	DilateArea construtor
--------------------------------------------------------------------------------
It might be a bit confusing that DilateArea IS A QCanvasView and yet also 
HAS A SoundPlot which IS also A QCanvasView. This works because more than one
QCanvasView can view the same QCanvas; SoundPlot can clutter it up with points
and lines while DilateArea manages the BreakPoints. Or at least it's supposed to.
*/
DilateArea::DilateArea(
	QCanvas*	canvas,
	QWidget*	parent,
	char*		name,
	SoundList*	pList,
	QStatusBar*	status,
	int		w
):QCanvasView(canvas, parent, name){
  which = w;
  statusbar = status;
  soundList = pList;

  leftMargin = 30;
  rightMargin = 0;
  topMargin = 10;
  bottomMargin = 20;
  plotScale = 1.0;

  width = canvas->width();
  height = canvas->height();

  dilatePlot = new SoundPlot(
    canvas,
    this,
    "dilatePlot",
    pList,
    Tab::empty,
    -1
  );

  dilatePlot->setMinimumSize(QSize(canvas->width() + 4, canvas->height() + 4));
  dilatePlot->setMaximumSize(QSize(canvas->width() + 4, canvas->height() + 4));
  dilatePlot->setBackgroundColor("white");
  dilatePlot->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)0,
                (QSizePolicy::SizeType)0,
                dilatePlot->sizePolicy().hasHeightForWidth()
        )
  );

  dilatePlot->hide();

}


/*
--------------------------------------------------------------------------------
	~DilateArea
--------------------------------------------------------------------------------
*/
DilateArea::~DilateArea(){
}


/*
--------------------------------------------------------------------------------
	resetAxis
--------------------------------------------------------------------------------
Wrapper funtion to tell the SoundPlot to reset its axis.
*/
void DilateArea::resetAxis(double max){
  plotScale = soundList->getSound(dilateIndex)->getDuration() / max;

  dilatePlot->resetAxis(max); 
}

/*
--------------------------------------------------------------------------------
	updatePlot
--------------------------------------------------------------------------------
Wrapper function to tell the SoundPlot to update itself.
*/
void DilateArea::updatePlot(){dilatePlot->updatePlot();}


/*
--------------------------------------------------------------------------------
	setSound
--------------------------------------------------------------------------------
Tells the DilateArea to update itself to the specified sound. 
*/
void DilateArea::setSound(QString& name, int pos){
  sound = name;

  dilateIndex = pos;

  //Next update the soundPlot.
  dilatePlot->setSelected(pos);
  dilatePlot->setType(Tab::amplitude);	//This automatically updates the plot.
}

/*
--------------------------------------------------------------------------------
	getTimes
--------------------------------------------------------------------------------
Return a list of times where the user has placed markers.
*/
list<double>* DilateArea::getTimes(){
  list<double>* newList = new list<double>;
  list<QCanvasItem*>::iterator it;
  double time; 

  double factor = soundList->getSound(dilateIndex)->getDuration();
  factor /= (width - leftMargin - rightMargin);


  for(	it = dilateList.begin();
	it != dilateList.end();
	it++
  ){
    time = (*it)->x();
    time *= factor;
    time /= plotScale;
    newList->push_front( time );
  }

  newList->sort();

  return newList;
}

/*
--------------------------------------------------------------------------------
	addBreakPoint
--------------------------------------------------------------------------------
Create a BreakPoint for matching the 2 sounds against each other.
*/
void DilateArea::addBreakPoint(int x, int y){
  DilatePoint* newPoint = new DilatePoint(
	canvas(),
	x,
	height-topMargin,
	bottomMargin
  );

  dilateList.push_front(newPoint);
  moving.push_front(newPoint);
  newPoint->show();
}

/*
--------------------------------------------------------------------------------
	inArea
--------------------------------------------------------------------------------
*/
bool DilateArea::inArea(int x, int y){
  bool inX = x+1 >= leftMargin && x <= width-rightMargin;
  bool inY = y+1 >= topMargin  && y <= height-bottomMargin;
  return inX && inY;
}


/*
--------------------------------------------------------------------------------
	contentsMousePressEvent
--------------------------------------------------------------------------------
*/
void DilateArea::contentsMousePressEvent(QMouseEvent* e){
  int found = 0;
  QCanvasItemList::iterator it;
  ButtonState buttonState;
  QCanvasItemList allItemsHit;

  if( inArea(e->x(), e->y()) ){
    buttonState = e->button();

    //Get all items colliding with a vertical line where the click occurred.
    allItemsHit = canvas()->collisions(
	QRect( (e->x())-1, topMargin, 3, height )
    );
    moving.clear();

    //If the user left-clicked on a point, drag it around with the mouse.
    //If the user right-clicked on a point, remove it.
    if (!allItemsHit.isEmpty()){
      for(it = allItemsHit.begin();
          it != allItemsHit.end();
          it++)
        if((*it)->rtti() == DilatePoint::rttiNr)
          switch(buttonState){
            case LeftButton:
              moving.push_front( *it );
              found++;
              break;

            case RightButton:
              delete *it;
              dilateList.remove( *it );
              break;
          }

      //There could have been collisions with datapoints, but not with any lines.
      if(!found && buttonState == LeftButton) addBreakPoint(e->x(), e->y());
    } else if(buttonState == LeftButton){
      //If there is no point there, create one.
      addBreakPoint(e->x(), e->y());
    }

    canvas()->update();
  }
}

/*
--------------------------------------------------------------------------------
	contentsMouseMoveEvent
--------------------------------------------------------------------------------
*/
void DilateArea::contentsMouseMoveEvent(QMouseEvent* e){
  std::list<QCanvasItem*>::iterator it;

  //Typically there should only be one moving item, but there may be more...
  if(inArea(e->x(), e->y()))
    for(it = moving.begin();
        it != moving.end();
        it++
    )
      (*it)->move(e->x(), 0);

  canvas()->update();
}

/*
--------------------------------------------------------------------------------
	contentsMouseReleaseEvent
--------------------------------------------------------------------------------
*/
void DilateArea::contentsMouseReleaseEvent(QMouseEvent* e){
  moving.clear();
}

/*
--------------------------------------------------------------------------------
	dilate
--------------------------------------------------------------------------------
*/
void DilateArea::dilate(){


}
