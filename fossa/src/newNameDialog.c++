#include "newNameDialog.h"

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif


NewNameDialog::NewNameDialog( QWidget* parent, const char* name,  PartialsList* pList, QStatusBar* sBar): QDialog( parent, name, TRUE){
  
  partialsList = pList;
  statusbar    = sBar;

  if ( !name )
    setName( "newNameDialog" );
  resize(450, 80); 
  setGui();
  setConnections();
  show();
}

void NewNameDialog::setConnections(){
  connect(okButton, SIGNAL(clicked()), this, SLOT(rename()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(hide()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(hide()));
}

void NewNameDialog::rename(){
  if(!partialsList->isEmpty()){
    partialsList->renameCurrent(newNameInput->text());
  }
}

void NewNameDialog::setGui(){
  setCaption( tr( "New name" ) );
  newNameDialogLayout = new QGridLayout( this ); 
  newNameDialogLayout->setSpacing( 6 );
  newNameDialogLayout->setMargin( 11 );
  
  newNameText = new QLabel( this, "newNameText" );
  newNameText->setText( tr( "Enter new name:" ) );
  
  newNameDialogLayout->addWidget( newNameText, 0, 0 );
  
  newNameInput = new QLineEdit( this, "newNameInput" );
  
  newNameDialogLayout->addWidget( newNameInput, 0, 1 );

  okButton = new QPushButton( this, "okButton" );
  okButton->setText( tr( "ok" ) );
  
  newNameDialogLayout->addWidget( okButton, 0, 2 );
  
  cancelButton = new QPushButton( this, "cancelButton" );
  cancelButton->setText( tr( "cancel" ) );
  
  newNameDialogLayout->addWidget( cancelButton, 0, 3 );
}




