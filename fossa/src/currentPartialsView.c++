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
 * currentPartialsView.c++, Tab.c++ , 
 * AmplitudeTab.c++ , FrequencyTab.c++, NoiseTab.c++
 *
 * class CurrentPartialsView has three tabs representing the amplitude, 
 * frequency, and noise plots for current collection of partials in the 
 * partialsList.  Whenever current partials changes drawCurrent() is called 
 * and the plots will get updated.
 *
 * class Tab acts as a virtual base class for AmplitudeTab, FrequencyTab, and NoiseTab.
 * They display current partial's envelopes (amplitude, frequency, and noise) 
 * as well as information about current partials.   
 *
 * Susanne Lefvert, 1 March 2002
 * 
 *
 
 */

#if HAVE_CONFIG_H       // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include <iostream>

#include "currentPartialsView.h"
#include "partialsList.h"

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qspinbox.h> 
#include <qlineedit.h> 
#include <qstring.h>

using std::cout;

// ---------------------------------------------------------------------------
//	CurrentPartialsView constructor
// ---------------------------------------------------------------------------

CurrentPartialsView::CurrentPartialsView(QWidget* parent, char* name, PartialsList* list):QFrame(parent,name){
  partialsList = list;
  setGui(); 
  setConnections();
}

// ---------------------------------------------------------------------------
//	drawCurrent
// ---------------------------------------------------------------------------
// Update tabs and set correct tab to be selected and visible, happens everytime
// current partials changes in partialsList

void CurrentPartialsView::drawCurrent(){  
  int currentState = tab->currentPageIndex();
  int newState     = partialsList->getCurrentState();
  
  amplitudeTab -> update();    
  frequencyTab -> update();
  noiseTab     -> update();
  
  if(currentState != newState){  
    tab->setCurrentPage(newState); 
  }
}

// ---------------------------------------------------------------------------
//	setConnections
// ---------------------------------------------------------------------------

void CurrentPartialsView::setConnections(){
  connect(partialsList, SIGNAL(currentChanged()), this, SLOT(drawCurrent()));
  connect(tab, SIGNAL(currentChanged(QWidget*)), this, SLOT(stateChanged())); 
}

// ---------------------------------------------------------------------------
//	stateChanged
// ---------------------------------------------------------------------------
// When the user clicks on a tab it changes the state of current partials in partialsList.

void CurrentPartialsView::stateChanged(){
  Partials::State state;
  int currentState = tab->currentPageIndex();
  
  switch(currentState){
  case 0: state = Partials::amplitude; break;
  case 1: state = Partials::frequency; break;
  case 2: state = Partials::noise; break;
  default: cout<<"CurrentPartialsView::stateChanged(): check the switch statement"<<endl;
  }
  partialsList->setCurrentState(state);
}

// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------
// Basically just adds the gui components to the class.

void CurrentPartialsView::setGui(){
 
  layout       = new QGridLayout(this); 
  tab          = new QTabWidget(this, "tab");
  amplitudeTab = new AmplitudeTab(tab, "amplitudeTab", partialsList);
  frequencyTab = new FrequencyTab(tab, "frequencyTab", partialsList); 
  noiseTab     = new NoiseTab(tab, "noiseTab", partialsList); 
  QFont tab_font(tab->font());
  tab_font.setPointSize(12);
  tab->setFont(tab_font); 
  tab->insertTab(amplitudeTab, "  Amplitude  ");
  tab->insertTab(frequencyTab, "  Frequency  ");
  tab->insertTab(noiseTab    , "    Noise    ");
  layout->addWidget(tab, 1, 0);
}

/*********************************************************************************/


// ---------------------------------------------------------------------------
//	Tab constructor - Tab is a virtual base class so actual Tab objects can not be 
//                        constructed, just subclasses of Tab.
// ---------------------------------------------------------------------------

Tab::Tab(QWidget* parent, char* name, PartialsList* pList):QWidget(parent, name){
  partialsList = pList;
  setGui();
  connect(okPushButton, SIGNAL(clicked()), this, SLOT(shiftValues())); 
}

// ---------------------------------------------------------------------------
//	update - virtual method
// ---------------------------------------------------------------------------
// is called when current partials in partialsList is changed and the tab
// needs to be updated. See update in subclasses to get further information.

