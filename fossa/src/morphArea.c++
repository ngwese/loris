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
 * partialsList.c++
 *
 * The PartialsList class is the main model of the application, ie. changes in 
 * PartialsList will trigger updates in view classes. This class has a list
 * containing imported partials and partials produced from different 
 * manipulations. One partials in the list is always set to current partials, 
 * and modifications can be made only to current partials. 
 * PartialsList also keeps track of two partials which are selected when 
 * the user decides to make a morph between two sounds. The class 
 * communicates with LorisInterface for necessary operations on partials.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "morphArea.h"
using namespace Loris;

// ---------------------------------------------------------------------------
//      MorphArea constructor
// ---------------------------------------------------------------------------

MorphArea::MorphArea(
	QCanvas* canvas, 
	QWidget* parent,
	char* name,  
	PartialsList* pList, 
	QStatusBar* status
	):QCanvasView(canvas,parent,name){

  statusbar = status;
  state = all;
  newPointIndex = 5;
  partialsList = pList;
  leftMargin = 30; rightMargin = 30; topMargin = 30; bottomMargin = 20;
  
  width  = canvas->width(); 
  height = canvas->height();
  
  lAxis = new VerticalAxis(canvas, leftMargin, height - bottomMargin, "Percent", 
		       height-bottomMargin-topMargin, 30, 100,  0, 100, true); 
  rAxis = new VerticalAxis(canvas, width-rightMargin, height - bottomMargin, "Percent", 
			height-bottomMargin-topMargin, 30, 100, 100, 0, false);
  tAxis = 0;
  setHorizontalAxis();

  // the points are saved in lists which sort the points by x-value
  // because the canvas only returns objects sorted by z-value.  
  aList.setAutoDelete(true); //qt deletes objects automatically
  fList.setAutoDelete(true);
  nList.setAutoDelete(true);
 
  lAxis->show();
  rAxis->show();
  tAxis->show();

  canvas->update();
}  

// ---------------------------------------------------------------------------
//      MorphArea contentsMousePressEvent
// ---------------------------------------------------------------------------
// Is called when the user clicks with the mouse in the area.

void MorphArea::contentsMousePressEvent(QMouseEvent* e){
  if (inArea(e->x(), e->y())){
    ButtonState buttonState = e->button();
    QCanvasItemList allItemsHit = canvas()->collisions(e->pos());
    moving.clear();

    switch(buttonState){

      // if a click is made with left mouse button the intension is to 
      // either add a new point or to move a point.
    case LeftButton:
      if (!allItemsHit.isEmpty()){ // the user clicked on an object in canvas.
	QCanvasItemList::Iterator it = allItemsHit.begin();

	switch(state){
	case all:
	  // Save all objects the user clicked on which are points.
	    for(it; it != allItemsHit.end(); it++){
	      if((*it)->rtti() == AmplitudePoint::rttiNr||
		 (*it)->rtti() == FrequencyPoint::rttiNr||
		 (*it)->rtti() == NoisePoint::rttiNr){
		moving.append(*it); //we need an array of moving items!
	      }
	    }
	    break;
	    
	case amplitude:
	  // Save the object the user clicked on if it is an amplitude point.
	  if((*it)->rtti() == AmplitudePoint::rttiNr)
	    moving.append(*it);
	  break;
	  // Save the object the user clicked on if it is a frequency point.
	case frequency: 
	  if((*it)->rtti() == FrequencyPoint::rttiNr)
	    moving.append(*it);
	  break;
	   // Save the object the user clicked on if it is a noise point.
	case noise:  
	  if((*it)->rtti() == NoisePoint::rttiNr)
	    moving.append(*it);
	  break;
	  
	default: std::cout<<"MorphArea::contentsMousePressEvent(QMouseEvent* e): have no state, weird"<<endl;
	}
      }
      
      if(moving.isEmpty()){
	// the user clicked in an empty area - add point.
	addPoint(e->x(), e->y()); 
      }
     
      break;
      
      // If a user clicks with right button on a point 
      // it should be deleted if the corresponding state is set 
    case RightButton:
      if (!allItemsHit.isEmpty()){
	QCanvasItemList::Iterator it = allItemsHit.begin();
	if((*it) != 0){
	  
	  switch(state){
	    
	  case all: 
	    while(it != allItemsHit.end()){
	      switch((*it)->rtti()){
	      case AmplitudePoint::rttiNr: 
		rightButtonHelp(aList, (AmplitudePoint*)(*it));
		allItemsHit = canvas()->collisions(e->pos());
		it = allItemsHit.begin();
		break;
	      case FrequencyPoint::rttiNr: 
		rightButtonHelp(fList, (FrequencyPoint*)(*it)); 
		allItemsHit = canvas()->collisions(e->pos());
		it = allItemsHit.begin();
		break;
	      case NoisePoint::rttiNr:
		rightButtonHelp(nList, (NoisePoint*)(*it));
		allItemsHit = canvas()->collisions(e->pos());
		it = allItemsHit.begin();
		break;
	      default: 
		// user clicked on a non-point object.
		it++; 
	      }
	    }
	    break;
	    
	  case amplitude: 
	    if((*it)->rtti()==AmplitudePoint::rttiNr)
	      rightButtonHelp(aList, (AmplitudePoint*)(*it));
	    break;
	    
	  case frequency: 
	    if((*it)->rtti()==FrequencyPoint::rttiNr)
	      rightButtonHelp(fList, (FrequencyPoint*)(*it));
	    break;
	    
	  case noise: 
	    if((*it)->rtti()==NoisePoint::rttiNr)
	      rightButtonHelp(nList, (NoisePoint*)(*it));
	    break;

	  default: std::cout<<"MorphArea::contentsMousePressEvent(): no state which is weird"<<endl;
	  }
	  canvas()->update();
	}
      }
    default: break;
    }
  }
}

