#ifndef PLAYER_H
#define PLAYER_H

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
 * player.h++
 *
 * Definition of class Player used for auditing current partials. Contains 
 * unimplemented parts.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qwidget.h>
#include "partialsList.h"

class QGridLayout; 
class QLabel;
class QProgressBar;
class QPushButton;

// ---------------------------------------------------------------------------
// class Player
//
// Used for auditing current partials. Stop button, play time, and the progessbar
// is not implemented yet.

class Player : public QWidget{ 
  Q_OBJECT

  public:
    Player(
	QWidget*	parent,
	const char*	name,
	PartialsList*	partialsList
    );
    
  private slots:
    void play();

  private:
    QLabel*		playTimeText;		// not implemented
    QProgressBar*	progressBar;		// not implemented
    QLabel*		pbParText;
    QPushButton*	stopButton;		// not implemented
    QPushButton*	playButton;
    QGridLayout*	playerLayout;
    
    PartialsList*	partialsList;
    
    void		setGui();
    void		setConnections();
};

#endif // PLAYER_H
