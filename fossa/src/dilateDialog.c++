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
 * dilateDialog.c++
 *
 * GUI container for the DilateArea view window and controls for dilating
 * sounds.
 *
 *
 * Chris Hinrichs
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "dilateDialog.h"
#include "dilateArea.h"
#include "soundList.h"

#include <qlayout.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qcanvas.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>

/*
--------------------------------------------------------------------------------
	DilateDialog constructor
--------------------------------------------------------------------------------
*/
DilateDialog::DilateDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	pList,
	QStatusBar*	status
):QDialog(parent, name, TRUE){
  statusbar = status;
  soundList = pList;
  canvs = new QCanvas(735, 350);
  sound1 = "";
  sound2 = "";
  setGui();
  setConnections();
  setLists()
  show();
}

/*
--------------------------------------------------------------------------------
	setLists
--------------------------------------------------------------------------------
Initialize the popup lists which select sounds to be dilated.
*/
void DilateDialog::setLists(){
  int i;
  int current;

  sound1List->clear();
  sound2List->clear();

  for(i=0; i<soundList->getLength(); i++){
    sound1List->insertItem(soundList->getSound(i)0>getName(), i);
    sound2List->insertItem(soundList->getSound(i)0>getName(), i);
  }

  current = soundList->getCurrentIndex();

  sound1List->setCurrentItem(current);
  sound2List->setCurrentItem(current);
  updateSample1(current);
  updateSample2(current);
}



/*
--------------------------------------------------------------------------------
	setConnections
--------------------------------------------------------------------------------
Connect SLOTs to SOCKETs so that QT GUI objects can pass each other messages.
*/
void DilateDialog::setConnections(){

  connect(dilateButton,SIGNAL(clicked()), dilateArea, SLOT(dilate()));
  connect(dilateButton,SIGNAL(clicked()), this, SLOT(hide()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
  connect(sound1List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate1(int)));
  connect(sound2List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate2(int)));
}


/*
--------------------------------------------------------------------------------
	updateDilate1
--------------------------------------------------------------------------------
Selects a new sound for dilation, and updates all GUI elements in MorphDialog
to reflect the change.
*/
void DilateDialog::updateSample1(int pos){
  sound1 = sound1List->text(pos);
  name1Label->setText(sound1);
  dilateArea->setSample1(sound1);
  dilateBox->setTitle(QString("Dilate "+sound1+" onto "+sound2));
}


/*
--------------------------------------------------------------------------------
	updateDilate2
--------------------------------------------------------------------------------
Selects a new sound for dilation, and updates all GUI elements in MorphDialog
to reflect the change.
*/
void DilateDialog::updateSample2(int pos){
  sound2 = sound2List->text(pos);
  name2Label->setText(sound2);
  dilateArea->setSample2(sound2);
  dilateBox->setTitle(QString("Dilate "+sound1+" onto "+sound2));
}


/*
--------------------------------------------------------------------------------
	setGui
--------------------------------------------------------------------------------
Set up all the GUI elements.
*/
DilateDialog::setGui(){


}