// ---------------------------------------------------------------------------
//      rightButtonHelp
// ---------------------------------------------------------------------------
// deletes a point from the list and sets the left line of the point after the 
// deleted point.

void MorphArea::rightButtonHelp(QSortedList<PointWithText>& list, PointWithText* point){
  if(list.remove(point)){
    PointWithText* current = list.current();
    PointWithText* prev    = list.prev();
    
    if(list.count()>0){  
      if(current != list.first()){
	current->setLeftLine(prev->x(), prev->y()); //set left line
      }
      else{
	current->setLeftLine(getOrigo().x(), getOrigo().y());
      }
    }
  }
}

// ---------------------------------------------------------------------------
//      contentsMouseMoveEvent
// ---------------------------------------------------------------------------
// Is called when the user dragg the mouse pointer in the area, causes all items
// in moving to move.

void MorphArea::contentsMouseMoveEvent(QMouseEvent* e){
  // if left button clicked on one or several points and the click is in the area, 
  // move objects.
  if(!moving.isEmpty() && inArea(e->x(), e->y())){ 
    PointWithText* movingPoint = (PointWithText*)moving.first();
    
    QSortedList<PointWithText>* list = 0;
    switch(state){
    case all: 
      for(movingPoint; movingPoint != 0; movingPoint = (PointWithText*)moving.next()){
	switch(movingPoint->rtti()){
	case AmplitudePoint::rttiNr: moveHelp(aList, movingPoint, e->x(), e->y()); break; 
	case FrequencyPoint::rttiNr: moveHelp(fList, movingPoint, e->x(), e->y()); break;
	case NoisePoint::rttiNr: moveHelp(nList, movingPoint, e->x(), e->y()); break;
	default: std::cout<<"MorphArea::contentsMouseMoveEvent(QMouseEvent* e): no point???"<<endl;
	}
      }
      break;
      
    case amplitude: 
      if(movingPoint->rtti()== AmplitudePoint::rttiNr)
	moveHelp(aList, movingPoint, e->x(), e->y()); break;
    case frequency: 
      if(movingPoint->rtti()==FrequencyPoint::rttiNr)
	moveHelp(fList, movingPoint, e->x(), e->y()); break;
    case noise:   
      if(movingPoint->rtti()==NoisePoint::rttiNr)
	moveHelp(nList, movingPoint, e->x(), e->y()); break;
    default: std::cout<<"MorphArea::contentsMouseMoveEvent(QMouseEvent* e): no state?????"<<endl;
    }
    canvas()->update();
  }
}

// ---------------------------------------------------------------------------
//      moveHelp
// ---------------------------------------------------------------------------
// moves a point to (x,y)

void MorphArea::moveHelp(QSortedList<PointWithText>& list, PointWithText* movingPoint, int x,int y){
  // the first point in list isn't included in findRef, why???
  if(list.findRef(movingPoint) || movingPoint->x() == list.at(0)->x()){ 
    int current = list.at();
    
    if(list.count() == 1){   // last and first point in list
      if(current == 0){
	if(inArea(x,y))
	  movingPoint->move(x,y);
      }
    }
    
    else{                              // more than one object in the list
      if(current == 0){                // and the point is first in list
	PointWithText* next = list.at(current+1);
	if(x < next->x()){
	  movingPoint->move(x, y);
	  next->setLeftLine(list.at(current)->x(), list.at(current)->y());
	}
      }
      else{
	if(current == list.count()-1){ // last in list
	  if(x > list.at(current-1)->x())
	    movingPoint->move(x, y);
	}
	else{                          // middle point
	  PointWithText* next = list.at(current+1);
	  PointWithText* prev = list.at(current-1);
	  if(x < next->x() && x > prev->x()){
	    movingPoint->move(x, y);
	    next->setLeftLine(list.at(current)->x(), list.at(current)->y());
	  }
	}
      }
    }
  }
}

