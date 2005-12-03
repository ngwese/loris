#ifndef CURRENT_PARTIALS_VIEW_H
#define CURRENT_PARTIALS_VIEW_H

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
 * currentPartialsView.h
 *
 * Definition of class CurrentPartialsView, containing the views of the 
 * application model.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qframe.h> 

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QGroupBox;
class QButtonGroup;
class QCheckBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QStatusBar;
class QMultiLineEdit;
class QTabWidget;
class QSpinBox;
class QLineEdit;

class Tab;
class PartialsList;

// ---------------------------------------------------------------------------
// class CurrentPartialsView
//
// The view has three tabs representing the amplitude, frequency, and noise
// plots for current collection of partials in the partialsList.  Whenever the
// current partials changes drawCurrent() is called and the plots will get updated.
//	

class CurrentPartialsView:public QFrame{ 
 
  Q_OBJECT

 public:
  CurrentPartialsView(QWidget* parent, char* name,  PartialsList* partialsList);
 
  public slots:
    void drawCurrent();
  
  private slots:
  void stateChanged();

 private:
  QGridLayout* layout;
  QTabWidget* tab;
  PartialsList* partialsList;
  Tab* amplitudeTab;
  Tab* frequencyTab;
  Tab* noiseTab;
  
  void setGui();
  void setConnections();
};

// ---------------------------------------------------------------------------
// class Tab
//
// A tab has usual widgets for displaying information about current collection of 
// partials in the partialsList. It also shows current partials pixmap, a plot of a partials
// parameter envelopes against time.
//	
class Tab:public QWidget{
 Q_OBJECT

 public:
  Tab(QWidget* parent, char* name, PartialsList* partialsList);
  virtual void update();
  
 private:
  QVBoxLayout* tabLayout;
  QGroupBox* box;
  QGridLayout* boxLayout;
  QFrame* infoBox;
  QLabel* stateText;
  QLabel* nrOfPartialsText;
  QLabel* durationText;
  QPushButton* okPushButton;
  QGridLayout* infoBoxLayout;
  
  void setGui();
  
 private slots:
   virtual void shiftValues() = 0;

 protected:
  QLabel* maxNumber;
  QLineEdit* shiftValue;
  QLabel* shiftText; 
  QLabel* maxText;
  QLabel* partialsView;
  PartialsList* partialsList;
};

// ---------------------------------------------------------------------------
// class AmplitudeTab
//
// Inherits Tab and acts as a display for current partial's amplitude 
// parameter envelopes against time. 
//
class AmplitudeTab:public Tab{
  
 public:
  AmplitudeTab(QWidget* parent, char* name, PartialsList* partialsList);
  void update();
  void shiftValues();
};

// ---------------------------------------------------------------------------
// class FrequencyTab
//
// Inherits Tab and acts as a display for current partial's frequency
// parameter envelopes against time. 
//
class FrequencyTab:public Tab{
  
 public:
  FrequencyTab(QWidget* parent, char* name, PartialsList* partialsList);
  void update();
  void shiftValues();
};

// ---------------------------------------------------------------------------
// class NoiseTab
//
// Inherits Tab and acts as a display for current partial's noise
// parameter envelopes against time. 
//
class NoiseTab:public Tab{
  
 public:
  NoiseTab(QWidget* parent, char* name, PartialsList* partialsList);
  void update();
  void shiftValues();
};

#endif // CURRENT_PARTIALS_VIEW_
