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
 * dilateDialog.c++
 *
 * GUI container for the DilateArea view window and controls for dilating
 * sounds.
 *
 *
 * Chris Hinrichs
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "dilateDialog.h"
#include "dilateArea.h"
#include "soundList.h"

#include <qlayout.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qcanvas.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlabel.h>

using namespace Loris;

/*
--------------------------------------------------------------------------------
	DilateDialog constructor
--------------------------------------------------------------------------------
*/
DilateDialog::DilateDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	pList,
	QStatusBar*	status
):QDialog(parent, name, TRUE){
  statusbar	= status;
  soundList	= pList;
//  canvas	= new QCanvas(735, 350);
  canvas1	= new QCanvas(735, 175);
  canvas2	= new QCanvas(735, 175);
  sound1	= "";
  sound2	= "";

  dilatePos1 = dilatePos2 = -1;

  setGui();
  setConnections();
  setLists();
  show();
}

/*
--------------------------------------------------------------------------------
	setLists
--------------------------------------------------------------------------------
Initialize the popup lists which select sounds to be dilated.
*/
void DilateDialog::setLists(){
  int i;
  int current;

  sound1List->clear();
  sound2List->clear();

  for(i=0; i<soundList->getLength(); i++){
    sound1List->insertItem(soundList->getSound(i)->getName(), i);
    sound2List->insertItem(soundList->getSound(i)->getName(), i);
  }

  current = soundList->getCurrentIndex();

  sound1List->setCurrentItem(current);
  sound2List->setCurrentItem(current);
}



