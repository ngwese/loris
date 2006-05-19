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
 * currentSoundView.c++
 *
 * Class CurrentSoundView has three tabs representing the amplitude, 
 * frequency, and noise plots for current collection of partials in the 
 * soundList. Whenever current sound changes reText() is called 
 * and the plots are updated.
 *
 * Class Tab displays current partial's envelopes
 * (amplitude, frequency, and noise) 
 * as well as information about the current sound.   
 *
 * Susanne Lefvert, 1 March 2002
 *
 * Class Tab is a QCanvasView so that it can use the Fossa::Axis class to
 * draw the axes. (Originally Tab was just a big blank label
 * onto which the Pixmap was pasted.) -Chris H.
 */

#if HAVE_CONFIG_H       // #define directives are placed in config.h by autoconf
#include <config.h>
#endif

#include "currentSoundView.h"
#include "soundList.h"
#include "sound.h"
#include "soundPlot.h"

#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qspinbox.h> 
#include <qlineedit.h> 
#include <qstring.h>


/*
---------------------------------------------------------------------------
	CurrentSoundView constructor
---------------------------------------------------------------------------
*/
CurrentSoundView::CurrentSoundView(
	QWidget*	parent,
	char*		name,
	SoundList*	list
):QFrame(parent,name){
  soundList = list;
  setGui(); 
  setConnections();
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
Basically just adds the gui components to the class.
*/
void CurrentSoundView::setGui(){
  layout	= new QGridLayout(this);
  tab		= new QTabWidget(this, "tab");
  amplitudeTab	= new Tab(tab, "amplitudeTab", soundList, Tab::amplitude);
  frequencyTab	= new Tab(tab, "frequencyTab", soundList, Tab::frequency);
  noiseTab	= new Tab(tab, "noiseTab", soundList, Tab::noise);

  QFont tab_font(tab->font());
  tab_font.setPointSize(12);
  tab->setFont(tab_font);
  tab->insertTab(amplitudeTab, "  Amplitude  ");
  tab->insertTab(frequencyTab, "  Frequency  ");
  tab->insertTab(noiseTab    , "    Noise    ");
  layout->addWidget(tab, 1, 0);
}

/*
---------------------------------------------------------------------------
	setConnections
---------------------------------------------------------------------------
*/
void CurrentSoundView::setConnections(){
  connect(soundList, SIGNAL(currentChanged()), this, SLOT(redraw()));
  connect(tab, SIGNAL(currentChanged(QWidget*)), this, SLOT(update()));
}

/*
---------------------------------------------------------------------------
	update
---------------------------------------------------------------------------
Update tabs and set correct tab to be selected and visible.
*/
void CurrentSoundView::update(){
  int currentType = tab->currentPageIndex();
  
  switch(currentType){
    case 0: amplitudeTab->update(false); break;
    case 1: frequencyTab->update(false); break;
    case 2: noiseTab->update(false); break;
  }
}

/*
---------------------------------------------------------------------------
	redraw
---------------------------------------------------------------------------
Same as update, but redo the plot as well.
*/
void CurrentSoundView::redraw(){
  int currentType = tab->currentPageIndex();
  
/*
  switch(currentType){
    case 0: amplitudeTab->update(true); break;
    case 1: frequencyTab->update(true); break;
    case 2: noiseTab->update(true); break;
  }
*/
   amplitudeTab->update(true);
   frequencyTab->update(true);
   noiseTab->update(true);
}


/*********************************************************************************/


/*
---------------------------------------------------------------------------
	Tab constructor
---------------------------------------------------------------------------
*/
Tab::Tab(
	QWidget*	parent,
	char*		name,
	SoundList*	pList,
	TabType		t
):QWidget(parent, name){
  double maxY;

  soundList = pList;
  type = t;
  plotted = false;

  canvas  = new QCanvas(800, 450);

  setGui();
  setConnections();

  switch( type ){
    case empty:
      std::cout <<
	"Shouldn't be seeing this... misuse of Tab constructor."
	<< endl;
      break;

    case amplitude:
      shiftText->setText( tr( "Scale Amplitude" ) );
      maxText->setText( tr( "Max Amplitude: " ) );
      break;

    case frequency:
      shiftText->setText( tr( "Scale Frequency" ) );
      maxText->setText( tr( "Max Frequency: " ) );
      break;

    case noise:
      shiftText->setText( tr( "Scale Noise" ) );
      maxText->setText( tr( "Max Noise: " ) );
      break;
  }

}

/*
---------------------------------------------------------------------------
	hilight
---------------------------------------------------------------------------
Wrapper for partialsView->hilight()
*/
void Tab::hilight(int p){ partialsView->hilight(p-1); }

/*
---------------------------------------------------------------------------
	update
---------------------------------------------------------------------------
The purpose here is really to refresh the text fields in the tab. This is
called whenever the user clicks fr
*/
void Tab::update(bool redraw){

  /**********************/
  QString state = "";
  QString duration = "Duration: "; 
  QString nrOfPartials = "Number of Partials: "; 
  QString temp = "";
  QString name = soundList->getCurrentName();
  
  // setNum -> bad way to convert double to string - Susanne
  if(!soundList->isEmpty()){
    duration.append(temp.setNum(soundList->getCurrentDuration()));
    nrOfPartials.append(temp.setNum(soundList->getCurrentNrOfPartials()));

    QString s = "";
    maxNumber->setText(s.setNum(soundList->getCurrentMax((Sound::ValType)type))); 

    if(soundList->isCurrentChannelized()){
      state.append("channelized ");
    }
    
    if(soundList->isCurrentDistilled()){
      state.append("distilled ");
    }
  } else {
    duration = "";
    nrOfPartials = "";
    maxNumber->setText("");
  }

  box->setTitle(name);
  stateText->setText(state);
  durationText->setText(duration);
  nrOfPartialsText->setText(nrOfPartials);

  /*Make the slider visible or invisible depending on whether the current
   *sound is channelized and distilled. */
  if( ! soundList->isEmpty() &&
	soundList->isCurrentChannelized() && soundList->isCurrentDistilled() ){
    pSelect->show();
    pIndicator->show();
  } else {
    pSelect->hide();
    pIndicator->hide();
  }

  //Check that the plot needs to be updated.
  if(soundList->isEmpty() || soundList->getCurrentIndex() == -1){
    pSelect->setRange(0, 0);
    pSelect->setValue(0);

    if(plotted){
      partialsView->setType(empty);
      plotted = false;
    }
  }else{
    pSelect->setRange(0, soundList->getCurrentNrOfPartials());
    pSelect->setValue(0);

    if(!plotted || redraw){
      partialsView->setType(type);
      plotted = true;
    }
  }
}

/*
---------------------------------------------------------------------------
	setConnections
---------------------------------------------------------------------------
*/
void Tab::setConnections(){
  connect(okPushButton, SIGNAL(clicked()), this, SLOT(shiftValues())); 
  connect(pSelect, SIGNAL(valueChanged(int)), pIndicator, SLOT(display(int)));
  connect(pSelect, SIGNAL(valueChanged(int)), this, SLOT(hilight(int))); 
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
sets all gui components of the Tab class
*/
void Tab::setGui(){
  QSpacerItem* spacer;
  QSpacerItem* spacer_5;
  QSpacerItem* spacer_6;


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



  /*The first row contains the Info Box which holds the text labels.*/
  infoBox = new QFrame( box, "infoBox" );
  infoBox->setFrameShape( QFrame::PopupPanel );
  infoBox->setFrameShadow( QFrame::Raised );

  infoBoxLayout = new QGridLayout( infoBox ); 
  infoBoxLayout->setSpacing( 6 );
  infoBoxLayout->setMargin( 11 );

  
  /*Set up the text labels and input boxes.*/
  stateText = new QLabel( infoBox, "stateText" );
  QFont stateText_font(  stateText->font() );
  stateText->setFont( stateText_font ); 

  /*Label that says "Shift"*/
  shiftText = new QLabel( infoBox, "shiftText" );
  QFont shiftText_font(  shiftText->font() );
  shiftText->setFont( shiftText_font ); 
  shiftText->setMinimumSize(120,25);
  shiftText->setMaximumSize(120,25);

  /*Input box in which the user enters the amount to shift.*/
  shiftValue = new QLineEdit( infoBox, "shiftValue" );
  QFont shiftValue_font(shiftValue->font() );
  shiftValue->setFont(shiftValue_font ); 
  shiftValue->setMaxLength(40);
  shiftValue->setMinimumSize(40,20);
  shiftValue->setMaximumSize(40,20);
  shiftValue->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		shiftValue->sizePolicy().hasHeightForWidth()
	)
  );

  /*Label that says Number of Partials:.*/
  nrOfPartialsText = new QLabel( infoBox, "nrOfPartialsText" );
  QFont nrOfPartialsText_font(  nrOfPartialsText->font() );
  nrOfPartialsText->setFont( nrOfPartialsText_font ); 
  nrOfPartialsText->setText("Number of Partials: "); 
  nrOfPartialsText->setMinimumSize(200,16);
  nrOfPartialsText->setMaximumSize(200,16);

  /*Label that says Max Amp/Freq/noise.*/
  maxText = new QLabel(infoBox, "maxText");
  QFont maxText_font(maxText->font());
  maxText->setFont(maxText_font); 
  maxText->setMinimumSize(120,25); 
  maxText->setMaximumSize(120,25); 

  /*Label that holds the actual number.*/
  maxNumber = new QLabel( infoBox, "maxNumber" );
  QFont maxNumber_font(  maxNumber->font() );
  maxNumber->setFont( maxNumber_font ); 
  maxNumber->setMinimumSize(150,25);
  maxNumber->setMaximumSize(150,25);
  maxNumber->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)1,
		(QSizePolicy::SizeType)1,
		maxNumber->sizePolicy().hasHeightForWidth()
	)
  );

  /*Label with "Duration: "*/
  durationText = new QLabel( infoBox, "durationText" );
  QFont durationText_font(  durationText->font() );
  durationText->setFont( durationText_font ); 
  durationText->setText("duration: ");
  durationText->setMinimumSize(135,16);
  durationText->setMaximumSize(135,16);

  /*Various spacers.*/
  spacer_5 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Expanding,
	QSizePolicy::Minimum
  );

  spacer_6 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Expanding,
	QSizePolicy::Minimum
  );

  /*OK button tells Fossa to do the shift.*/
  okPushButton = new QPushButton( infoBox, "okPushButton" );
  okPushButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		okPushButton->sizePolicy().hasHeightForWidth()
	)
  );

  okPushButton->setMaximumSize( QSize( 30, 25 ) );
  okPushButton->setText( tr( "OK" ) );


  /*Set up the slider which selects which partial to hilight.*/
  pSelect = new QSlider(Horizontal, infoBox, "partialSelect");
  pSelect->setRange(0,0);
  pSelect->setValue(0);
  pSelect->hide();
  pIndicator = new QLCDNumber( 4, infoBox, "partialIndicator" );
  pIndicator->hide();


  /* ****************************************
   * The tab has 3 parts - the infobox in top with all the labels, a spacer,
   * and the SoundPlot. They've already been allocated, now we just put them
   * in the layout.
  */
  infoBoxLayout->addWidget( durationText, 0, 0 );
  infoBoxLayout->addItem( spacer_5, 0, 2 );
  infoBoxLayout->addWidget( stateText, 0, 2 );

  infoBoxLayout->addWidget(maxText, 1, 0 );
  infoBoxLayout->addWidget( maxNumber, 1, 1 );
