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
 * The main window of Fossa, including gui element such as statusbar, menubar, 
 * and fossaFrame. Also creates the models of the application, LorisInterface 
 * and SoundList which are non-gui components. 
 * When selecting menu options appropriate dialogs are opened or, when no 
 * parameters need to be specified, fossaWindow communicates straight to 
 * the soundList.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H   // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include "fossaWindow.h"
#include "fossaFrame.h"
#include "importDialog.h"
#include "exportDialog.h"
#include "morphDialog.h"
#include "dilateDialog.h"
#include "channelizeDialog.h"
#include "newNameDialog.h"
#include "soundList.h"
#include "sound.h"
#include "lorisInterface.h"

#include <qlayout.h> 
#include <qapplication.h>
#include <qtoolbar.h>    // future versions might want a toolbar. 
#include <qstatusbar.h>
#include <qmenubar.h> 
#include <qpopupmenu.h> 
#include <qwhatsthis.h>
#include <qtooltip.h>

// ---------------------------------------------------------------------------
//	FossaWindow constructor
// ---------------------------------------------------------------------------
FossaWindow::FossaWindow(
	QWidget*	parent,
	const char*	name
):QMainWindow(parent,name){
  soundList		= new SoundList();
  fossaFrame		= new FossaFrame(this, "FossaFrame", soundList);
  //  toolbar		= new QToolBar("test", this);	// later versions might want 
  menubar		= menuBar(); 			// to implement a toolbar.
  statusbar		= statusBar();

  importDialog		= 0;
  channelizeDialog	= 0;
  newNameDialog		= 0;
  morphDialog		= 0;
  exportDialog		= 0;
  // later versions might want to implement a toolbar.
  // QWhatsThis::whatsThisButton(toolbar);

  setCaption("Fossa");
  setCentralWidget(fossaFrame);
  setMenuBar(); 
  setConnections();
  addWhatIsThis();
  addToolTips();
  QToolTip::setEnabled(true); 
}

// ---------------------------------------------------------------------------
//	FossaWindow destructor
// ---------------------------------------------------------------------------
FossaWindow::~FossaWindow(){
  delete soundList;
}

// ---------------------------------------------------------------------------
//	 updateMenuOptions. 
// ---------------------------------------------------------------------------
// SLOT which enables and disables menu options. 
void FossaWindow::updateMenuOptions(){
  // If we have sound we should be able to do operation - enable options 
  if(!soundList->isEmpty()){  
    fileMenu->setItemEnabled(exportID    , TRUE);
    editMenu->setItemEnabled(deleteId    , TRUE);
    editMenu->setItemEnabled(renameId    , TRUE);
    editMenu->setItemEnabled(copyId      , TRUE);
    manipulateMenu->setItemEnabled(channelizeID, TRUE);
    manipulateMenu->setItemEnabled(distillID   , TRUE);
  }

  // Disables menu options if we don't have any sound is soundList 
  else{
    fileMenu->setItemEnabled(exportID    , FALSE);
    editMenu->setItemEnabled(deleteId    , FALSE);
    editMenu->setItemEnabled(renameId    , FALSE);
    editMenu->setItemEnabled(copyId      , FALSE);
    manipulateMenu->setItemEnabled(channelizeID, FALSE);
    manipulateMenu->setItemEnabled(distillID   , FALSE);
  }

  //There must be 2 or more sounds in order to dilate.
  if( soundList->getLength() == 1 ){
    manipulateMenu->setItemEnabled(dilateID, FALSE);
  }else{
    manipulateMenu->setItemEnabled(dilateID, TRUE);
  }

  //not ok if less than 2 sounds in soundList are channelized & distilled
  int okToMorph = 0;
 
  for(int i = 0; i<soundList->getLength(); i++){
    const Sound* p = soundList->getSound(i);
    if (p->isDistilled() && p->isChannelized()){
      okToMorph++;
    }
  }
  
  if(okToMorph >= 2){
    manipulateMenu->setItemEnabled(morphID, TRUE);
  }else{
    manipulateMenu->setItemEnabled(morphID, FALSE);
  }
}

// ---------------------------------------------------------------------------
//	setConnections. 
// ---------------------------------------------------------------------------
// Menu options are updated when soundList is changed.
void FossaWindow::setConnections(){
  connect(soundList, SIGNAL(listChanged()), this , SLOT(updateMenuOptions()));
  connect(soundList, SIGNAL(currentChanged()), this , SLOT(updateMenuOptions()));
}

