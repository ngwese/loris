/*
 * This is Fossa, a grapical control application for analysis, synthesis, 
 * and manipulations of digitized sounds using Loris (Fitz and Haken). 
 *
 * Fossa is Copyright (c) 2001-2 by Susanne Lefvert, 2003 by Donour Sizemore
 *
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
 * fossaWindow.c++
 *
 * Frame containing the views, Sidebar and CurrentPartialsView, of the application model.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 */

#if HAVE_CONFIG_H     // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include "fossaFrame.h"
#include "sidebar.h"
#include "currentPartialsView.h"
#include "partialsList.h"

#include <qlayout.h>

// ---------------------------------------------------------------------------
//	FossaFrame constructor
// ---------------------------------------------------------------------------

FossaFrame::FossaFrame(QWidget* parent, const char* name, PartialsList* pList):QFrame(parent, name){
  partialsList = pList; 
  setGui();
}

// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------
// Contains Sidebar and CurrentPartialsView

void FossaFrame::setGui(){
  split               = new QSplitter(this);
  fossaFrameLayout    = new QGridLayout(this); 
  sidebar             = new Sidebar(split, "Sidebar", partialsList);
  currentPartialsView = new CurrentPartialsView(split, "currentPartialsView", partialsList);
   
  setSizePolicy(QSizePolicy((QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, sizePolicy().hasHeightForWidth()));
  fossaFrameLayout->addWidget(split, 0, 0 );
    
}