void Tab::update(){
  QString state = "";
  QString duration = "duration: "; 
  QString nrOfPartials = "number of Partials: "; 
  QString temp = "";
  QString max = "";
  QString name = partialsList->getCurrentName();
  
  if(!partialsList->isEmpty()){            // setNum -> bad way to convert double to string
    duration.append(temp.setNum(partialsList->getCurrentDuration()));
    nrOfPartials.append(temp.setNum(partialsList->getCurrentNrOfPartials()));

    if(partialsList->isCurrentChannelized()){
      state.append("channelized ");
    }
    
    if(partialsList->isCurrentDistilled()){
      state.append("distilled ");
    }
  }

  box->setTitle(name);
  stateText->setText(state);
  durationText->setText(duration);
  nrOfPartialsText->setText(nrOfPartials);
  maxNumber->setText(max);
}

// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------
// sets all gui components of the Tab class

void Tab::setGui(){
  tabLayout = new QVBoxLayout(this); 
  tabLayout->setSpacing( 6 );
  tabLayout->setMargin( 11 );

  box = new QGroupBox( this, "box" );
  box->setFrameShadow( QGroupBox::Sunken );
  box->setAlignment(AlignHCenter ); 
  box->setColumnLayout(0, Qt::Vertical );
  box->layout()->setSpacing( 0 );
  box->layout()->setMargin( 0 );
  boxLayout = new QGridLayout( box->layout() );
  boxLayout->setAlignment( Qt::AlignTop );
  boxLayout->setSpacing( 6 );
  boxLayout->setMargin( 11 );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  boxLayout->addItem( spacer, 1, 0 );
  
  partialsView = new QLabel(box, "label");
 
  boxLayout->addWidget(partialsView, 2, 0 );
  infoBox = new QFrame( box, "infoBox" );
  infoBox->setFrameShape( QFrame::NoFrame );
  infoBox->setFrameShadow( QFrame::Raised );
  infoBoxLayout = new QGridLayout( infoBox ); 
  infoBoxLayout->setSpacing( 6 );
  infoBoxLayout->setMargin( 11 );
  
  stateText = new QLabel( infoBox, "stateText" );
  QFont stateText_font(  stateText->font() );
  stateText->setFont( stateText_font ); 
  infoBoxLayout->addMultiCellWidget( stateText, 0, 0, 0, 1 );

  shiftText = new QLabel( infoBox, "shiftText" );
  QFont shiftText_font(  shiftText->font() );
  shiftText->setFont( shiftText_font ); 
  shiftText->setMinimumSize(100,25);
  shiftText->setMaximumSize(100,25);
  
  infoBoxLayout->addWidget( shiftText, 1, 3 );

  shiftValue = new QLineEdit( infoBox, "shiftValue" );
  shiftValue->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, shiftValue->sizePolicy().hasHeightForWidth() ) );
  QFont shiftValue_font(shiftValue->font() );
  shiftValue->setFont(shiftValue_font ); 
  shiftValue->setMaxLength(40);
  shiftValue->setMinimumSize(40,20);
  shiftValue->setMaximumSize(40,20);

  infoBoxLayout->addWidget( shiftValue, 1, 4 );

  nrOfPartialsText = new QLabel( infoBox, "nrOfPartialsText" );
  QFont nrOfPartialsText_font(  nrOfPartialsText->font() );
  nrOfPartialsText->setFont( nrOfPartialsText_font ); 
  nrOfPartialsText->setText("number of Partials: "); 
  nrOfPartialsText->setMinimumSize(200,16);
  nrOfPartialsText->setMaximumSize(200,16);

  infoBoxLayout->addWidget( nrOfPartialsText, 1, 7 );

  maxText = new QLabel(infoBox, "maxText");
  QFont maxText_font(maxText->font());
  maxText->setFont(maxText_font); 
  maxText->setMinimumSize(82,25); 
  maxText->setMaximumSize(82,25); 

  infoBoxLayout->addWidget(maxText, 1, 0 );

  maxNumber = new QLabel( infoBox, "maxNumber" );
  maxNumber->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, maxNumber->sizePolicy().hasHeightForWidth() ) );
  QFont maxNumber_font(  maxNumber->font() );
  maxNumber->setFont( maxNumber_font ); 
  maxNumber->setMinimumSize(150,25);
  maxNumber->setMaximumSize(150,25);
  infoBoxLayout->addWidget( maxNumber, 1, 1 );

  durationText = new QLabel( infoBox, "durationText" );
  QFont durationText_font(  durationText->font() );
  durationText->setFont( durationText_font ); 
  durationText->setText("duration: ");
  durationText->setMinimumSize(135,16);
  durationText->setMaximumSize(135,16);
  
  infoBoxLayout->addWidget( durationText, 0, 7);
  QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  infoBoxLayout->addItem( spacer_5, 1, 6 );
  QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  infoBoxLayout->addItem( spacer_6, 1, 2 );
  QSpacerItem* spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  infoBoxLayout->addItem( spacer_7, 1, 8 );

  okPushButton = new QPushButton( infoBox, "okPushButton" );
  okPushButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, okPushButton->sizePolicy().hasHeightForWidth() ) );
  okPushButton->setMaximumSize( QSize( 30, 25 ) );
  okPushButton->setText( tr( "OK" ) );

  infoBoxLayout->addWidget( okPushButton, 1, 5 );

  boxLayout->addWidget( infoBox, 0, 0 );
  tabLayout->addWidget( box );
}

