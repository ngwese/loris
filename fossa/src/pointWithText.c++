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
  * pointWithText.c++
  *
  * The class for representing breakpoints is called PointWithText, and is inherited by
  * the classes; AmplitudePoint, FrequencyPoint, and NoisePoint. See code for more detailed
  * descriptions.
  *
  * Susanne Lefvert, 1 March 2002
  *
  *
  */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "pointWithText.h"

using namespace Loris;

// ---------------------------------------------------------------------------
// class PointWithText
//
// A PointWithText consists of a line, percent and time text, and a drawn rectangle, 
// which from now on is referred to as a point. The class inherits QCanvasRectangle,
// and it can be placed in a canvas, shown to users by a QCanvasView. The 
// QCanvasRectangle itself is invisible, what is shown is the texts, line and 
// the point which is placed inside the invisible rectangle.  A PointWithText is 
// inserted to the canvas everytime the user clicks on an empty spot in the MorphArea 
// and its components gets viewable. Points can be inserted between two points as 
// well after. If the user clicks on an already inserted PointWithText, the point 
// gets movable and the user can drag it, between its neighbours, on the MorphArea.
// A PointWithText can be erased by right clicking on it.

// ---------------------------------------------------------------------------
//      PointWithText constructor
// ---------------------------------------------------------------------------
// Creates a new point to place on the canvas. 

PointWithText::PointWithText(MorphArea* morphArea, double ix, double iy, QCanvas* canvas, int id, QString& morph1, QString& morph2)
  :QCanvasRectangle(ix - size, iy - size, 2 * size, 2 * size, canvas){ 
  // a user clicks on the view with coordinates (ix,iy), a rectangele is created
  // around the coordinates, with width and height 2 * size.
  
  partial1 = morph1; // to be able to set right text.
  partial2 = morph2;
  m = morphArea;
  lineLeft  = new QCanvasLine(canvas); // line to left neighbour breakpoint
  percentLabel = new QCanvasText("percentLabel",QFont("helvetica", 8),canvas);
  timeLabel = new QCanvasText("timeLabel",QFont("helvetica", 8),canvas);
  
  
  // The z values decides the depths of the point. Z-values are specified as follows:
  // User looks down here 
  
  // z-value 5 --------- new point starts here and next point's z is increased by 1
  // (z-value 4 --------- new point's text and lines NOT increased) 
  // z-value 3 --------- shown PointsWithText
  // (z-value 2 --------- shown  texts and lines) 
  // z-value 1 --------- hidden PointsWithText
  // (z-value 0 --------- hidden texts and lines)

  // If 2 points are placed over eachother the last point added will be the one 
  // which will be draggable and erasable. Hidden points will be placed below 
  // shown points. 

  lineLeft->setZ(4);
  percentLabel->setZ(4);
  timeLabel->setZ(4);
  setZ(id);
    
  setGui();
}

// ---------------------------------------------------------------------------
//      PointWithText destructor
// ---------------------------------------------------------------------------

PointWithText::~PointWithText(){
  delete lineLeft;
  delete percentLabel;
  delete timeLabel;
}

// ---------------------------------------------------------------------------
//      Pure virtual methods implemented by subclasses
// ---------------------------------------------------------------------------

//void PointWithText::show() = 0;
//void PointWithText::hide() = 0;
//int PointWithText::rtti() const = 0;

// ---------------------------------------------------------------------------
//     x
// ---------------------------------------------------------------------------
// Return x coordinate

double PointWithText::x() const{
  return QCanvasRectangle::x() + size;  // where the user originally clicked
}

// ---------------------------------------------------------------------------
//     y
// ---------------------------------------------------------------------------
// Return y coordinate
  
double PointWithText::y() const{
  return QCanvasRectangle::y() + size;  // where the user originally clicked
}
  
// ---------------------------------------------------------------------------
//     operators used by QSortedList to sort point after x values
// ---------------------------------------------------------------------------
 
int PointWithText::operator==(PointWithText& right){
  return x() == right.x();
}

int PointWithText::operator!=(PointWithText& right){
  return (x() != right.x());
}

int PointWithText::operator<(PointWithText& right){
  return x() < right.x();
}

int PointWithText::operator>(PointWithText& right){
  return x() > right.x();
}

// ---------------------------------------------------------------------------
//     move
// ---------------------------------------------------------------------------
// Moves the point to new (x,y) coordinate

void PointWithText::move(double x, double y){
  QCanvasRectangle::move(x - size, y - size);
  percentLabel->move(x+7,y-25);
  timeLabel->move(x+7,y);
  moveLeftLine(x, y);
}

// ---------------------------------------------------------------------------
//     moveLeftLine
// ---------------------------------------------------------------------------
// (can not use setLeftLine because 1 is added to every move)
// This method is virtual because I don't want lines to be exactly at the same
// place. Each subclass places the left line slightly different.

