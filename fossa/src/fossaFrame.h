#ifndef FOSSA_FRAME_H
#define FOSSA_FRAME_H

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
 * fossaFrame.h++
 *
 * Definition of class FossaFrame, the main view of the partialsList model, 
 * including Sidebar and CurrentPartialsView.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qframe.h> 

class QGridLayout;
class Sidebar;
class CurrentPartialsView;
class PartialsList;

// ---------------------------------------------------------------------------
// class FossaFrame
//
// Contains the views, Sidebar and CurrentPartialsView, of the application model.
//	
class FossaFrame:public QFrame{ 
  Q_OBJECT

  public:
    FossaFrame(
	QWidget*		parent,
	const char*		name,
	PartialsList*		partialsList
    );
    
  private:
    QGridLayout*		fossaFrameLayout;
    Sidebar*			sidebar;
    PartialsList*		partialsList;
    CurrentPartialsView*	currentPartialsView;

    void			setGui();
};

#endif // FOSSA_FRAME_H
