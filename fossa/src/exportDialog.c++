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
 * exportDialog.c++ 
 *
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "exportDialog.h"
#include "soundList.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qstatusbar.h>

// ---------------------------------------------------------------------------
//	ExportAiffWidget constructor
// ---------------------------------------------------------------------------
// Creates a widget which makes it possible for a user to specify the 
// parameters, samplerate, and numer of sample bits, when exporting to 
// an aiff file. 

ExportAiffWidget::ExportAiffWidget(
	QWidget*	parent,
	char*		name
):QWidget(parent,name){
  setGui();
}

// ---------------------------------------------------------------------------
//	getSampleRate
// ---------------------------------------------------------------------------
// Returns the samplerate specified by the user.

int ExportAiffWidget::getSampleRate(){
  return sampleRateSpinBox->value();
} 

// ---------------------------------------------------------------------------
//	getNbOfBits
// ---------------------------------------------------------------------------
// Returns number the number of sample bits specified by the user.

int ExportAiffWidget::getNbOfBits(){
  return nbOfBitsSpinBox->value();
}
// ---------------------------------------------------------------------------
//	setGui
// ---------------------------------------------------------------------------
// Sets all GUI elements for the widget.

void ExportAiffWidget::setGui(){
  QSpacerItem* spacer_13;
  QSpacerItem* spacer_14;
  QSpacerItem* spacer_15;
  QSpacerItem* spacer_16;
  QSpacerItem* spacer_17;

  thisLayout = new QGridLayout(this ); 
 
  setMinimumSize(400,120);

  audioParamBox = new QGroupBox( this, "audioParamBox" );
  QFont audioParamBox_font(audioParamBox->font());
  audioParamBox->setFont(audioParamBox_font); 
  audioParamBox->setFrameShape( QGroupBox::Panel );
  audioParamBox->setTitle( tr( "Set audio playback parameters" ) );
  audioParamBox->setColumnLayout(0, Qt::Vertical );
  audioParamBox->setGeometry( QRect( 11, 11, 800, 588 ) ); 
  audioParamBox->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)7,
		audioParamBox->sizePolicy().hasHeightForWidth()
	)
  );
  audioParamBoxLayout = new QGridLayout( audioParamBox->layout() );
  audioParamBoxLayout->setAlignment( Qt::AlignTop );
 
  sampleRateLabel = new QLabel( audioParamBox, "sampleRateLabel" );
  QFont sampleRateLabel_font(  sampleRateLabel->font() );
  sampleRateLabel_font.setPointSize( 12 );
  sampleRateLabel->setFont( sampleRateLabel_font ); 
  sampleRateLabel->setText( tr( " Sample rate (Hz)" ) );

  audioParamBoxLayout->addWidget( sampleRateLabel, 5, 2 );

  nbOfBitsLabel = new QLabel( audioParamBox, "nbOfBitsLabel" );
  QFont nbOfBitsLabel_font(  nbOfBitsLabel->font() );
  nbOfBitsLabel_font.setPointSize( 12 );
  nbOfBitsLabel->setFont( nbOfBitsLabel_font ); 
  nbOfBitsLabel->setText( tr( " Number of sample bits" ) );

  audioParamBoxLayout->addWidget( nbOfBitsLabel, 3, 2 );
  /*
  nbOfChanLabel = new QLabel( audioParamBox, "nbOfChanLabel" );
  QFont nbOfChanLabel_font(  nbOfChanLabel->font() );
  nbOfChanLabel_font.setPointSize( 12 );
  nbOfChanLabel->setFont( nbOfChanLabel_font ); 
  nbOfChanLabel->setText( tr( " Number of channels" ) );

  audioParamBoxLayout->addWidget( nbOfChanLabel, 1, 2 );
  */

  spacer_13 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  audioParamBoxLayout->addItem( spacer_13, 2, 2 );
  spacer_14 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  audioParamBoxLayout->addItem( spacer_14, 4, 2 );
  spacer_15 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  audioParamBoxLayout->addItem( spacer_15, 6, 2 );
  spacer_16 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
  audioParamBoxLayout->addItem( spacer_16, 0, 2 );
  spacer_17 = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
  audioParamBoxLayout->addItem( spacer_17, 3, 0 );
 
  nbOfBitsSpinBox = new QSpinBox( audioParamBox, "nbOfBitsSpinBox" );
  nbOfBitsSpinBox->setMaximumSize( QSize(80, 20 ) );
  nbOfBitsSpinBox->setValue(16);
  nbOfBitsSpinBox->setLineStep(8);
  nbOfBitsSpinBox->setMinValue(8);
  nbOfBitsSpinBox->setMaxValue(24);

  audioParamBoxLayout->addWidget( nbOfBitsSpinBox, 3, 1 );

  sampleRateSpinBox = new QSpinBox( audioParamBox, "sampleRateSpinBox" );
  sampleRateSpinBox->setMaximumSize( QSize( 80, 20 ) );
  sampleRateSpinBox->setMaxValue(100000);
  sampleRateSpinBox->setMinValue(44100);
  sampleRateSpinBox->setLineStep(100);

  audioParamBoxLayout->addWidget( sampleRateSpinBox, 5, 1 );

  thisLayout->addWidget( audioParamBox, 0, 0 ); 
}

