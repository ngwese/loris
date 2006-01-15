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
 * morphDialog.c++
 *
 * GUI container for the morphArea view window and controls for morphing
 * sounds.
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "morphDialog.h"
#include "morphArea.h"
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

// ---------------------------------------------------------------------------
//      MorphDialog constructor
// ---------------------------------------------------------------------------
MorphDialog::MorphDialog(
	QWidget*	parent,
	char*		name,
	PartialsList*	pList,
	QStatusBar*	status
):QDialog(parent, name, TRUE){
  statusbar	= status;
  partialsList	= pList;
  canvas	= new QCanvas(735, 350); 
  morph1	= "";
  morph2	= "";
  setGui();
  setConnections();
  setLists();
  show();
  i = 0;
}

// ---------------------------------------------------------------------------
//      setLists
// ---------------------------------------------------------------------------
// Every time a new morphDialog is created the pop-up lists for selecting
// partials to be morphed have to be filled.
void MorphDialog::setLists(){
  partial1List->clear();
  partial2List->clear();
  
  // fill pop-up lists for selection of morph partials.
  // First I just inserted the channelized and distilled ones 
  // but then I couldn't keep track of their position in partialsList
  // and couldn't communicate correctly which partials should actually
  // be morphed. Find out a way to do that! If you morph without 
  // channelizing and distilling first the morph will just result
  // in a cross fade.

  for(int i = 0; i<partialsList->getLength(); i++){
    partial1List->insertItem(partialsList->getPartials(i)->getName(), i);
    partial2List->insertItem(partialsList->getPartials(i)->getName(), i);
  }
  
  int current = partialsList->getCurrentIndex();
  
  partial1List->setCurrentItem(current);
  partial2List->setCurrentItem(current);
  updateMorph1(current);  // default should be
  updateMorph2(current);  // current partials
}

