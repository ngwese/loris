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
#include "partialsList.h"

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
	PartialsList*	pList,
	QStatusBar*	status
):QDialog(parent, name, TRUE){
  statusbar = status;
  partialsList = pList;
  canvs = new QCanvas(735, 350);
  sample1 = "";
  sample2 = "";
  setGui();
  setConnections();
  setLists()
  show();
}

/*
--------------------------------------------------------------------------------
	setLists
--------------------------------------------------------------------------------
Initialize the popup lists which select samples to be dilated.
*/
void DilateDialog::setLists(){
  int i;
  int current;

  sample1List->clear();
  sample2List->clear();

  for(i=0; i<partialsList->getLength(); i++){
    sample1List->insertItem(partialsList->getPartials(i)0>getName(), i);
    sample2List->insertItem(partialsList->getPartials(i)0>getName(), i);
  }

  current = partialsList->getCurrentIndex();

  sample1List->setCurrentItem(current);
  sample2List->setCurrentItem(current);
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
  connect(sample1List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate1(int)));
  connect(sample2List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate2(int)));
}


/*
--------------------------------------------------------------------------------
	updateDilate1
--------------------------------------------------------------------------------
Selects a new sample for dilation, and updates all GUI elements in MorphDialog
to reflect the change.
*/
void DilateDialog::updateSample1(int pos){
  sample1 = sample1List->text(pos);
  name1Label->setText(sample1);
  dilateArea->setSample1(sample1);
  dilateBox->setTitle(QString("Dilate "+sample1+" onto "+sample2));
}


/*
--------------------------------------------------------------------------------
	updateDilate2
--------------------------------------------------------------------------------
Selects a new sample for dilation, and updates all GUI elements in MorphDialog
to reflect the change.
*/
void DilateDialog::updateSample2(int pos){
  sample2 = sample2List->text(pos);
  name2Label->setText(sample2);
  dilateArea->setSample2(sample2);
  dilateBox->setTitle(QString("Dilate "+sample1+" onto "+sample2));
}


/*
--------------------------------------------------------------------------------
	setGui
--------------------------------------------------------------------------------
Set up all the GUI elements.
*/
DilateDialog::setGui(){


}