// ---------------------------------------------------------------------------
//	ExportDialog constructor
// ---------------------------------------------------------------------------
// This class is implemented to avoid repeated code in ExportAiffDialog and 
// ExportSdifDialog. (Might be unnecessary). The dialog is modal, the user have
// to finish the operation before selecting another window.

ExportDialog::ExportDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):QFileDialog(parent, name, TRUE){
  statusbar = status;
  QStringList filter;
  setFilters(filter);
  QFileDialog::Mode mode = QFileDialog::AnyFile;
  setMode(mode);
  setSelection(soundList->getCurrentName());
}

// ---------------------------------------------------------------------------
//	startDialog
// ---------------------------------------------------------------------------
// If the user has specified a file name when clicking on save button, the path 
// and name of the file is set and the method returns true. 

bool ExportDialog::startDialog(QString fileType){
  if(exec() == QDialog::Accepted){
    QString dir  = dirPath();
    path         = selectedFile();
    ending       = path;

    ending.remove(0, ending.length()-5);

    if(ending != fileType){
      path.append(fileType);
    }

    return true;
  }
  else
    return false;
}

// ---------------------------------------------------------------------------
//	ExportAiffDialog
// ---------------------------------------------------------------------------
// Inherits ExportDialog and provides the user with a dialog for exporting
// sound to an aiff file. 

ExportAiffDialog::ExportAiffDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):ExportDialog(parent, name, soundList, status){
  ExportAiffWidget* exportAiffWidget = new ExportAiffWidget(this,"ok");

  addWidgets(0, exportAiffWidget, 0);
  resize(400,300);
  addFilter("Audio file (*.aiff)");

  if(startDialog(".aiff")){ 
    double sampleRate = exportAiffWidget->getSampleRate();
    int bitsPerSamp   = exportAiffWidget->getNbOfBits();

    try{
      soundList->exportAiff(sampleRate, bitsPerSamp, path);
      statusbar->message("Export sound to "+path+", successfully.");
    }
    catch(...){
      statusbar->message("could not export sound to "+path+", please try again");
    }
  }
}

// ---------------------------------------------------------------------------
//	ExportSdifDialog
// ---------------------------------------------------------------------------
// Inherits ExportDialog and provides the user with a dialog for exporting
// sound to a sdif file. 

ExportSdifDialog::ExportSdifDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):ExportDialog(parent, name, soundList, status){
  addFilter("Sound file (*.sdif)");
 
  if(startDialog(".sdif")){ 
    try{
       soundList->exportSdif(path);
     }
     catch(...){
       statusbar->message("could not export file to sdif file, please try again");
     }
  }
}
