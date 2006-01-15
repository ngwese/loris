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
 * player.c++
 *
 * When clicking on the play button current partials are synthesized and played
 * 
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "player.h"

#include <qlabel.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


// ---------------------------------------------------------------------------
//	Player constructor
// ---------------------------------------------------------------------------

Player::Player(
	QWidget*	parent,
	const char*	name,
	PartialsList*	pList
):QWidget(parent, name){
  partialsList = pList;
  
  setGui();
  setConnections();
}

// ---------------------------------------------------------------------------
//	setConnections
// ---------------------------------------------------------------------------

void Player::setConnections(){
  connect(playButton, SIGNAL(clicked()), this , SLOT(play()));
}

// ---------------------------------------------------------------------------
//	play
// ---------------------------------------------------------------------------
// Plays current partials in partialsList

void Player::play(){
  partialsList->playCurrent();
}

// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------

void Player::setGui(){
  resize( 188, 242 ); 
  setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)5,
		(QSizePolicy::SizeType)0,
		sizePolicy().hasHeightForWidth()
	)
  );
  
  playerLayout = new QGridLayout( this ); 
  playerLayout->setSpacing( 6 );
  playerLayout->setMargin(0);
  
  /***************** Not implemented yet *************************************/

  playTimeText = new QLabel( this, "playTimeText" );   
  playTimeText->setText( tr( "play time:" ) );

  playerLayout->addMultiCellWidget( playTimeText, 0, 0, 0, 1 );

  progressBar = new QProgressBar( this, "progressBar" );  
  progressBar->setFrameShadow( QProgressBar::Sunken );

  playerLayout->addMultiCellWidget( progressBar, 1, 1, 0, 1 );

  stopButton = new QPushButton( this, "stopButton" );
  stopButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		stopButton->sizePolicy().hasHeightForWidth()
	)
  );
  stopButton->setText( tr( "stop" ) );

  playerLayout->addWidget( stopButton, 2, 0 );

  /****************************************************************************/

  playButton = new QPushButton( this, "playButton" );
  playButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		playButton->sizePolicy().hasHeightForWidth()
	)
  );
  playButton->setText( tr( "play" ) );

  playerLayout->addWidget( playButton, 2, 1 );
}