//  infoBoxLayout->addItem( spacer_6, 1, 2 );
  infoBoxLayout->addWidget( nrOfPartialsText, 1, 3 );

  infoBoxLayout->addWidget( shiftText, 2, 0 );
  infoBoxLayout->addWidget( shiftValue, 2, 1 );
  infoBoxLayout->addWidget( okPushButton, 2, 2 );

  infoBoxLayout->addWidget( pSelect, 3, 0 );
  infoBoxLayout->addWidget( pIndicator, 3, 1 );

  boxLayout->addWidget( infoBox, 0, 0 );




  /*Below the Info Box is a simple spacer.*/
  spacer = new QSpacerItem(
	20,
	20,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
  );
  boxLayout->addItem( spacer, 1, 0 );
  


  /*Below the spacer is the big plot of the partials against time.*/
  partialsView = new SoundPlot(
	canvas,
	box,
	"partialsView",
	soundList,
	empty,
	-1
  );
  partialsView->setMinimumSize(QSize(5+canvas->width(), 5+canvas->height()));
  partialsView->setMaximumSize(QSize(5+canvas->width(), 5+canvas->height()));
  partialsView->setBackgroundColor("white");
  partialsView->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)0,
                (QSizePolicy::SizeType)0,
                partialsView->sizePolicy().hasHeightForWidth()
        )
  );
  boxLayout->addWidget(partialsView, 2, 0 );

  tabLayout->addWidget( box );
}


/*
---------------------------------------------------------------------------
	shiftValues
---------------------------------------------------------------------------
shift the amplitude values of current sound in soundList.
*/
void Tab::shiftValues(){
  switch(type){
    case amplitude:
      soundList->shiftCurrentAmplitude((shiftValue->text()).toDouble());
      break;
    case frequency:
      soundList->shiftCurrentFrequency((shiftValue->text()).toDouble());
      break;
    case noise:
      soundList->shiftCurrentNoise((shiftValue->text()).toDouble());
      break;
  }
}
