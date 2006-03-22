#ifndef CURRENT_SOUND_VIEW_H
#define CURRENT_SOUND_VIEW_H

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
 * currentSoundView.h
 *
 * Definition of class CurrentSoundView, containing the views of the 
 * application model.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qframe.h> 
#include <qcanvas.h>

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
class SoundList;
class SoundPlot;

// ---------------------------------------------------------------------------
// class CurrentSoundView
//
// The view has three tabs representing the amplitude, frequency, and noise
// plots for the current collection of partials in the soundList. Whenever the
// current sound changes drawCurrent() is called and the plots are updated.
class CurrentSoundView:public QFrame{
  Q_OBJECT

  public:
    CurrentSoundView(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList
    );
 
  public slots:
    void		drawCurrent();
  
  private:
    QGridLayout*	layout;
    QTabWidget*		tab;
    SoundList*		soundList;
    Tab*		amplitudeTab;
    Tab*		frequencyTab;
    Tab*		noiseTab;
  
    void		setGui();
    void		setConnections();
};

// ---------------------------------------------------------------------------
// class Tab
//
// A tab has usual widgets for displaying information about current collection
// of partials in the soundList. It also shows the current SoundPlot, a plot of
// a sound's parameter envelopes against time.
class Tab:public QWidget{
  Q_OBJECT

  public:
    enum        	TabType{amplitude, frequency, noise, empty};

    Tab(
	QWidget*	parent,
	char*		name,
	SoundList*	pList,
	TabType		t
    );

    void update();
  
  private:
    QVBoxLayout*	tabLayout;
    QGroupBox*		box;
    QGridLayout*	boxLayout;
    QFrame*		infoBox;
    QLabel*		stateText;
    QLabel*		nrOfPartialsText;
    QLabel*		durationText;
    QPushButton*	okPushButton;
    QGridLayout*	infoBoxLayout;

    TabType		type;
  
    void setGui();
  
  private slots:
    void		shiftValues();

  protected:
    QLabel*		maxNumber;
    QLineEdit*		shiftValue;
    QLabel*		shiftText;
    QLabel*		maxText;
    QCanvas*		canvas;
    SoundPlot*		partialsView;
    SoundList*		soundList;
};

#endif // CURRENT_SOUND_VIEW_H
