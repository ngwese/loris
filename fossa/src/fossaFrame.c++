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
 * fossaWindow.c++
 *
 * Frame containing the views, Sidebar and CurrentSoundView, of the application model.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H     // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include "fossaFrame.h"
#include "sidebar.h"
#include "currentSoundView.h"
#include "soundList.h"

#include <qlayout.h>

/*
---------------------------------------------------------------------------
	FossaFrame constructor
---------------------------------------------------------------------------
*/
FossaFrame::FossaFrame(
	QWidget*	parent,
	const char*	name,
	SoundList*	pList
):QFrame(parent, name){
  soundList = pList; 
  setGui();
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
Contains Sidebar and CurrentSoundView
*/
void FossaFrame::setGui(){
  fossaFrameLayout    = new QGridLayout(this); 
  sidebar             = new Sidebar(this, "Sidebar", soundList);
  currentSoundView = new CurrentSoundView(
	this,
	"currentSoundView",
	soundList
  );
   
  setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)3,
		(QSizePolicy::SizeType)3,
		sizePolicy().hasHeightForWidth()
	)
  );

  fossaFrameLayout->addWidget(sidebar, 0, 0 );
  fossaFrameLayout->addWidget(currentSoundView, 0, 1);
}
