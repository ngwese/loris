#ifndef SIDEBAR_H
#define SIDEBAR_H

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
 * Definition of class Sidebar.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qframe.h>
#include "soundList.h"

class SoundList;
class Player;

class QFrame;
class QGroupBox;
class QListBox;
class QBoxLayout;

// ---------------------------------------------------------------------------
// class Sidebar
//
// Sidebar provides a view over SoundList, the container class of all 
// collections of sound which has been imported or produced by manipulations. 
// Sidebar also has a player, which makes it possible for the user to audit current 
// sound, see class Player. 

class Sidebar:public QFrame{ 
  Q_OBJECT

  public:
    Sidebar(
	QWidget*	parent,
	const char*	name,
	SoundList*	pList
    );
 
  public slots:
    void updateSoundListView();  // updates the view over soundList

  private slots:
    void setCurrentSound(int);

  private:
    QBoxLayout*		sidebarLayout;
    QGroupBox*		playerGroup;
    QBoxLayout*		playerGroupLayout;
    QGroupBox*		soundListGroup;
    QBoxLayout*		soundListGroupLayout;
    QListBox*		soundListView;
    SoundList*	soundList;
    Player*		player;
  
    void		setGui();
    void		setConnections();
};
#endif // SIDEBAR_H
