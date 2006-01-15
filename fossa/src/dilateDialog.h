#ifndef DILATE_DIALOG_H
#define DILATE_DIALOG_H

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
 * DilateDialog.h
 *
 * Chris Hinrichs, 1/10/2006
 *
 */

#include "partialsList.h"
#include <qdialog.h>

class QGridLayout;
class QFrame;
class QGroupBox;
class QButtonGroup;
class QPushButton;
class QRadioButton;
class QStatusBar;
class QComboBox;
class QLabel;
class QCanvas;

class PartialsList;
class DilateArea;

/*
--------------------------------------------------------------------------------
class DilateDialog
This is the dialog window which contains the dilation view object, and which
allows the user to dilate (timewise)one sample onto another one. The user
selects points in time by clicking on the dilateArea. The points are then matched
from one sample to the other.
*/
/*
class DilateDialog:public QDialog{
  Q_OBJECT

  public:
    DilateDialog(
	QWidget*	parent,
	char*		name,
	PartialsList*	list,
	QStatusBar*	status
    );

  private slots:
    void                updateDilate1(int pos);
    void                updateDilate2(int pos);


  private:
    QCanvas*		canvas;
    QStatusBar*		statusbar;
    PartialsList*	partialsList;
    DilateArea*		dilateArea;
    QGridLayout*	dialogLayout;

    QLabel*		sample1Label;
    QLabel*		sample2Label;
    QComboBox*		sample1List;
    QComboBox*		sample2List;

    QPushButton*	dilateButton;
    QPushButton*	cancelButton;

    QString		sample1;
    QString		sample2;

    void		setConnections;
    void		setLists;
    void		setGui;
};
*/
#endif // DILATE_DIALOG_H