// ---------------------------------------------------------------------------
//     setConnections
// ---------------------------------------------------------------------------
// Sets all connections which handles GUI events.
void MorphDialog::setConnections(){
  // the integer sent represents one of the elements in the button group
  // which changes state or clears some of the breakpoints.
  connect(onOffBox, SIGNAL(clicked(int)), morphArea, SLOT(showHideClear(int)));
  connect(clearAllButton,SIGNAL(clicked()), morphArea, SLOT(clearAll()));
  connect(morphButton,SIGNAL(clicked()), morphArea, SLOT(morph()));
  connect(morphButton,SIGNAL(clicked()), this, SLOT(hide()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
  connect(partial1List, SIGNAL(highlighted(int)), 
	  this, SLOT(updateMorph1(int)));
  connect(partial2List, SIGNAL(highlighted(int)), 
	  this, SLOT(updateMorph2(int)));
}

// ---------------------------------------------------------------------------
//     updateMorph1
// ---------------------------------------------------------------------------
// changes all gui elements when morph1 is changed, and changes  
// morph1 partials in partialslist. I didn't see the need in using model/view
// pattern here since the messages will just go back and forth.
void MorphDialog::updateMorph1(int pos){
  partialsList->setMorphPartials1(pos);
  morph1 = partial1List->text(pos);
  name1Label->setText(morph1);
  morphArea->setMorph1(morph1);
  morphBox->setTitle(QString("Morph "+morph1+" with "+morph2));
}
  
// ---------------------------------------------------------------------------
//     updateMorph2
// ---------------------------------------------------------------------------
// changes all gui elements when morph2 is changed, and changes 
// morph2 partials in partialslist. I didn't see the need in using model/view
// pattern here since the messages will just go back and forth.
void MorphDialog::updateMorph2(int pos){
  partialsList->setMorphPartials2(pos);
  morph2 = partial2List->text(pos);
  name2Label->setText(morph2); 
  morphArea->setMorph2(morph2);
  morphBox->setTitle(QString("Morph "+morph1+" with "+morph2));
}

// ---------------------------------------------------------------------------
//     setGui
// ---------------------------------------------------------------------------
// Sets all GUI components of the dialog
void MorphDialog::setGui(){
  QSpacerItem* spacer_0;
  QSpacerItem* spacer_12;
  QSpacerItem* spacer_13;
  QSpacerItem* spacer_14;
  QSpacerItem* spacer_16;
  QSpacerItem* spacer_17;
  QSpacerItem* spacer_19;
  QSpacerItem* spacer_20;
  QSpacerItem* spacer_21;

  dialogLayout = new QGridLayout(this); 
  dialogLayout->setSpacing(6);
  dialogLayout->setMargin(20);
  morphBox = new QGroupBox(this, "morphBox" );
  morphBox->setColumnLayout(0, Qt::Vertical);
  morphBox->layout()->setSpacing(0);
  morphBox->layout()->setMargin(0);
  morphBoxLayout = new QGridLayout(morphBox->layout());
  morphBoxLayout->setAlignment(Qt::AlignTop);
  morphBoxLayout->setSpacing(6);
  morphBoxLayout->setMargin(11);
  spacer_0 = new QSpacerItem(
	150,
	20,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
  );

  dialogLayout->addItem(spacer_0, 2, 1);
  morphArea = new MorphArea(
	canvas,
	morphBox,
	"morphArea",
	partialsList,
	statusbar
  );

  morphArea->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		morphArea->sizePolicy().hasHeightForWidth()
	)
  );

  morphArea->setMinimumSize(QSize(740, 356));
  morphArea->setMaximumSize(QSize(740, 356));
  morphBoxLayout->addWidget(morphArea, 1, 0);
  name1Label = new QLabel(morphBox, "name1Label");
  QFont name1Label_font(name1Label->font());
  name1Label_font.setPointSize(10);
  name1Label->setFont(name1Label_font); 
   
  morphBoxLayout->addWidget(name1Label, 0, 0);
  
  morphSideLayout = new QGridLayout; 
  morphSideLayout->setSpacing(6);
  morphSideLayout->setMargin(0);
  onOffBox = new QButtonGroup(morphBox, "onOffBox");
  onOffBox->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		onOffBox->sizePolicy().hasHeightForWidth()
	)
  );

  onOffBox->setColumnLayout(0, Qt::Vertical);
  onOffBox->layout()->setSpacing(0);
  onOffBox->layout()->setMargin(0);
  onOffBoxLayout = new QGridLayout(onOffBox->layout());
  onOffBoxLayout->setAlignment(Qt::AlignTop);
  onOffBoxLayout->setSpacing(6);
  onOffBoxLayout->setMargin(11);

  allButton = new QRadioButton( onOffBox, "allButton" );
  allButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		allButton->sizePolicy().hasHeightForWidth()
	)
  );

  allButton->setMaximumSize( QSize(90, 19 ) );
   
  QPalette pal;
  QColorGroup cg;
  cg = allButton->colorGroup();
  cg.setColor( QColorGroup::Foreground, QColor("black"));
  pal.setActive(cg);
  cg.setColor( QColorGroup::Foreground, QColor("black"));
  pal.setInactive(cg);
  allButton->setPalette(pal);
  QFont allButton_font(allButton->font());
  allButton_font.setPointSize(10);
  allButton->setFont(allButton_font); 
  allButton->setText(tr("Amp/Fre/Noise"));
  allButton->toggle();

  onOffBoxLayout->addWidget(allButton, 0, 0);

  line = new QFrame(onOffBox, "line");
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);

  onOffBoxLayout->addMultiCellWidget(line, 1, 1, 0, 1 );
 
  amplitudeButton = new QRadioButton( onOffBox, "amplitudeButton" );
  amplitudeButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		amplitudeButton->sizePolicy().hasHeightForWidth()
	)
  );

  amplitudeButton->setMaximumSize( QSize( 90, 19 ) );
  cg = amplitudeButton->colorGroup();
  cg.setColor( QColorGroup::Foreground, QColor("red") );
  pal.setActive( cg );
  cg.setColor( QColorGroup::Foreground, QColor("red") );
  pal.setInactive( cg );
  amplitudeButton->setPalette( pal );
  QFont amplitudeButton_font(  amplitudeButton->font() );
  amplitudeButton_font.setPointSize( 10 );
  amplitudeButton->setFont( amplitudeButton_font ); 
  amplitudeButton->setText( tr( "Amplitude" ) );
  onOffBoxLayout->addWidget( amplitudeButton, 2, 0 );

  frequencyButton = new QRadioButton( onOffBox, "frequencyButton" );
  frequencyButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		frequencyButton->sizePolicy().hasHeightForWidth()
	)
  );

  frequencyButton->setMaximumSize( QSize( 90, 19 ) );
  cg = frequencyButton->colorGroup();
  cg.setColor( QColorGroup::Foreground, QColor("darkgreen") );
  pal.setActive( cg );
  cg.setColor( QColorGroup::Foreground, QColor("darkgreen") );
  pal.setInactive( cg );
  frequencyButton->setPalette( pal );
  QFont frequencyButton_font(  frequencyButton->font() );
  frequencyButton_font.setPointSize( 10 );
  frequencyButton->setFont( frequencyButton_font ); 
  frequencyButton->setText( tr( "Frequency" ) );

  onOffBoxLayout->addWidget( frequencyButton, 3, 0 );
  noiseButton = new QRadioButton(onOffBox, "noiseButton");
  noiseButton->setSizePolicy(	
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		noiseButton->sizePolicy().hasHeightForWidth()
	)
  );

  noiseButton->setMaximumSize(QSize( 90, 19 ));
  cg = noiseButton->colorGroup();
  cg.setColor( QColorGroup::Foreground, QColor("blue") );
  pal.setActive( cg );
  cg.setColor( QColorGroup::Foreground, QColor("blue") );
  pal.setInactive( cg );
  noiseButton->setPalette( pal );
  QFont noiseButton_font(  noiseButton->font() );
  noiseButton_font.setPointSize( 10 );
  noiseButton->setFont( noiseButton_font ); 
  noiseButton->setText( tr( "Noise" ) );
  
  onOffBoxLayout->addWidget( noiseButton, 4, 0 );

  clearAllButton = new QPushButton( onOffBox, "clearAllButton" );
  clearAllButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		clearAllButton->sizePolicy().hasHeightForWidth()
	)
  );

  clearAllButton->setMaximumSize( QSize( 37, 25 ) );
  QFont clearAllButton_font(  clearAllButton->font() );
  clearAllButton_font.setPointSize( 10 );
  clearAllButton->setFont( clearAllButton_font ); 
  clearAllButton->setText( tr( "clear" ) );
  
  onOffBoxLayout->addWidget( clearAllButton, 0, 1 );

  clearAmpButton = new QPushButton( onOffBox, "clearAmpButton" );
  clearAmpButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		clearAmpButton->sizePolicy().hasHeightForWidth()
	)
  );

  clearAmpButton->setMaximumSize( QSize( 37, 25 ) );
  QFont clearAmpButton_font(  clearAmpButton->font() );
  clearAmpButton_font.setPointSize( 10 );
  clearAmpButton->setFont( clearAmpButton_font ); 
  clearAmpButton->setText( tr( "clear" ) );
  
  onOffBoxLayout->addWidget( clearAmpButton, 2, 1 );
  
  clearFreqButton = new QPushButton( onOffBox, "clearFreqButton" );
  clearFreqButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		clearFreqButton->sizePolicy().hasHeightForWidth()
	)
  );

  clearFreqButton->setMinimumSize( QSize( 0, 0 ) );
  clearFreqButton->setMaximumSize( QSize( 37, 25 ) );
  QFont clearFreqButton_font(  clearFreqButton->font() );
  clearFreqButton_font.setPointSize( 10 );
  clearFreqButton->setFont( clearFreqButton_font ); 
  clearFreqButton->setText( tr( "clear" ) );

  onOffBoxLayout->addWidget( clearFreqButton, 3, 1 );
  
  clearNoiseButton = new QPushButton( onOffBox, "clearNoiseButton" );
  clearNoiseButton->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)0,
		clearNoiseButton->sizePolicy().hasHeightForWidth()
	)
  );

  clearNoiseButton->setMinimumSize( QSize( 3, 25 ) );
  clearNoiseButton->setMaximumSize( QSize( 37, 25 ) );
  QFont clearNoiseButton_font(  clearNoiseButton->font() );
  clearNoiseButton_font.setPointSize( 10 );
  clearNoiseButton->setFont( clearNoiseButton_font ); 
  clearNoiseButton->setText( tr( "clear" ) );
  
  onOffBoxLayout->addWidget( clearNoiseButton, 4, 1 );
  
  morphSideLayout->addMultiCellWidget( onOffBox, 0, 0, 0, 1 );
  morphButton = new QPushButton( morphBox, "morphButton" );
  QFont morphButton_font(  morphButton->font() );
  morphButton_font.setPointSize( 12 );
  morphButton->setFont( morphButton_font ); 
  morphButton->setText( tr( "MORPH" ) );

  morphSideLayout->addWidget( morphButton, 4, 0 );
  
  spacer_12 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Expanding,
	QSizePolicy::Minimum
  );
  morphSideLayout->addMultiCell( spacer_12, 5, 5, 0, 1);
  
  spacer_13 = new QSpacerItem(
	20,
	60,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
  );
  morphSideLayout->addItem( spacer_13, 2, 0 );
  
  cancelButton = new QPushButton( morphBox, "cancelButton" );
  QFont cancelButton_font(  cancelButton->font() );
  cancelButton_font.setPointSize( 12 );
  cancelButton->setFont( cancelButton_font ); 
  cancelButton->setText( tr( "CANCEL" ) );

  morphSideLayout->addWidget( cancelButton, 3, 0 );

  morphBoxLayout->addMultiCellLayout( morphSideLayout, 1, 2, 1, 1 );

  name2Label = new QLabel( morphBox, "name2Label" );
  QFont name2Label_font(  name2Label->font() );
  name2Label_font.setPointSize(10);
  name2Label->setFont(name2Label_font); 
  name2Label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

  morphBoxLayout->addWidget( name2Label, 2, 0 );
 
  dialogLayout->addMultiCellWidget(morphBox, 3, 7, 0, 2);
  spacer_14 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
  );

  dialogLayout->addItem( spacer_14, 0, 0 );
  partialsBox = new QGroupBox(this, "partialsBox" );
  QFont partialsBox_font(  partialsBox->font() );
  partialsBox_font.setPointSize( 12 );
  partialsBox->setFont( partialsBox_font ); 
  partialsBox->setTitle( tr( "Select partials to morph" ) );
  partialsBox->setColumnLayout(0, Qt::Vertical );
  partialsBox->layout()->setSpacing( 0 );
  partialsBox->layout()->setMargin( 0 );
  partialsBoxLayout = new QGridLayout( partialsBox->layout() );
  partialsBoxLayout->setAlignment( Qt::AlignTop );
  partialsBoxLayout->setSpacing( 6 );
  partialsBoxLayout->setMargin( 11 );
  
  partial1List = new QComboBox( FALSE, partialsBox, "partial1List" );
  partial1List->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)0,
		partial1List->sizePolicy().hasHeightForWidth()
	)
  );

  partial1List->setMaximumSize( QSize( 32767, 20 ) );
  
  partialsBoxLayout->addWidget( partial1List, 1, 2 );
    
  partial2List = new QComboBox( FALSE, partialsBox, "partial2List" );
  partial2List->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)0,
		partial2List->sizePolicy().hasHeightForWidth()
	)
  );

  partial2List->setMaximumSize( QSize( 32767, 20 ) );

  partialsBoxLayout->addWidget( partial2List, 1, 6 );

  partial1Label = new QLabel( partialsBox, "partial1Label" );
  QFont partial1Label_font(  partial1Label->font() );
  partial1Label_font.setPointSize( 12 );
  partial1Label->setFont( partial1Label_font ); 
  partial1Label->setText( tr( "Morph" ) );

  partialsBoxLayout->addWidget( partial1Label, 1, 0 );

  partial2Label = new QLabel( partialsBox, "partial2Label" );
  QFont partial2Label_font(  partial2Label->font() );
  partial2Label_font.setPointSize( 12 );
  partial2Label->setFont( partial2Label_font ); 
  partial2Label->setText( tr( "with" ) );
  partialsBoxLayout->addWidget( partial2Label, 1, 4 );

  spacer_16 = new QSpacerItem(
	21,
	20,
	QSizePolicy::Fixed,
	QSizePolicy::Minimum
  );
  partialsBoxLayout->addMultiCell( spacer_16, 1, 2, 1, 1 );

  spacer_17 = new QSpacerItem(
	21,
	20,
	QSizePolicy::Fixed,
	QSizePolicy::Minimum
  );
  partialsBoxLayout->addMultiCell( spacer_17, 1, 2, 3, 3 );

  spacer_21 = new QSpacerItem(
	21,
	20,
	QSizePolicy::Fixed,
	QSizePolicy::Minimum
  );
  partialsBoxLayout->addMultiCell( spacer_21, 1, 2, 5, 5 );

  spacer_19 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
	);
  partialsBoxLayout->addItem( spacer_19, 2, 6 );

  spacer_20 = new QSpacerItem(
	20,
	20,
	QSizePolicy::Minimum,
	QSizePolicy::Expanding
  );
  partialsBoxLayout->addItem( spacer_20, 0, 6 );

  dialogLayout->addWidget(partialsBox, 1, 0);
}