//void PointWithText::moveLeftLine(double lastX, double lastY) = 0;

// ---------------------------------------------------------------------------
//     setLeftLine
// ---------------------------------------------------------------------------
// This method is virtual because I don't want lines to be exactly at the same
// place. Each subclass places the left line slightly different.

//void PointWithText::setLeftLine(double toX, double toY) = 0; 
  
// ---------------------------------------------------------------------------
//     drawShape
// ---------------------------------------------------------------------------
// Have to be implemented when a class inherits QCanvas objects.

void PointWithText::drawShape(QPainter & painter){
  painter.setPen(Qt::black);
  painter.drawRect (x()-size/2.0, y()-size/2.0, size, size); 
  setPointText();
}  

// ---------------------------------------------------------------------------
//     setGui
// ---------------------------------------------------------------------------
// Sets GUI components of this class

void PointWithText::setGui(){
  percentLabel->setX(x()+7);
  percentLabel->setY(y()-25);
  timeLabel->setX(x()+7);
  timeLabel->setY(y());
    
  setPointText();
    
  percentLabel->show();
  timeLabel->show();
  lineLeft->show();
  QCanvasRectangle::show();
}
  
// ---------------------------------------------------------------------------
//     setPointText
// ---------------------------------------------------------------------------
// Helper to setGui, sets the texts beloning to the point

void PointWithText::setPointText(){
  int percent  = m->toYAxisValue(y());
  double time  = m->toXAxisValue(x());
  percentLabel ->setText(QString(partial1+": %1% \n"+partial2+": %2%").arg(percent).arg(100-percent));
  timeLabel    ->setText(QString("At time %1s").arg(time));
}

// ---------------------------------------------------------------------------
//      setMorph1
// ---------------------------------------------------------------------------
// Sets all GUI components depending on the first collection of partials to 
// the new name and updates the view.

void PointWithText::setMorph1(QString& name){
  partial1 = name;
  setPointText();
  canvas()->update();
}
  
// ---------------------------------------------------------------------------
//      setMorph2
// ---------------------------------------------------------------------------
// Sets all GUI components depending on the second collection of partials to 
// the new name and updates the view.

void PointWithText::setMorph2(QString& name){
  partial2 = name;
  setPointText();
  canvas()->update();
}

// ---------------------------------------------------------------------------
//     AmplitudePoint constructor
// ---------------------------------------------------------------------------
  
AmplitudePoint::AmplitudePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1,QString& morph2):
  PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
  pen.setColor(QColor("red"));
  lineLeft->setPen(pen);
}

// ---------------------------------------------------------------------------
//     rtti
// ---------------------------------------------------------------------------
// Returns a Run Time Type Identification value to make it possible to 
// distinguish between objects returned by QCanvas::at(). 

int AmplitudePoint::rtti() const{
  return rttiNr;
}

// ---------------------------------------------------------------------------
//     hide
// ---------------------------------------------------------------------------
// Hide means that a point is faded and is placed on a lower z-level.
 
void AmplitudePoint::hide(){
  percentLabel->setColor("grey");
  timeLabel->setColor("grey");
  pen.setColor(QColor("lightpink"));
  lineLeft->setPen(pen);
  PointWithText::setZ(1);
  lineLeft->setZ(0);
  percentLabel->setZ(0);
  timeLabel->setZ(0); 
}

// ---------------------------------------------------------------------------
//     show
// ---------------------------------------------------------------------------
// Show means that a point has its real color and is placed on a higher z-level
// compared it's hidden state.
  
void AmplitudePoint::show(){
  percentLabel->setColor("black");
  timeLabel->setColor("black");
  pen.setColor(QColor("red"));
  lineLeft->setPen(pen);
  PointWithText::setZ(3);
  lineLeft->setZ(2);
  percentLabel->setZ(2);
  timeLabel->setZ(2); 
}

// ---------------------------------------------------------------------------
//     setLeftLine
// ---------------------------------------------------------------------------
// Change the left line connecting this point to the closest left neighbour.
  
void AmplitudePoint::setLeftLine(double toX, double toY){
  lineLeft->setPoints(toX+1, toY, x()+1, y());
  lineLeft->show();
}

// ---------------------------------------------------------------------------
//     moveLeftLine
// ---------------------------------------------------------------------------
// Moves the left line connecting this point to the closest left neighbour. 
// We cannot use setLeftLine for this purpous since it uses x()+1 which will
// continue adding the x value of the first end.
  
void AmplitudePoint::moveLeftLine(double toX, double toY){
  lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX+1, toY);
  lineLeft->show();
}

// ---------------------------------------------------------------------------
//     FrequencyPoint constructor
// ---------------------------------------------------------------------------

FrequencyPoint::FrequencyPoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2):
  PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
  pen.setColor(QColor("palegreen4"));
  lineLeft->setPen(pen);
}

