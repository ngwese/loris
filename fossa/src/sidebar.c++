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
 * sidebar.c++
 *
 * The Sidebar class provides a view over PartialsList, the container class 
 * of all collections of partials which has been imported or produced by 
 * manipulations. Sidebar also has a player which makes it possible for the user
 * to audit current partials, see class Player. 
 * 
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H     // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include "sidebar.h"
#include "player.h"

#include <qlayout.h>
#include <qlistbox.h>
#include <qgroupbox.h>

// ---------------------------------------------------------------------------
//	Sidebar constructor
// ---------------------------------------------------------------------------

Sidebar::Sidebar(
	QWidget*	parent, 
	const char*	name, 
	PartialsList*	pList
	):QFrame(parent, name){
 
  partialsList = pList;
    
  setGui();
  setConnections();
}

// ---------------------------------------------------------------------------
//	updatePartialsListView
// ---------------------------------------------------------------------------
// Every time the partialsList is changed the partialsListView gets updated.

void Sidebar::updatePartialsListView(){
  partialsListView->clear();
  int listLength = partialsList->getLength();
  if(listLength>0){
    for(int i = 0; i<listLength ; i++){
      const Partials* p = partialsList->getPartials(i);
      partialsListView->insertItem(p->getName());
    }
    partialsListView->setCurrentItem(partialsList->getCurrentIndex());
  }
}

// ---------------------------------------------------------------------------
//	setConnections
// ---------------------------------------------------------------------------

void Sidebar::setConnections(){
  connect(partialsListView, SIGNAL(highlighted(int)), 
	  this, SLOT(setCurrentPartials(int)));    
  connect(partialsList, SIGNAL(listChanged()),// When partialsList is changed 
	  this, SLOT(updatePartialsListView()));    // the view gets updated.
}

// ---------------------------------------------------------------------------
//	setCurrentPartials
// ---------------------------------------------------------------------------
// When a user clicks on an item in the list, current partials are changed in
// partialsList;

void Sidebar::setCurrentPartials(int pos){
  partialsList->setCurrentPartials(pos);
}

// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------

void Sidebar::setGui(){
  setFrameShape(QFrame::WinPanel);
  setFrameShadow(QFrame::Raised);
  sidebarLayout = new QVBoxLayout(this); 
  sidebarLayout->setSpacing(6);
  sidebarLayout->setMargin(11);

  playerGroup = new QGroupBox(this,"playerGroup");
  playerGroup->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)1,
		playerGroup->sizePolicy().hasHeightForWidth()
	)
  );
  
  QFont playerGroup_font(playerGroup->font());
  playerGroup_font.setFamily( "helvetica" );
  playerGroup_font.setPointSize( 12 );
  playerGroup->setFont( playerGroup_font ); 
  playerGroup->setColumnLayout(0, Qt::Vertical );
  playerGroup->layout()->setSpacing( 0 );
  playerGroup->layout()->setMargin( 0 );
  playerGroupLayout = new QHBoxLayout(playerGroup->layout());
  playerGroupLayout->setAlignment( Qt::AlignTop );
  playerGroupLayout->setSpacing( 6 );
  playerGroupLayout->setMargin( 11 );
  
  player = new Player(playerGroup, "player", partialsList);
  QFont player_font(player->font() );
  player_font.setPointSize( 12 );
  player->setFont(player_font );
  player->show();

  playerGroupLayout->addWidget(player);
  sidebarLayout->addWidget(playerGroup);
  
  partialsListGroup = new QGroupBox(this, "partialsListGroup");
  partialsListGroup->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)7, 
		partialsListGroup->sizePolicy().hasHeightForWidth()
	)
  );
  QFont partialsListGroup_font(partialsListGroup->font() );
  partialsListGroup_font.setFamily( "helvetica" );
  partialsListGroup_font.setPointSize( 12 );
  partialsListGroup->setFont( partialsListGroup_font ); 
  partialsListGroup->setTitle( tr( "List of partials" ) );
  partialsListGroup->setColumnLayout(0, Qt::Vertical );
  partialsListGroup->layout()->setSpacing( 0 );
  partialsListGroup->layout()->setMargin( 0 );
  partialsListGroupLayout = new QVBoxLayout( partialsListGroup->layout() );
  partialsListGroupLayout->setAlignment( Qt::AlignTop );
  partialsListGroupLayout->setSpacing( 6 );
  partialsListGroupLayout->setMargin( 11 );

  partialsListView = new QListBox(partialsListGroup,"partialsListView");
  partialsListGroupLayout->addWidget(partialsListView);
  sidebarLayout->addWidget(partialsListGroup);
}