// ---------------------------------------------------------------------------
//     setMenuBar
// ---------------------------------------------------------------------------
//  sets menus and menu options in the menuBar. 
void FossaWindow::setMenuBar(){
  fileMenu       = new QPopupMenu(this);
  importMenu     = new QPopupMenu(fileMenu);
  exportMenu     = new QPopupMenu(fileMenu);
  editMenu       = new QPopupMenu(this);
  manipulateMenu = new QPopupMenu(this);
  helpMenu       = new QPopupMenu(this);

  // options for import menu in file menu.
  importMenu->insertItem(
	"Import Ai&ff",
	this,
	SLOT(openImportAiffDialog()),
	CTRL+Key_F
  );

  importMenu->insertItem(
	"&Import Sdif",
	this,
	SLOT(openImportSdifDialog()),
	CTRL+Key_S
  );
  
  // options for export menu in file menu.
  exportMenu->insertItem(
	"&Export Aiff",
	this,
	SLOT(openExportAiffDialog()),
	CTRL+Key_A
  );

  exportMenu->insertItem(
	"&Export Sdif",
	this,
	SLOT(openExportSdifDialog()),
	CTRL+Key_S
  );
  
  // insert import and export menu in file menu.
  fileMenu->insertItem("Import", importMenu);
  exportID = fileMenu->insertItem("&Export", exportMenu);
  fileMenu->insertSeparator(5);
  fileMenu->insertItem("E&xit",qApp, SLOT(quit()),CTRL+Key_X);
  
  // options for edit menu.
  deleteId = editMenu->insertItem(
	"&Delete",
	this,
	SLOT(remove()),
	CTRL+Key_D);
  renameId = editMenu->insertItem(
	"&Rename",
	this,
	SLOT(openNewNameDialog()),
	CTRL+Key_R
  );

  copyId   = editMenu->insertItem(
	"&Copy",
	this,
	SLOT(copy()),
	CTRL+Key_C
  );
  
  // options for manipulate menu.
  channelizeID = manipulateMenu->insertItem(
	"&Channelize",
	this,
	SLOT(openChannelizeDialog()),
	CTRL+Key_H
  );

  distillID = manipulateMenu->insertItem(
	"&Distill",
	this,
	SLOT(distill()),
	CTRL+Key_D
  );

  morphID = manipulateMenu->insertItem(
	"&Morph",
	this,
	SLOT(openMorphDialog()),
	CTRL+Key_M
  );

  dilateID = manipulateMenu->insertItem(
	"Dilate",
	this,
	SLOT(openDilateDialog()),
	CTRL+Key_L
  );
  
  // options for help menu.
  helpMenu->insertItem(
	"&What is this",
	this,
	SLOT(whatIsThis()),
	CTRL+Key_W
  );

  helpMenu->insertItem(
	"&Manual",
	this,
	SLOT(manual()),
	CTRL+Key_M
  );

  helpMenu->insertItem(
	"&About Fossa",
	this,
	SLOT(about()), CTRL+Key_A
  );
  
  // insert menus in menubar
  menubar->insertItem("File"        , fileMenu);
  menubar->insertItem("Edit"        , editMenu);
  menubar->insertItem("Manipulate"  , manipulateMenu);
  menubar->insertItem("Help"        , helpMenu);
  
  updateMenuOptions();
}

// ---------------------------------------------------------------------------
//      addWhatIsThis
// ---------------------------------------------------------------------------
// Adds whatIsThis messages to gui elements, visible when WhatIsThisMode is
// entered.
void FossaWindow::addWhatIsThis(){
  QWhatsThis::add(statusbar, "I tell you what is going on");
}

// ---------------------------------------------------------------------------
//      addToolTips
// ---------------------------------------------------------------------------
// Adds addToolTips messages to gui elements, visible when tooltip is enabled.
void FossaWindow::addToolTips(){
  QToolTip::add(statusbar, "statusbar");
}

// ---------------------------------------------------------------------------
//      openImportAiffDialog
// ---------------------------------------------------------------------------
//  opens the import dialog for aiff file format
void FossaWindow::openImportAiffDialog(){
  if(importDialog) delete importDialog; 

  importDialog = new ImportAiffDialog(
	this,
	"importAiffDialog",
	soundList,
	statusbar
  );
}

