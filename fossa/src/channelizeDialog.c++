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
 * channelizeDialog.c++ 
 *
 *
 * Implementation of class ChannelizeDialog, a dialog provided to let a user
 * specify channelization parameters, minimum frequency, maximum frequency,
 * and reference label.
 * 
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "channelizeDialog.h"
#include "soundList.h"

#include <qlayout.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
---------------------------------------------------------------------------
	ChannelizeDialog constructor
---------------------------------------------------------------------------
Creates a modal dialog to let the user specify channelization parameters. 
*/
ChannelizeDialog::ChannelizeDialog( 
	QWidget*	parent,
	const char*	name, 
	SoundList*	list, 
	QStatusBar*	status
): QDialog( parent, name, TRUE){
  statusbar = status;
  soundList = list;

  setGui();
  setConnections();
  show();
}

/*
---------------------------------------------------------------------------
	setConnections
---------------------------------------------------------------------------
Specifies how cancel and channelize buttons react when clicked.
*/
void ChannelizeDialog::setConnections(){
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
  connect(channelizeButton, SIGNAL(clicked()), this, SLOT(channelizing()));
}

/*
---------------------------------------------------------------------------
	channelizing
---------------------------------------------------------------------------
Is called when a user clicks on the channelize button.
*/
void ChannelizeDialog::channelizing(){
  //value() doesn't work so well!
  double refLabel = refSpinBox->text().toDouble();
  double max = maxSpinBox->text().toInt();
  double min = minSpinBox->text().toInt();
  QString temp = "";

  if(max > min){
    try{
      statusbar->message("Channelizing sound with minimum frequency: "
			+temp.setNum(min)
			+", maximum frequency: "
			+temp.setNum(max)
			+", and reference label: "
			+temp.setNum(refLabel));
      soundList->channelizeCurrent((int)refLabel, min, max);
      statusbar->message("Channelized sound successfully.", 5000);
    }
    catch(InvalidArgument& ex){
      statusbar->message(ex.what());
    }
    catch(...){
      statusbar->message(
	"Could not channelize sound, are the parameters correct?."
      );
    }
  }
  else {
    statusbar->message(
	"Maximum Frequency is greater than minimum frequency, please try again."
    );
  }
  hide();
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
Sets GUI components of this dialog.
*/
void ChannelizeDialog::setGui(){
  QSpacerItem* spacer;
  QSpacerItem* spacer_2;
  QSpacerItem* spacer_3;
  QSpacerItem* spacer_4;
  QSpacerItem* spacer_5;
  QSpacerItem* spacer_6;
  QSpacerItem* spacer_7;

  resize( 641, 209 ); 

  ChannelizationLayout = new QGridLayout( this ); 
  ChannelizationLayout->setSpacing( 6 );
  ChannelizationLayout->setMargin( 11 );

  paramBox = new QGroupBox( this, "paramBox" );
  paramBox->setSizePolicy( 
	QSizePolicy( 
		(QSizePolicy::SizeType)7, 
		(QSizePolicy::SizeType)7, 
		paramBox->sizePolicy().hasHeightForWidth() 
	)
  );
  QFont paramBox_font(  paramBox->font() );
  paramBox_font.setFamily( "helvetica" );
  paramBox_font.setPointSize( 14 );
  paramBox->setFont( paramBox_font ); 
  paramBox->setTitle( tr( "Channelization parameters" ) );
  paramBox->setColumnLayout(0, Qt::Vertical );
  paramBox->layout()->setSpacing( 0 );
  paramBox->layout()->setMargin( 0 );
  paramBoxLayout = new QGridLayout( paramBox->layout() );
  paramBoxLayout->setAlignment( Qt::AlignTop );
  paramBoxLayout->setSpacing( 6 );
  paramBoxLayout->setMargin( 11 );
  spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  paramBoxLayout->addItem( spacer, 2, 1 );
  spacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  paramBoxLayout->addItem( spacer_2, 0, 1 );

  maxSpinBox = new QSpinBox( paramBox, "maxSpinBox" );
  QFont maxSpinBox_font(  maxSpinBox->font() );
  maxSpinBox_font.setPointSize( 12 );
  maxSpinBox->setFont( maxSpinBox_font ); 
  maxSpinBox->setMaxValue(10000); 
  maxSpinBox->setValue(1000); 

  paramBoxLayout->addWidget( maxSpinBox, 1, 5 );
  spacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
  paramBoxLayout->addItem(spacer_3, 6, 1);

  maxLabel = new QLabel( paramBox, "maxLabel" );
  maxLabel->setSizePolicy( 
	QSizePolicy( 
		(QSizePolicy::SizeType)7, 
		(QSizePolicy::SizeType)1, 
		maxLabel->sizePolicy().hasHeightForWidth() 
	) 
  );
  QFont maxLabel_font(  maxLabel->font() );
  maxLabel_font.setPointSize( 12 );
  maxLabel->setFont( maxLabel_font ); 
  maxLabel->setText( tr( "Maximum Frequency" ) );
 

  paramBoxLayout->addWidget( maxLabel, 1, 4 );
  spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  paramBoxLayout->addItem( spacer_4, 4, 1 );
  spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  paramBoxLayout->addItem( spacer_5, 1, 6 );
  spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  paramBoxLayout->addItem( spacer_6, 1, 3 );

  minLabel = new QLabel( paramBox, "minLabel" );
  minLabel->setSizePolicy( 
	QSizePolicy( 
		(QSizePolicy::SizeType)7, 
		(QSizePolicy::SizeType)1, 
		minLabel->sizePolicy().hasHeightForWidth() 
	) 
  );
  QFont minLabel_font(  minLabel->font() );
  minLabel_font.setPointSize( 12 );
  minLabel->setFont( minLabel_font ); 
  minLabel->setText( tr( "Minimum frequency" ) );

  paramBoxLayout->addWidget( minLabel, 1, 1 );
 
  refSpinBox = new QSpinBox( paramBox, "refSpinBox" );
  QFont refSpinBox_font(  refSpinBox->font() );
  refSpinBox_font.setPointSize( 12 );
  refSpinBox->setFont( refSpinBox_font ); 
  refSpinBox->setValue(1); 

  paramBoxLayout->addWidget( refSpinBox, 3, 2 );
 
  minSpinBox = new QSpinBox( paramBox, "minSpinBox" );
  QFont minSpinBox_font(  minSpinBox->font() );
  minSpinBox_font.setPointSize( 12 );
  minSpinBox->setFont( minSpinBox_font );
  minSpinBox->setMinValue(0); 
  minSpinBox->setMaxValue(10000); 

  paramBoxLayout->addWidget( minSpinBox, 1, 2 );

  refLabel = new QLabel( paramBox, "refLabel" );
  refLabel->setSizePolicy( 
	QSizePolicy( 
		(QSizePolicy::SizeType)7, 
		(QSizePolicy::SizeType)1, 
		refLabel->sizePolicy().hasHeightForWidth() 
	) 
  );
  QFont refLabel_font(  refLabel->font() );
  refLabel_font.setPointSize( 12 );
  refLabel->setFont( refLabel_font ); 
  refLabel->setText( tr( "Reference label" ) );

  paramBoxLayout->addWidget( refLabel, 3, 1 );

  ChannelizationLayout->addMultiCellWidget( paramBox, 0, 2, 0, 0 );

  channelizeButton = new QPushButton( this, "channelizeButton" );
  channelizeButton->setText( tr( "Channelize" ) );

  ChannelizationLayout->addWidget( channelizeButton, 1, 1 );

  cancelButton = new QPushButton( this, "cancelButton" );
  cancelButton->setText( tr( "Cancel" ) );

  ChannelizationLayout->addWidget( cancelButton, 2, 1 );
  spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  ChannelizationLayout->addItem( spacer_7, 0, 1 );
}