/*
--------------------------------------------------------------------------------
	setConnections
--------------------------------------------------------------------------------
Connect SLOTs to SOCKETs so that QT GUI objects can pass each other messages.
*/
void DilateDialog::setConnections(){

  connect(dilateButton,SIGNAL(clicked()), this, SLOT(dilate()));
  connect(dilateButton,SIGNAL(clicked()), this, SLOT(hide()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
  connect(sound1List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate1(int)));
  connect(sound2List, SIGNAL(highlighted(int)),
          this, SLOT(updateDilate2(int)));
}

/*
--------------------------------------------------------------------------------
	dilate
--------------------------------------------------------------------------------
*/
void DilateDialog::dilate(){
  int i=0;

  i++;

  return;
}


/*
--------------------------------------------------------------------------------
	updateDilate1
--------------------------------------------------------------------------------
Selects a new sound for dilation, and updates all GUI elements in DilateDialog
to reflect the change.
*/
void DilateDialog::updateDilate1(int pos){
  dilatePos1 = pos;
  sound1 = sound1List->text(pos);
  name1Label->setText(sound1);
  dilateArea1->setSound(sound1, pos);
  dilateBox->setTitle(QString("Dilate "+sound2+" onto "+sound1));
}


/*
--------------------------------------------------------------------------------
	updateDilate2
--------------------------------------------------------------------------------
Selects a new sound for dilation, and updates all GUI elements in DilateDialog
to reflect the change.
*/
void DilateDialog::updateDilate2(int pos){
  dilatePos2 = pos;
  sound2 = sound2List->text(pos);
  name2Label->setText(sound2);
  dilateArea2->setSound(sound2, pos);
  dilateBox->setTitle(QString("Dilate "+sound2+" onto "+sound1));
}


/*
--------------------------------------------------------------------------------
	setGui
--------------------------------------------------------------------------------
Set up all the GUI elements.
*/
void DilateDialog::setGui(){
  QSpacerItem* spacer_0;
  QSpacerItem* spacer_16;


  dialogLayout = new QGridLayout(this);
  dialogLayout->setSpacing(6);
  dialogLayout->setMargin(20);

  //Below that put the box around the 2 ComboBoxes where the sounds are chosen.
  soundBox = new QGroupBox(this, "soundBox" );
  QFont soundBox_font(  soundBox->font() );
  soundBox_font.setPointSize( 12 );
  soundBox->setFont( soundBox_font );
  soundBox->setTitle( tr( "Select sounds to dilate" ) );
  soundBox->setColumnLayout(0, Qt::Vertical );
  soundBox->layout()->setSpacing( 0 );
  soundBox->layout()->setMargin( 0 );

  soundBoxLayout = new QGridLayout( soundBox->layout() );
  soundBoxLayout->setAlignment( Qt::AlignTop );
  soundBoxLayout->setSpacing( 6 );
  soundBoxLayout->setMargin( 11 );


  //Set up the 2 ComboBoxes.
  sound1List = new QComboBox( FALSE, soundBox, "sound1List" );
  sound1List->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)7,
                (QSizePolicy::SizeType)0,
                sound1List->sizePolicy().hasHeightForWidth()
        )
  );

  sound1List->setMaximumSize( QSize( 32767, 20 ) );

  soundBoxLayout->addWidget( sound1List, 1, 2 );

  sound2List = new QComboBox( FALSE, soundBox, "sound2List" );
  sound2List->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)7,
                (QSizePolicy::SizeType)0,
                sound2List->sizePolicy().hasHeightForWidth()
        )
  );

  sound2List->setMaximumSize( QSize( 32767, 20 ) );

  soundBoxLayout->addWidget( sound2List, 1, 6 );

  //Label that says "Dilate" to the left of the first ComboBox.
  sound1Label = new QLabel( soundBox, "sound1Label" );
  QFont sound1Label_font(  sound1Label->font() );
  sound1Label_font.setPointSize( 12 );
  sound1Label->setFont( sound1Label_font );
  sound1Label->setText( tr( "Dilate" ) );

  soundBoxLayout->addWidget( sound1Label, 1, 0 );

  //Label that says "with" to the right of the first ComboBox.
  sound2Label = new QLabel( soundBox, "sound2Label" );
  QFont sound2Label_font(  sound2Label->font() );
  sound2Label_font.setPointSize( 12 );
  sound2Label->setFont( sound2Label_font );
  sound2Label->setText( tr( "with" ) );
  soundBoxLayout->addWidget( sound2Label, 1, 4 );

  //Spacer to the right of the Sound Box.
  //NOTE: Don't yet know if this is strictly necessary...
  spacer_16 = new QSpacerItem(
        21,
        20,
        QSizePolicy::Fixed,
        QSizePolicy::Minimum
  );
  soundBoxLayout->addMultiCell( spacer_16, 1, 2, 1, 1 );


  //And that's it, put the soundBox in the dialogLayout.
  dialogLayout->addWidget(soundBox, 1, 0);



  //Add another spacer between the soundBox and the dilateBox
  spacer_0 = new QSpacerItem(
        150,
        20,
        QSizePolicy::Minimum,
        QSizePolicy::Expanding
  );
  dialogLayout->addItem(spacer_0, 2, 1);



  //Below the soundBox is the dilateBox which has the 2 SoundPlots.
  dilateBox = new QGroupBox(this, "dilateBox" );
  dilateBox->setColumnLayout(0, Qt::Vertical);
  dilateBox->layout()->setSpacing(0);
  dilateBox->layout()->setMargin(0);

  dilateBoxLayout = new QGridLayout(dilateBox->layout());
  dilateBoxLayout->setAlignment(Qt::AlignTop);
  dilateBoxLayout->setSpacing(6);
  dilateBoxLayout->setMargin(11);


  //There are 2 dilateAreas, 1 for each sound.
  dilateArea1 = new DilateArea(
    canvas1,
    dilateBox,
    "dilateArea1",
    soundList,
    statusbar,
    1
  );

  dilateArea1->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)0,
                (QSizePolicy::SizeType)0,
                dilateArea1->sizePolicy().hasHeightForWidth()
        )
  );

  //Note: QCanvas size is 735 x 175
  dilateArea1->setMinimumSize(QSize(740, 180));
  dilateArea1->setMaximumSize(QSize(740, 180));
  dilateBoxLayout->addWidget(dilateArea1, 1, 0);


  dilateArea2 = new DilateArea(
    canvas2,
    dilateBox,
    "dilateArea2",
    soundList,
    statusbar,
    2
  );

  dilateArea2->setSizePolicy(
        QSizePolicy(
                (QSizePolicy::SizeType)0,
                (QSizePolicy::SizeType)0,
                dilateArea2->sizePolicy().hasHeightForWidth()
        )
  );

  //Note: QCanvas size is 735 x 175
  dilateArea2->setMinimumSize(QSize(740, 180));
  dilateArea2->setMaximumSize(QSize(740, 180));
  dilateBoxLayout->addWidget(dilateArea2, 2, 0);



  //This label has the name of the first sound, and goes above the dilateArea.
  name1Label = new QLabel(dilateBox, "name1Label");
  QFont name1Label_font(name1Label->font());
  name1Label_font.setPointSize(10);
  name1Label->setFont(name1Label_font);
  dilateBoxLayout->addWidget(name1Label, 0, 0);



  //This label has the name of the second sound, and goes below the dilateArea.
  name2Label = new QLabel( dilateBox, "name2Label" );
  QFont name2Label_font(  name2Label->font() );
  name2Label_font.setPointSize(10);
  name2Label->setFont(name2Label_font);
  name2Label->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
  dilateBoxLayout->addWidget( name2Label, 3, 0 );




  dilateButton = new QPushButton( dilateBox, "dilateButton" );
  QFont dilateButton_font(  dilateButton->font() );
  dilateButton_font.setPointSize( 12 );
  dilateButton->setFont( dilateButton_font );
  dilateButton->setText( tr( "DILATE" ) );

  dilateBoxLayout->addWidget( dilateButton, 4, 0 );


  cancelButton = new QPushButton( dilateBox, "cancelButton" );
  QFont cancelButton_font(  cancelButton->font() );
  cancelButton_font.setPointSize( 12 );
  cancelButton->setFont( cancelButton_font );
  cancelButton->setText( tr( "CANCEL" ) );

  dilateBoxLayout->addWidget( cancelButton, 4, 1 );


  //That's it, add the dilateBox to the dialog layout.
  dialogLayout->addMultiCellWidget(dilateBox, 3, 7, 0, 2);
}
