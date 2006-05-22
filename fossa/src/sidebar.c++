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
 * The Sidebar class provides a view over SoundList, the container class 
 * of all collections of sound which has been imported or produced by 
 * manipulations. Sidebar also has a player which makes it possible for the user
 * to audit current sound, see class Player. 
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

/*
---------------------------------------------------------------------------
	Sidebar constructor
---------------------------------------------------------------------------
*/
Sidebar::Sidebar(
	QWidget*	parent, 
	const char*	name, 
	SoundList*	pList
):QFrame(parent, name){
  soundList = pList;

  setGui();
  setConnections();
}

/*
---------------------------------------------------------------------------
	setCurrentSound
---------------------------------------------------------------------------
When a user clicks on an item in the list, current sound are changed in
soundList;
*/
void Sidebar::setCurrentSound(int pos){
  soundList->setCurrentSound(pos);
}

/*
---------------------------------------------------------------------------
	updateSoundListView
---------------------------------------------------------------------------
Every time the soundList is changed the soundListView gets updated.
*/
void Sidebar::updateSoundListView(){
  soundListView->clear();
  int listLength = soundList->getLength();
  if(listLength>0){
    for(int i = 0; i<listLength ; i++){
      const Sound* p = soundList->getSound(i);
      soundListView->insertItem(p->getName());
    }

    soundListView->setCurrentItem(soundList->getCurrentIndex() - 1);
  }
}

/*
---------------------------------------------------------------------------
	setConnections
---------------------------------------------------------------------------
There are 2 directions here - the first call sets it up so that if the
user clicks on a new sound in the sidebar it tells the soundList (main
model class) to set a new current sound. The second sets it up the other
way, so that if the soundList changes current, then the sidebar updates.
*/
void Sidebar::setConnections(){
  connect(soundListView, SIGNAL(highlighted(int)), 
	  this, SLOT(setCurrentSound(int)));    
  connect(soundList, SIGNAL(listChanged()),
	  this, SLOT(updateSoundListView()));
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
*/
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
  
  player = new Player(playerGroup, "player", soundList);
  QFont player_font(player->font() );
  player_font.setPointSize( 12 );
  player->setFont(player_font );
  player->show();

  playerGroupLayout->addWidget(player);
  sidebarLayout->addWidget(playerGroup);
  
  soundListGroup = new QGroupBox(this, "soundListGroup");
  soundListGroup->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)7, 
		soundListGroup->sizePolicy().hasHeightForWidth()
	)
  );
  QFont soundListGroup_font(soundListGroup->font() );
  soundListGroup_font.setFamily( "helvetica" );
  soundListGroup_font.setPointSize( 12 );
  soundListGroup->setFont( soundListGroup_font ); 
  soundListGroup->setTitle( tr( "Currently loaded sounds" ) );
  soundListGroup->setColumnLayout(0, Qt::Vertical );
  soundListGroup->layout()->setSpacing( 0 );
  soundListGroup->layout()->setMargin( 0 );
  soundListGroupLayout = new QVBoxLayout( soundListGroup->layout() );
  soundListGroupLayout->setAlignment( Qt::AlignTop );
  soundListGroupLayout->setSpacing( 6 );
  soundListGroupLayout->setMargin( 11 );

  soundListView = new QListBox(soundListGroup,"soundListView");
  soundListView->setSelectionMode(QListBox::Single);
  soundListGroupLayout->addWidget(soundListView);
  sidebarLayout->addWidget(soundListGroup);
}