// ---------------------------------------------------------------------------
//     rtti
// ---------------------------------------------------------------------------
// Returns a Run Time Type Identification value to make it possible to 
// distinguish between objects returned by QCanvas::at(). 

int FrequencyPoint::rtti() const{
  return rttiNr;
}
  
// ---------------------------------------------------------------------------
//     hide
// ---------------------------------------------------------------------------
// Hide means that a point is faded and is placed on a lower z-level.
  
void FrequencyPoint::hide(){
  percentLabel->setColor("grey");
  timeLabel->setColor("grey");
  pen.setColor(QColor("darkseagreen1"));
  lineLeft->setPen(pen);
  PointWithText::setZ(1);
  lineLeft->setZ(0);
  percentLabel->setZ(0);
  timeLabel->setZ(0); 
}

// ---------------------------------------------------------------------------
//     show
// ---------------------------------------------------------------------------
// Show means that a point has its real color and is placed on a higher z-level
// compared it's hidden state.
  
void FrequencyPoint::show(){
  percentLabel->setColor("black");
  timeLabel->setColor("black");
  pen.setColor(QColor("palegreen4"));
  lineLeft->setPen(pen);
  PointWithText::setZ(3);
  lineLeft->setZ(2);
  percentLabel->setZ(2);
  timeLabel->setZ(2); 
}
  
// ---------------------------------------------------------------------------
//     setLeftLine
// ---------------------------------------------------------------------------
// Change the left line connecting this point to the closest left neighbour.
  
void FrequencyPoint::setLeftLine(double toX, double toY){
  lineLeft->setPoints(toX-1, toY-1, x()-1, y()-1);
  lineLeft->show();
}
  
// ---------------------------------------------------------------------------
//     moveLeftLine
// ---------------------------------------------------------------------------
// Moves the left line connecting this point to the closest left neighbour. 
// We cannot use setLeftLine for this purpous since it uses x()-1 which will
// continue subtracting the x value of the first end.
   
void FrequencyPoint::moveLeftLine(double toX, double toY){
  lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY-1);
  lineLeft->show();
}

// ---------------------------------------------------------------------------
//     NoisePoint constructor
// ---------------------------------------------------------------------------

NoisePoint::NoisePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2 ):
  PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
  pen.setColor(QColor("blue"));
  lineLeft->setPen(pen);
}
  
// ---------------------------------------------------------------------------
//     rtti
// ---------------------------------------------------------------------------
// Returns a Run Time Type Identification value to make it possible to 
// distinguish between objects returned by QCanvas::at(). 

int NoisePoint::rtti() const{
  return rttiNr;
}
  
// ---------------------------------------------------------------------------
//     hide
// ---------------------------------------------------------------------------
// Hide means that a point is faded and is placed on a lower z-level.
  
void NoisePoint::hide(){
  percentLabel->setColor("grey");
  timeLabel->setColor("grey");
  pen.setColor(QColor("lightskyblue3"));
  lineLeft->setPen(pen);
  PointWithText::setZ(1);
  lineLeft->setZ(0);
  percentLabel->setZ(0);
  timeLabel->setZ(0); 
}
  
// ---------------------------------------------------------------------------
//     show
// ---------------------------------------------------------------------------
// Show means that a point has its real color and is placed on a higher z-level
// compared it's hidden state.
  
void NoisePoint::show(){
  percentLabel->setColor("black");
  timeLabel->setColor("black");
  pen.setColor(QColor("blue"));
  lineLeft->setPen(pen);
  PointWithText::setZ(3);
  lineLeft->setZ(2);
  percentLabel->setZ(2);
  timeLabel->setZ(2); 
}

// ---------------------------------------------------------------------------
//     setLeftLine
// ---------------------------------------------------------------------------
// Change the left line connecting this point to the closest left neighbour.
  
void NoisePoint::setLeftLine(double toX, double toY){
  lineLeft->setPoints(toX-1, toY+1, x()-1, y()+1);
  lineLeft->show();
}
  
// ---------------------------------------------------------------------------
//     moveLeftLine
// ---------------------------------------------------------------------------
// Moves the left line connecting this point to the closest left neighbour. 
// We cannot use setLeftLine for this purpous since it uses x()-1 y()-1 which will
// continue subtracting the x and y value of the first end.
  
void NoisePoint::moveLeftLine(double toX, double toY){  
  lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY+1);
  lineLeft->show();
}