/******************************************************************************/

// ---------------------------------------------------------------------------
//	AmplitudeTab constructor
// ---------------------------------------------------------------------------
// sets all specific parameters for amplitude in a tab

AmplitudeTab::AmplitudeTab(QWidget* parent, char* name, PartialsList* partialsList):Tab(parent, name, partialsList){
  shiftText->setText( tr( "scale amplitude" ) );
  maxText->setText( tr( "max amplitude: " ) );
  QPixmap pixmap = partialsList->getCurrentAmplitudePixmap();
  partialsView->setMinimumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setMaximumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setBackgroundColor("white");
  partialsView->setPixmap(pixmap);
}

// ---------------------------------------------------------------------------
//	update
// ---------------------------------------------------------------------------
// is called whenever current partials in partialsList is changed and the tab
// needs to get updated.

void AmplitudeTab::update(){
  Tab::update();
  partialsView->setPixmap(partialsList->getCurrentAmplitudePixmap()); // amplitude plot
  if(!partialsList->isEmpty()){
    QString s = "";                     // lazy way to convert from double to string
    maxNumber->setText(s.setNum(partialsList->getCurrentMaxAmplitude())); 
  }
}

// ---------------------------------------------------------------------------
//      shiftValues
// ---------------------------------------------------------------------------
// shift the amplitude values of current partials in partialsList.

void AmplitudeTab::shiftValues(){
  partialsList->shiftCurrentAmplitude((int)(shiftValue->text()).toDouble());
}

/************************************************************************************/

FrequencyTab::FrequencyTab(QWidget* parent, char* name, PartialsList* partialsList):Tab(parent, name, partialsList){
  shiftText->setText(tr("scale frequency"));
  maxText->setText(tr("max frequency: "));
  QPixmap pixmap = partialsList->getCurrentFrequencyPixmap();
  partialsView->setMinimumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setMaximumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setBackgroundColor("white");
  partialsView->setPixmap(pixmap);
}

// ---------------------------------------------------------------------------
//	update
// ---------------------------------------------------------------------------
// is called whenever current partials in partialsList is changed and the tab
// needs to get updated.

void FrequencyTab::update(){
  Tab::update();
  partialsView->setPixmap(partialsList->getCurrentFrequencyPixmap()); 
  if(!partialsList->isEmpty()){
    QString s = "";                    // lazy way to convert from double to string
    maxNumber->setText(s.setNum(partialsList->getCurrentMaxFrequency()));
  }
}

// ---------------------------------------------------------------------------
//      shiftValues
// ---------------------------------------------------------------------------
// shift the frequency values of current partials in partialsList.

void FrequencyTab::shiftValues(){
  partialsList->shiftCurrentFrequency((int)(shiftValue->text()).toDouble());
}

/************************************************************************************/

NoiseTab::NoiseTab(QWidget* parent, char* name, PartialsList* partialsList):Tab(parent, name, partialsList){
  shiftText->setText(tr("scale noise"));
  maxText->setText(tr("max noise: "));
  QPixmap pixmap = partialsList->getCurrentNoisePixmap();
  partialsView->setMinimumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setMaximumSize(QSize(pixmap.width(), pixmap.height()));
  partialsView->setBackgroundColor("white");
  partialsView->setPixmap(pixmap);
}

// ---------------------------------------------------------------------------
//      shiftValues
// ---------------------------------------------------------------------------
// shift the noise values of current partials in partialsList.

void NoiseTab::shiftValues(){
  partialsList->shiftCurrentNoise((int)(shiftValue->text()).toDouble());
}

// ---------------------------------------------------------------------------
//	update
// ---------------------------------------------------------------------------
// is called whenever current partials in partialsList is changed and the tab
// needs to get updated.
void NoiseTab::update(){
  Tab::update();
  partialsView->setPixmap(partialsList->getCurrentNoisePixmap()); 
  if(!partialsList->isEmpty()){
    QString s = "";                       // lazy way to convert from double to string
    maxNumber->setText(s.setNum(partialsList->getCurrentMaxNoise()));
  }
}