// ---------------------------------------------------------------------------
//      contentsMouseReleaseEvent
// ---------------------------------------------------------------------------
// Is called when the user releases the mouse buttons.

void MorphArea::contentsMouseReleaseEvent(QMouseEvent* e){
  moving.clear();
}

// ---------------------------------------------------------------------------
//      inArea
// ---------------------------------------------------------------------------
// Checks if the given (x,y) coorinat is in the area between the axis.

bool MorphArea::inArea(int x, int y){
  bool inX = x+1 >= leftMargin && x <= width-rightMargin; 
  bool inY = y+1 >= topMargin  && y <= height-bottomMargin; 
  return inX && inY;
}

// ---------------------------------------------------------------------------
//    addPoint
// ---------------------------------------------------------------------------
// Adds a new points to the area.

void MorphArea::addPoint(int x, int y){
  if(inArea(x, y)){
    switch(state){
    case all:
      addpointHelp(aList, new AmplitudePoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      addpointHelp(fList, new FrequencyPoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      addpointHelp(nList, new NoisePoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      break;

    case amplitude:
      addpointHelp(aList, new AmplitudePoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      break;
      
    case frequency:
      addpointHelp(fList, new FrequencyPoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      break;
      
    case noise:
      addpointHelp(nList, new NoisePoint(this, x, y, canvas(), newPointIndex, morph1, morph2));
      break;
    }
    newPointIndex =  newPointIndex + 2;
    canvas()->update();
  }
}

// ---------------------------------------------------------------------------
//    addPointHelp
// ---------------------------------------------------------------------------
// Adds a new point to the area.

void MorphArea::addpointHelp(QSortedList<PointWithText>& list, PointWithText* newPoint){
  PointWithText* prev;
  PointWithText* next;

  list.inSort(newPoint);  // is sorted after x-values.
  int current = list.at();
  
  if(current > 0){
    prev = list.at(current-1);
    newPoint->setLeftLine(prev->x(), prev->y());
  }
  else{
    newPoint->setLeftLine(getOrigo().x(), getOrigo().y());
  }
  if(current < list.count()-1){
    next = list.at(current+1);
    next->setLeftLine(newPoint->x(), newPoint->y());
  }
  moving.append(newPoint);
  newPoint->show();
}

// ---------------------------------------------------------------------------
//    clearAll
// ---------------------------------------------------------------------------
// Removes all points from the area.

void MorphArea::clearAll(){
  aList.clear();
  fList.clear();
  nList.clear();
  canvas()->update();
}

// ---------------------------------------------------------------------------
//    showHideClear
// ---------------------------------------------------------------------------
// Takes care of two different operations, show/hide and clear, depending on 
// buttonId from morphDialog. 

void MorphArea::showHideClear(int buttonId){
  // show/hide buttons are clicked in morphDialog
  if(buttonId>-1 && buttonId <4){ 
                                     
    switch (state){
    case all: 
      showHideList(aList, false);
      showHideList(fList, false);
      showHideList(nList, false);  
      break;
    case amplitude: showHideList(aList, false); break;  
    case frequency: showHideList(fList, false); break;  //list = &fList; break;  
    case noise:  showHideList(nList, false); //list = &nList; break; 
    default: std::cout<<"MorphArea::showHide(int buttonId): state is not set, something wrong"<<endl;
    }
    
    // set new state and show points belonging to that state.
    switch (buttonId){
    case 0: 
      state = all; 
      showHideList(aList, true);
      showHideList(fList, true);
      showHideList(nList, true);  
      break;
    case 1: state = amplitude; showHideList(aList, true); break; //list = aList; break;  
    case 2: state = frequency; showHideList(fList, true); break; //list = &fList; break;  
    case 3: state = noise; showHideList(nList, true); break; //list = &nList; break; 
    default: std::cout<<"MorphArea::showHide(int buttonId): no button id matches"<<endl;
    }
    
    canvas()->update();
  }
  
  // clear buttons clicked in morphDialog
  if(buttonId>3 && buttonId<8){
    switch (buttonId){
    case 4: aList.clear(); fList.clear(); nList.clear(); break; 
    case 5: aList.clear(); break;  
    case 6: fList.clear(); break;  
    case 7: nList.clear(); break; 
    }
    canvas()->update();
  }
  newPointIndex = 3;
}

// ---------------------------------------------------------------------------
//    showHideList
// ---------------------------------------------------------------------------
// shows or hides all items in a list.

void MorphArea::showHideList(QSortedList<PointWithText>& list, bool show){
  PointWithText* point;

  if(show){
    for(point = list.first(); point != 0; point = list.next()){
      point->show();
      newPointIndex = 5;
    }
  }
  else{
    for(point = list.first(); point != 0; point = list.next()){
      point->hide();
      newPointIndex = 5;
    }
  }
}

// ---------------------------------------------------------------------------
//    getOrigo
// ---------------------------------------------------------------------------
// retruns where the axis crosses.

const QPoint MorphArea::getOrigo() const{
  return QPoint(leftMargin, height - bottomMargin);
}

// ---------------------------------------------------------------------------
//    morph
// ---------------------------------------------------------------------------
// Morph button in morphArea is clicked

void MorphArea::morph(){
  BreakpointEnvelope amplitudePoints;
  BreakpointEnvelope frequencyPoints;
  BreakpointEnvelope noisePoints;
  
  fillEnvelope(aList, amplitudePoints);
  fillEnvelope(fList, frequencyPoints);
  fillEnvelope(nList, noisePoints);

  try{
    int time;
    int percent;
   
    partialsList->morph(amplitudePoints, frequencyPoints, noisePoints);
    clearAll();
  }
  
  catch(...){
  }
}

// ---------------------------------------------------------------------------
//    fillEnvelope
// ---------------------------------------------------------------------------
// Takes all points in the list and fills the envelope with the points.

void MorphArea::fillEnvelope(QSortedList<PointWithText>& list, BreakpointEnvelope& env){
  int time    = 0;
  int percent = 1; // percent index relates to second sound
  int count      = 0;
  
  if(list.first()!=0){ //insert a zero point first if there are points in list.
    env.insertBreakpoint(time, percent);  
  }
 
  for(PointWithText* point = list.first(); point != 0 ; point = list.next()){
    count++;
    time    = toXAxisValue(point->x());
    percent = 1-toYAxisValue(point->y())/100.0;  // percent index relates to second sound
    env.insertBreakpoint(time, percent);
  }
}

// ---------------------------------------------------------------------------
//    toXAxisValue
// ---------------------------------------------------------------------------
// An x-value on the canvas is translated into corresponding value on x-axis.

int MorphArea::toXAxisValue(int x){
  return ((int(x) - leftMargin)  * tAxis->getIndex());
}

// ---------------------------------------------------------------------------
//    toYAxisValue
// ---------------------------------------------------------------------------
// A y-value on the canvas is translated into corresponding value on y-axis.

int MorphArea::toYAxisValue(int y){
  return int((height-bottomMargin - y) * lAxis->getIndex());
}

// ---------------------------------------------------------------------------
//    setMorph1
// ---------------------------------------------------------------------------
// Changes items dependent on first partials to morph.

void MorphArea::setMorph1(QString& name){
  morph1 = name;
  if(!aList.isEmpty()){
    for(PointWithText* point = aList.first(); point!=0; point = aList.next()){
      point->setMorph1(name);
    }
  }
  
  if(!fList.isEmpty()){
    for(PointWithText* point = fList.first(); point!=0; point = fList.next()){
      point->setMorph1(name);
    }
  }

  if(!nList.isEmpty()){
    for(PointWithText* point = nList.first(); point!=0; point = nList.next()){
      point->setMorph1(name);
    }
  }

  setHorizontalAxis();  // the maximum value might have changed
}

// ---------------------------------------------------------------------------
//    setMorph2
// ---------------------------------------------------------------------------
// Changes items dependent on second partials to morph.

void MorphArea::setMorph2(QString& name){
  morph2 = name;
  if(!aList.isEmpty()){
    for(PointWithText* point = aList.first(); point!=0; point = aList.next()){
      point->setMorph2(name);
    }
  }
  
  if(!fList.isEmpty()){
    for(PointWithText* point = fList.first(); point!=0; point = fList.next()){
      point->setMorph2(name);
    }
  }

  if(!nList.isEmpty()){
    for(PointWithText* point = nList.first(); point!=0; point = nList.next()){
      point->setMorph2(name);
    }
  }

  setHorizontalAxis(); // the maximum value might have changed
}

// ---------------------------------------------------------------------------
//    setHorizontalAxis
// ---------------------------------------------------------------------------
// Adds the horisontal axis to the area.

void MorphArea::setHorizontalAxis(){

  int time = 0;
  int time1 = partialsList->getMorph1Duration();
  int time2 = partialsList->getMorph2Duration();
  
  if(time1 > time2){
    time = time1;
  }
  else time = time2;
  
  if(tAxis){
    delete tAxis;
  }

  tAxis = new HorizontalAxis(canvas(), leftMargin, height - bottomMargin, "time", 
			     width-rightMargin-leftMargin, 30, 100,  int(0), time); 
  tAxis->show();
  canvas()->update();
}