/*
    class PointWithText;
 
    using namespace Loris;

    // ---------------------------------------------------------------------------
    // class PointWithText
    //
    // A PointWithText consists of a line, percent and time text, and a drawn rectangle, 
    // which from now on is referred to as a point. The class inherits QCanvasRectangle,
    // and it can be placed in a canvas, shown to users by a QCanvasView. The 
    // QCanvasRectangle itself is invisible, what is shown is the texts, line and 
    // the point which is placed inside the invisible rectangle.  A PointWithText is 
    // inserted to the canvas everytime the user clicks on an empty spot in the MorphArea 
    // and its components gets viewable. Points can be inserted between two points as 
    // well after. If the user clicks on an already inserted PointWithText, the point 
    // gets movable and the user can drag it, between its neighbours, on the MorphArea.
    // A PointWithText can be erased by right clicking on it.

    class PointWithText:public QCanvasRectangle{ 
    public:
    const static double size = 4; 


    // ---------------------------------------------------------------------------
    //      PointWithText constructor
    // ---------------------------------------------------------------------------
    // Creates a new point to place on the canvas. 

    PointWithText(MorphArea* morphArea, double ix, double iy, QCanvas* canvas, int id, QString& morph1, QString& morph2)
    :QCanvasRectangle(ix - size, iy - size, 2 * size, 2 * size, canvas){ 
    // a user clicks on the view with coordinates (ix,iy), a rectangele is created
    // around the coordinates, with width and height 2 * size.

    partial1 = morph1; // to be able to set right text.
    partial2 = morph2;
    m = morphArea;
     lineLeft  = new QCanvasLine(canvas); // line to left neighbour breakpoint
     percentLabel = new QCanvasText("percentLabel",QFont("helvetica", 8),canvas);
     timeLabel = new QCanvasText("timeLabel",QFont("helvetica", 8),canvas);
    

    // The z values decides the depths of the point. Z-values are specified as follows:
    // User looks down here 

    // z-value 5 --------- new point starts here and next point's z is increased by 1
    // (z-value 4 --------- new point's text and lines NOT increased) 
    // z-value 3 --------- shown PointsWithText
    // (z-value 2 --------- shown  texts and lines) 
    // z-value 1 --------- hidden PointsWithText
    // (z-value 0 --------- hidden texts and lines)

    // If 2 points are placed over eachother the last point added will be the one 
    // which will be draggable and erasable. Hidden points will be placed below 
    // shown points. 

    lineLeft->setZ(4);
    percentLabel->setZ(4);
    timeLabel->setZ(4);
    setZ(id);
    
    setGui();
    }

    // ---------------------------------------------------------------------------
    //      PointWithText destructor
    // ---------------------------------------------------------------------------

    ~PointWithText(){
    delete lineLeft;
    delete percentLabel;
    delete timeLabel;
    }
  
    // ---------------------------------------------------------------------------
    //      Pure virtual methods implemented by subclasses
    // ---------------------------------------------------------------------------

    virtual void show() = 0;
    virtual void hide() = 0;
    virtual int rtti() const = 0;
  
    // ---------------------------------------------------------------------------
    //     x
    // ---------------------------------------------------------------------------
    // Return x coordinate

    double x() const{
    return QCanvasRectangle::x() + size;  // where the user originally clicked
    }
  
    // ---------------------------------------------------------------------------
    //     y
    // ---------------------------------------------------------------------------
    // Return y coordinate
  
    double y() const{
    return QCanvasRectangle::y() + size;  // where the user originally clicked
    }
  
    // ---------------------------------------------------------------------------
    //     operators used by QSortedList to sort point after x values
    // ---------------------------------------------------------------------------
 
    int operator==(PointWithText& right){
    return x() == right.x();
    }

    int operator!=(PointWithText& right){
    return (x() != right.x());
    }

    int operator<(PointWithText& right){
    return x() < right.x();
    }

    int operator>(PointWithText& right){
    return x() > right.x();
    }

    // ---------------------------------------------------------------------------
    //     move
    // ---------------------------------------------------------------------------
    // Moves the point to new (x,y) coordinate

    void move(double x, double y){
    QCanvasRectangle::move(x - size, y - size);
    percentLabel->move(x+7,y-25);
    timeLabel->move(x+7,y);
    moveLeftLine(x, y);
    }
  
    // ---------------------------------------------------------------------------
    //     moveLeftLine
    // ---------------------------------------------------------------------------
    // (can not use setLeftLine because 1 is added to every move)
    // This method is virtual because I don't want lines to be exactly at the same
    // place. Each subclass places the left line slightly different.

    virtual void moveLeftLine(double lastX, double lastY) = 0;

    // ---------------------------------------------------------------------------
    //     setLeftLine
    // ---------------------------------------------------------------------------
    // This method is virtual because I don't want lines to be exactly at the same
    // place. Each subclass places the left line slightly different.
  
    virtual void setLeftLine(double toX, double toY) = 0; 
  
    // ---------------------------------------------------------------------------
    //     drawShape
    // ---------------------------------------------------------------------------
    // Have to be implemented when a class inherits QCanvas objects.

    void drawShape(QPainter & painter){
    painter.setPen(Qt::black);
    painter.drawRect (x()-size/2.0, y()-size/2.0, size, size); 
    setPointText();
    }  

    // ---------------------------------------------------------------------------
    //     setGui
    // ---------------------------------------------------------------------------
    // Sets GUI components of this class

    void setGui(){
    percentLabel->setX(x()+7);
    percentLabel->setY(y()-25);
    timeLabel->setX(x()+7);
    timeLabel->setY(y());
    
    setPointText();
    
    percentLabel->show();
    timeLabel->show();
    lineLeft->show();
    QCanvasRectangle::show();
    }
  
    // ---------------------------------------------------------------------------
    //     setPointText
    // ---------------------------------------------------------------------------
    // Helper to setGui, sets the texts beloning to the point

    void setPointText(){
    int percent  = m->toYAxisValue(y());
    double time  = m->toXAxisValue(x());
    percentLabel ->setText(QString(partial1+": %1% \n"+partial2+": %2%").arg(percent).arg(100-percent));
    timeLabel    ->setText(QString("At time %1s").arg(time));
    }

    // ---------------------------------------------------------------------------
    //      setMorph1
    // ---------------------------------------------------------------------------
    // Sets all GUI components depending on the first collection of partials to 
    // the new name and updates the view.

    void setMorph1(QString& name){
    partial1 = name;
    setPointText();
    canvas()->update();
    }
  
    // ---------------------------------------------------------------------------
    //      setMorph2
    // ---------------------------------------------------------------------------
    // Sets all GUI components depending on the second collection of partials to 
    // the new name and updates the view.

    void setMorph2(QString& name){
    partial2 = name;
    setPointText();
    canvas()->update();
    }

    protected:
    QCanvasText* percentLabel; 
    QCanvasText* timeLabel;
    QCanvasLine* lineLeft;
    MorphArea* m;
    QString partial1;
    QString partial2;
    }; 


    // ---------------------------------------------------------------------------
    // class AmplitudePoint
    //
    // Inherits PointWithText and represents an amplitude point on the canvas.

    class AmplitudePoint:public PointWithText{

    public:
    const static int rttiNr = 2000;  
    QPen pen;

  // ---------------------------------------------------------------------------
  //     AmplitudePoint constructor
  // ---------------------------------------------------------------------------
  
  AmplitudePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1,QString& morph2):
  PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
  pen.setColor(QColor("red"));
  lineLeft->setPen(pen);
  }

  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
  return rttiNr;
  }

  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
 
  void hide(){
  percentLabel->setColor("grey");
  timeLabel->setColor("grey");
  pen.setColor(QColor("lightpink"));
  lineLeft->setPen(pen);
  PointWithText::setZ(1);
  lineLeft->setZ(0);
  percentLabel->setZ(0);
  timeLabel->setZ(0); 
  }

  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
  percentLabel->setColor("black");
  timeLabel->setColor("black");
  pen.setColor(QColor("red"));
  lineLeft->setPen(pen);
  PointWithText::setZ(3);
  lineLeft->setZ(2);
  percentLabel->setZ(2);
  timeLabel->setZ(2); 
  }

  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
  lineLeft->setPoints(toX+1, toY, x()+1, y());
  lineLeft->show();
  }

  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()+1 which will
  // continue adding the x value of the first end.
  
  void moveLeftLine(double toX, double toY){
  lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX+1, toY);
  lineLeft->show();
  }
  };


  // ---------------------------------------------------------------------------
  // class FrequencyPoint
  //
  // Inherits PointWithText and represents a frequency point on the canvas.

  class FrequencyPoint:public PointWithText{

public:
  QPen pen;
  const static int rttiNr = 2001;

  // ---------------------------------------------------------------------------
  //     FrequencyPoint constructor
  // ---------------------------------------------------------------------------
  
  FrequencyPoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2):
    PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
    pen.setColor(QColor("palegreen4"));
    lineLeft->setPen(pen);
  }

  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
    return rttiNr;
  }
  
  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
  
  void hide(){
    percentLabel->setColor("grey");
    timeLabel->setColor("grey");
    pen.setColor(QColor("darkseagreen1"));
    lineLeft->setPen(pen);
    PointWithText::setZ(1);
    lineLeft->setZ(0);
    percentLabel->setZ(0);
    timeLabel->setZ(0); 
  }

  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
    percentLabel->setColor("black");
    timeLabel->setColor("black");
    pen.setColor(QColor("palegreen4"));
    lineLeft->setPen(pen);
    PointWithText::setZ(3);
    lineLeft->setZ(2);
    percentLabel->setZ(2);
    timeLabel->setZ(2); 
  }
  
  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
    lineLeft->setPoints(toX-1, toY-1, x()-1, y()-1);
    lineLeft->show();
  }
  
  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()-1 which will
  // continue subtracting the x value of the first end.
   
  void moveLeftLine(double toX, double toY){
    lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY-1);
    lineLeft->show();
  }
  };

  // ---------------------------------------------------------------------------
  // class NoisePoint
  //
  // Inherits PointWithText and represents a noise point on the canvas.

  class NoisePoint:public PointWithText{

public:
  const static int rttiNr = 2002;
  QPen pen;

  // ---------------------------------------------------------------------------
  //     NoisePoint constructor
  // ---------------------------------------------------------------------------

  NoisePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2 ):
    PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
    pen.setColor(QColor("blue"));
    lineLeft->setPen(pen);
  }
  
  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
    return rttiNr;
  }
  
  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
  
  void hide(){
    percentLabel->setColor("grey");
    timeLabel->setColor("grey");
    pen.setColor(QColor("lightskyblue3"));
    lineLeft->setPen(pen);
    PointWithText::setZ(1);
    lineLeft->setZ(0);
    percentLabel->setZ(0);
    timeLabel->setZ(0); 
  }
  
  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
    percentLabel->setColor("black");
    timeLabel->setColor("black");
    pen.setColor(QColor("blue"));
    lineLeft->setPen(pen);
    PointWithText::setZ(3);
    lineLeft->setZ(2);
    percentLabel->setZ(2);
    timeLabel->setZ(2); 
  }

  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
    lineLeft->setPoints(toX-1, toY+1, x()-1, y()+1);
    lineLeft->show();
  }
  
  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()-1 y()-1 which will
  // continue subtracting the x and y value of the first end.
  
  void moveLeftLine(double toX, double toY){  
    lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY+1);
    lineLeft->show();
    }
    };
*/
/*

  #if HAVE_CONFIG_H
  #include <config.h>  // #define directives are placed in config.h by autoconf
  #endif
  
  #include "morphArea.h"
  
  
  class PointWithText;
  
  using namespace Loris;
  
// ---------------------------------------------------------------------------
// class PointWithText
//
// A PointWithText consists of a line, percent and time text, and a drawn rectangle, 
// which from now on is referred to as a point. The class inherits QCanvasRectangle,
// and it can be placed in a canvas, shown to users by a QCanvasView. The 
// QCanvasRectangle itself is invisible, what is shown is the texts, line and 
// the point which is placed inside the invisible rectangle.  A PointWithText is 
// inserted to the canvas everytime the user clicks on an empty spot in the MorphArea 
// and its components gets viewable. Points can be inserted between two points as 
// well after. If the user clicks on an already inserted PointWithText, the point 
// gets movable and the user can drag it, between its neighbours, on the MorphArea.
// A PointWithText can be erased by right clicking on it.

class PointWithText:public QCanvasRectangle{ 
public:
  const static double size = 4; 


  // ---------------------------------------------------------------------------
  //      PointWithText constructor
  // ---------------------------------------------------------------------------
  // Creates a new point to place on the canvas. 

  PointWithText(MorphArea* morphArea, double ix, double iy, QCanvas* canvas, int id, QString& morph1, QString& morph2)
    :QCanvasRectangle(ix - size, iy - size, 2 * size, 2 * size, canvas){ 
    // a user clicks on the view with coordinates (ix,iy), a rectangele is created
    // around the coordinates, with width and height 2 * size.

    partial1 = morph1; // to be able to set right text.
    partial2 = morph2;
    m = morphArea;
    lineLeft  = new QCanvasLine(canvas); // line to left neighbour breakpoint
    percentLabel = new QCanvasText("percentLabel",QFont("helvetica", 8),canvas);
    timeLabel = new QCanvasText("timeLabel",QFont("helvetica", 8),canvas);
    

    // The z values decides the depths of the point. Z-values are specified as follows:
    // User looks down here 

    // z-value 5 --------- new point starts here and next point's z is increased by 1
    // (z-value 4 --------- new point's text and lines NOT increased) 
    // z-value 3 --------- shown PointsWithText
    // (z-value 2 --------- shown  texts and lines) 
    // z-value 1 --------- hidden PointsWithText
    // (z-value 0 --------- hidden texts and lines)

    // If 2 points are placed over eachother the last point added will be the one 
    // which will be draggable and erasable. Hidden points will be placed below 
    // shown points. 

    lineLeft->setZ(4);
    percentLabel->setZ(4);
    timeLabel->setZ(4);
    setZ(id);
    
    setGui();
  }

  // ---------------------------------------------------------------------------
  //      PointWithText destructor
  // ---------------------------------------------------------------------------

  ~PointWithText(){
    delete lineLeft;
    delete percentLabel;
    delete timeLabel;
  }
  
  // ---------------------------------------------------------------------------
  //      Pure virtual methods implemented by subclasses
  // ---------------------------------------------------------------------------

  virtual void show() = 0;
  virtual void hide() = 0;
  virtual int rtti() const = 0;
  
  // ---------------------------------------------------------------------------
  //     x
  // ---------------------------------------------------------------------------
  // Return x coordinate

  double x() const{
    return QCanvasRectangle::x() + size;  // where the user originally clicked
  }
  
  // ---------------------------------------------------------------------------
  //     y
  // ---------------------------------------------------------------------------
  // Return y coordinate
  
  double y() const{
    return QCanvasRectangle::y() + size;  // where the user originally clicked
  }
  
  // ---------------------------------------------------------------------------
  //     operators used by QSortedList to sort point after x values
  // ---------------------------------------------------------------------------
 
  int operator==(PointWithText& right){
    return x() == right.x();
  }

  int operator!=(PointWithText& right){
    return (x() != right.x());
  }

  int operator<(PointWithText& right){
    return x() < right.x();
  }

  int operator>(PointWithText& right){
    return x() > right.x();
  }

  // ---------------------------------------------------------------------------
  //     move
  // ---------------------------------------------------------------------------
  // Moves the point to new (x,y) coordinate

  void move(double x, double y){
    QCanvasRectangle::move(x - size, y - size);
    percentLabel->move(x+7,y-25);
    timeLabel->move(x+7,y);
    moveLeftLine(x, y);
  }
  
  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // (can not use setLeftLine because 1 is added to every move)
  // This method is virtual because I don't want lines to be exactly at the same
  // place. Each subclass places the left line slightly different.

  virtual void moveLeftLine(double lastX, double lastY) = 0;

  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // This method is virtual because I don't want lines to be exactly at the same
  // place. Each subclass places the left line slightly different.
  
  virtual void setLeftLine(double toX, double toY) = 0; 
  
  // ---------------------------------------------------------------------------
  //     drawShape
  // ---------------------------------------------------------------------------
  // Have to be implemented when a class inherits QCanvas objects.

  void drawShape(QPainter & painter){
    painter.setPen(Qt::black);
    painter.drawRect (x()-size/2.0, y()-size/2.0, size, size); 
    setPointText();
  }  

  // ---------------------------------------------------------------------------
  //     setGui
  // ---------------------------------------------------------------------------
  // Sets GUI components of this class

  void setGui(){
    percentLabel->setX(x()+7);
    percentLabel->setY(y()-25);
    timeLabel->setX(x()+7);
    timeLabel->setY(y());
    
    setPointText();
    
    percentLabel->show();
    timeLabel->show();
    lineLeft->show();
    QCanvasRectangle::show();
  }
  
  // ---------------------------------------------------------------------------
  //     setPointText
  // ---------------------------------------------------------------------------
  // Helper to setGui, sets the texts beloning to the point

  void setPointText(){
    int percent  = m->toYAxisValue(y());
    double time  = m->toXAxisValue(x());
    percentLabel ->setText(QString(partial1+": %1% \n"+partial2+": %2%").arg(percent).arg(100-percent));
    timeLabel    ->setText(QString("At time %1s").arg(time));
  }

  // ---------------------------------------------------------------------------
  //      setMorph1
  // ---------------------------------------------------------------------------
  // Sets all GUI components depending on the first collection of partials to 
  // the new name and updates the view.

  void setMorph1(QString& name){
    partial1 = name;
    setPointText();
    canvas()->update();
  }
  
  // ---------------------------------------------------------------------------
  //      setMorph2
  // ---------------------------------------------------------------------------
  // Sets all GUI components depending on the second collection of partials to 
  // the new name and updates the view.

  void setMorph2(QString& name){
    partial2 = name;
    setPointText();
    canvas()->update();
  }

protected:
  QCanvasText* percentLabel; 
  QCanvasText* timeLabel;
  QCanvasLine* lineLeft;
  MorphArea* m;
  QString partial1;
  QString partial2;
}; 


// ---------------------------------------------------------------------------
// class AmplitudePoint
//
// Inherits PointWithText and represents an amplitude point on the canvas.

class AmplitudePoint:public PointWithText{

public:
  const static int rttiNr = 2000;  
  QPen pen;

  // ---------------------------------------------------------------------------
  //     AmplitudePoint constructor
  // ---------------------------------------------------------------------------
  
  AmplitudePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1,QString& morph2):
    PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
    pen.setColor(QColor("red"));
    lineLeft->setPen(pen);
  }

  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
    return rttiNr;
  }

  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
 
  void hide(){
    percentLabel->setColor("grey");
    timeLabel->setColor("grey");
    pen.setColor(QColor("lightpink"));
    lineLeft->setPen(pen);
    PointWithText::setZ(1);
    lineLeft->setZ(0);
    percentLabel->setZ(0);
    timeLabel->setZ(0); 
  }

  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
    percentLabel->setColor("black");
    timeLabel->setColor("black");
    pen.setColor(QColor("red"));
    lineLeft->setPen(pen);
    PointWithText::setZ(3);
    lineLeft->setZ(2);
    percentLabel->setZ(2);
    timeLabel->setZ(2); 
  }

  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
    lineLeft->setPoints(toX+1, toY, x()+1, y());
    lineLeft->show();
  }

  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()+1 which will
  // continue adding the x value of the first end.
  
  void moveLeftLine(double toX, double toY){
    lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX+1, toY);
    lineLeft->show();
  }
};


// ---------------------------------------------------------------------------
// class FrequencyPoint
//
// Inherits PointWithText and represents a frequency point on the canvas.

class FrequencyPoint:public PointWithText{

public:
  QPen pen;
  const static int rttiNr = 2001;

  // ---------------------------------------------------------------------------
  //     FrequencyPoint constructor
  // ---------------------------------------------------------------------------
  
  FrequencyPoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2):
    PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
    pen.setColor(QColor("palegreen4"));
    lineLeft->setPen(pen);
  }

  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
    return rttiNr;
  }
  
  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
  
  void hide(){
    percentLabel->setColor("grey");
    timeLabel->setColor("grey");
    pen.setColor(QColor("darkseagreen1"));
    lineLeft->setPen(pen);
    PointWithText::setZ(1);
    lineLeft->setZ(0);
    percentLabel->setZ(0);
    timeLabel->setZ(0); 
  }

  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
    percentLabel->setColor("black");
    timeLabel->setColor("black");
    pen.setColor(QColor("palegreen4"));
    lineLeft->setPen(pen);
    PointWithText::setZ(3);
    lineLeft->setZ(2);
    percentLabel->setZ(2);
    timeLabel->setZ(2); 
  }
  
  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
    lineLeft->setPoints(toX-1, toY-1, x()-1, y()-1);
    lineLeft->show();
  }
  
  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()-1 which will
  // continue subtracting the x value of the first end.
   
  void moveLeftLine(double toX, double toY){
    lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY-1);
    lineLeft->show();
  }
};

// ---------------------------------------------------------------------------
// class NoisePoint
//
// Inherits PointWithText and represents a noise point on the canvas.

class NoisePoint:public PointWithText{

public:
  const static int rttiNr = 2002;
  QPen pen;

  // ---------------------------------------------------------------------------
  //     NoisePoint constructor
  // ---------------------------------------------------------------------------

  NoisePoint(MorphArea* m, double ix, double iy, QCanvas* canvas, int newPointIndex, QString& morph1, QString& morph2 ):
    PointWithText(m, ix, iy, canvas, newPointIndex, morph1, morph2){
    pen.setColor(QColor("blue"));
    lineLeft->setPen(pen);
  }
  
  // ---------------------------------------------------------------------------
  //     rtti
  // ---------------------------------------------------------------------------
  // Returns a Run Time Type Identification value to make it possible to 
  // distinguish between objects returned by QCanvas::at(). 

  int rtti() const{
    return rttiNr;
  }
  
  // ---------------------------------------------------------------------------
  //     hide
  // ---------------------------------------------------------------------------
  // Hide means that a point is faded and is placed on a lower z-level.
  
  void hide(){
    percentLabel->setColor("grey");
    timeLabel->setColor("grey");
    pen.setColor(QColor("lightskyblue3"));
    lineLeft->setPen(pen);
    PointWithText::setZ(1);
    lineLeft->setZ(0);
    percentLabel->setZ(0);
    timeLabel->setZ(0); 
  }
  
  // ---------------------------------------------------------------------------
  //     show
  // ---------------------------------------------------------------------------
  // Show means that a point has its real color and is placed on a higher z-level
  // compared it's hidden state.
  
  void show(){
    percentLabel->setColor("black");
    timeLabel->setColor("black");
    pen.setColor(QColor("blue"));
    lineLeft->setPen(pen);
    PointWithText::setZ(3);
    lineLeft->setZ(2);
    percentLabel->setZ(2);
    timeLabel->setZ(2); 
  }

  // ---------------------------------------------------------------------------
  //     setLeftLine
  // ---------------------------------------------------------------------------
  // Change the left line connecting this point to the closest left neighbour.
  
  void setLeftLine(double toX, double toY){
    lineLeft->setPoints(toX-1, toY+1, x()-1, y()+1);
    lineLeft->show();
  }
  
  // ---------------------------------------------------------------------------
  //     moveLeftLine
  // ---------------------------------------------------------------------------
  // Moves the left line connecting this point to the closest left neighbour. 
  // We cannot use setLeftLine for this purpous since it uses x()-1 y()-1 which will
  // continue subtracting the x and y value of the first end.
  
  void moveLeftLine(double toX, double toY){  
    lineLeft->setPoints(lineLeft->startPoint().x(), lineLeft->startPoint().y(), toX-1, toY+1);
    lineLeft->show();
  }
};
*/
