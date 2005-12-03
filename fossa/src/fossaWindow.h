#ifndef FOSSA_WINDOW_H
#define FOSSA_WINDOW_H

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
 * fossaWindow.h++
 *
 * Definition of class FossaWindow, the main window of the application.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qmainwindow.h> 

class QGridLayout; 
class QStatusBar;
class QToolBar;   // might be used in later versions.
class QMenuBar;
class QPopupMenu;
class QApplication;

class FossaFrame;
class ImportDialog;
class ImportAiffDialog;
class ImportSdifDialog;
class ChannelizeDialog;
class NewNameDialog;
class MorphDialog;
class ExportDialog;
class ExportAiffDialog;
class ExportSdifDialog;
class PartialsList;
class Partials;
class LorisInterface;

// ---------------------------------------------------------------------------
// class FossaWindow
//
// The main window of the application, including gui element such as statusbar, 
// menubar, and fossaFrame. Also creates the non-gui models of the application, 
// LorisInterface and PartialsList. 
// When selecting menu options appropriate dialogs are opened or, when no 
// parameters need to be specified, fossaWindow communicates straight to 
// the partialsList.
//	

class FossaWindow:public QMainWindow{ 
  Q_OBJECT
    
 public:
  FossaWindow(QWidget* parent, const char* name);
  FossaWindow::~FossaWindow();

 public slots:
  void updateMenuOptions();  
 
 private slots:  // responds to mouse clicks in the menu.
  void openImportAiffDialog();
  void openImportSdifDialog();
  void openChannelizeDialog();
  void distill();  
  void openMorphDialog();
  void openExportAiffDialog();
  void openExportSdifDialog();
  void openNewNameDialog();
  void copy();
  void remove();
  void whatIsThis();
  void about();
  void manual();
        
 private:
  FossaFrame* fossaFrame;
  PartialsList* partialsList;
  ImportDialog* importDialog;
  ChannelizeDialog* channelizeDialog;
  NewNameDialog* newNameDialog;
  MorphDialog* morphDialog;
  ExportDialog* exportDialog;
  
  QGridLayout* fossaLayout; 
  QStatusBar* statusbar;
  //QToolBar* toolbar; //later versions might want a toolbar   
  QMenuBar* menubar;         // top menu
  QPopupMenu* fileMenu;      // in menubar
  QPopupMenu* importMenu;       // in fileMenu
  QPopupMenu* exportMenu;       // in fileMenu
  QPopupMenu* manipulateMenu;
  QPopupMenu* editMenu;      // in menubar
  QPopupMenu* helpMenu;      // in menubar

  int distillID;
  int channelizeID;
  int morphID;
  int exportID;
  int deleteId;
  int renameId; 
  int copyId;
  int playId;
 
  void setMenuBar();
  void setConnections();
  void addWhatIsThis();
  void addToolTips();
};

#endif // FOSSA_WINDOW_H







