#ifndef MORPH_DIALOG_H
#define MORPH_DIALOG_H

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
 * morphDialog.h
 *
 * Definition of the MorphDialog class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qdialog.h>

class QGridLayout; 
class QFrame;
class QGroupBox;
class QButtonGroup;
class QPushButton;
class QRadioButton;
class QStatusBar;
class QComboBox;
class QLabel;
class QCanvas;

class PartialsList;
class MorphArea;

// ---------------------------------------------------------------------------
// class MorphDialog
// Provides a dialog for performing a morph between two sounds. The two 
// collections of partials are selected from listboxes and are morphed 
// together according to breakpoints specified by user mouse clicks on the
// morphArea.

class MorphDialog:public QDialog{ 
 
  Q_OBJECT

 public:
  int i;
  MorphDialog(QWidget* parent, char* name, PartialsList* pList, QStatusBar* statusbar);
 
 private slots:
  void updateMorph1(int pos);
  void updateMorph2(int pos);

 private:
  QCanvas* canvas;
  QStatusBar* statusbar;
  PartialsList* partialsList;
  MorphArea* morphArea;        // This is the area where breakpoints
  QGridLayout* dialogLayout;   // for a morph gets specified.
  QGroupBox* morphBox;
  QGridLayout* morphBoxLayout;
  QGroupBox* partialsBox;
  QGridLayout* partialsBoxLayout; 
  QLabel* partial1Label;
  QComboBox* partial2List;
  QComboBox* partial1List;
  QLabel* partial2Label;
  QLabel* name1Label;
  QLabel* name2Label;
  QPushButton* morphButton;

  QButtonGroup* onOffBox;
  QGridLayout* onOffBoxLayout;
  QFrame* line;
  QRadioButton* allButton;
  QRadioButton* amplitudeButton;
  QRadioButton* frequencyButton;
  QRadioButton* noiseButton;
  
  QGridLayout* morphSideLayout;
  QPushButton* clearNoiseButton;
  QPushButton* clearFreqButton;
  QPushButton* clearAmpButton;
  QPushButton* clearAllButton;
  QPushButton* cancelButton;
  
  QString morph1;
  QString morph2;
  
  void setConnections();
  void setLists();
  void setGui();
};
#endif // MORPH_DIALOG_H