// ---------------------------------------------------------------------------
//      openImportSdifDialog
// ---------------------------------------------------------------------------
//  opens the import dialog for sdif file format
void FossaWindow::openImportSdifDialog(){
  if(importDialog) delete importDialog; 

  importDialog = new ImportSdifDialog(
	this,
	"importSdifDialog",
	soundList,
	statusbar
  );
}

// ---------------------------------------------------------------------------
//      openChannelizeDialog
// ---------------------------------------------------------------------------
//  opens the dialog for performing channelization of current sound
void FossaWindow::openChannelizeDialog(){
  if(!soundList->isEmpty()){
    if(channelizeDialog) channelizeDialog->show();

    channelizeDialog = new ChannelizeDialog(
	this,
	"channelizeDialog",
	soundList,
	statusbar
    );
  }
}

// ---------------------------------------------------------------------------
//      distill
// ---------------------------------------------------------------------------
//  distill current sound. No parameters for distillation - no dialog is needed
void FossaWindow::distill(){ 
  try{
    soundList->distillCurrent();
    statusbar->message("Distilled sound successfully.", 5000);
  }
  catch(...){
    statusbar->message("Sound could not be distilled.");
  }
}

// ---------------------------------------------------------------------------
//      openMorphDialog
// ---------------------------------------------------------------------------
//  open dialog for performing a sound morph.
void FossaWindow::openMorphDialog(){
  if(!soundList->isEmpty()){
    if(morphDialog) delete morphDialog;

    morphDialog = new MorphDialog(
	this,
	"morphDialog",
	soundList,
	statusbar
    );
  }
}

// ---------------------------------------------------------------------------
//      openDilateDialog
// ---------------------------------------------------------------------------
//  opens a dialog for dilating one sound onto another.
void FossaWindow::openDilateDialog(){
/*
  if(!soundList->isEmpty()){
    if(dilateDialog) delete dilateDialog;

    dilateDialog = new DilateDialog(
	this,
	"dilateDialog",
	soundList,
	statusbar
    );
  }
*/
}

// ---------------------------------------------------------------------------
//      openExportAiffDialog
// ---------------------------------------------------------------------------
//  opens the dialog for exporting current sound to aiff file
void FossaWindow::openExportAiffDialog(){
  if(!soundList->isEmpty()){
    if(exportDialog) delete exportDialog;

    exportDialog = new ExportAiffDialog(
	this,
	"exportDialog",
	soundList,
	statusbar
    );
  }
}

// ---------------------------------------------------------------------------
//      openExportSdifDialog
// ---------------------------------------------------------------------------
//  opens the dialog for exporting current sound to sdif file
void FossaWindow::openExportSdifDialog(){
  if(!soundList->isEmpty()){
    if(exportDialog) delete exportDialog;

    exportDialog = new ExportSdifDialog(
	this,
	"exportDialog",
	soundList,
	statusbar
    );
  }
}

// ---------------------------------------------------------------------------
//      openNewNameDialog
// ---------------------------------------------------------------------------
//  Dialog for renaming current sound
void FossaWindow::openNewNameDialog(){
  if(!soundList->isEmpty()){
    if(newNameDialog) delete newNameDialog;

    newNameDialog = new NewNameDialog(
	this,
	"exportDialog",
	soundList,
	statusbar
    );
  }
}

// ---------------------------------------------------------------------------
//      copy
// ---------------------------------------------------------------------------
// copy current sound - no dialog is needed
void FossaWindow::copy(){
  soundList->copyCurrent();
}

// ---------------------------------------------------------------------------
//      remove
// ---------------------------------------------------------------------------
// remove current sound - no dialog is needed
void FossaWindow::remove(){
  soundList->removeCurrent();
}

// ---------------------------------------------------------------------------
//      whatIsThis
// ---------------------------------------------------------------------------
// Enters WhatsThisMode which shows descriptions of gui elements when clicked on.
void FossaWindow::whatIsThis(){
  QWhatsThis::enterWhatsThisMode();
}


// ---------------------------------------------------------------------------
//      about - NOT IMPLEMENTED YET!
// ---------------------------------------------------------------------------
// Opens the about window for Fossa.
void FossaWindow::about(){
}

// ---------------------------------------------------------------------------
//      manual - NOT IMPLEMENTED YET!
// ---------------------------------------------------------------------------
// Opens the manual of Fossa.
void FossaWindow::manual(){ }
