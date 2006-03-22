#ifndef NEW_NAME_DIALOG_H
#define NEW_NAME_DIALOG_H

#include <qvariant.h>
#include <qdialog.h>

#include "soundList.h"

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QLineEdit;
class QStatusBar;
class QPushButton;

class NewNameDialog : public QDialog{ 
  Q_OBJECT

  public:
    NewNameDialog( 
	QWidget*	parent, 
	const char*	name, 
	SoundList*	soundList, 
	QStatusBar*	statusbar
    );
   
  private slots:
    void 		rename();

  private:
    QLabel*		newNameText;
    QLineEdit*		newNameInput;
    QGridLayout*	newNameDialogLayout;
    SoundList*	soundList;
    QStatusBar*		statusbar;
    QPushButton*	okButton;
    QPushButton*	cancelButton;

    void		setGui();
    void		setConnections();
};

#endif // NEW_NAME_DIALOG_H
