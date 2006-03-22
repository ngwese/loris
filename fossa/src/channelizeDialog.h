#ifndef CHANNELIZATION_H
#define CHANNELIZATION_H

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
 * channelizeDialog.h
 *
 * Definition of class ChannelizeDialog, a dialog provided to let a user
 * specify channelization parameters, minimum frequency, maximum frequency,
 * and reference label.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qdialog.h>
class QGridLayout; 
class QGroupBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QStatusBar;
class SoundList;

// ---------------------------------------------------------------------------
// class  ChannelizeDialog
//
// Dialog for specifying channelization parameters. 
class ChannelizeDialog : public QDialog{ 
  Q_OBJECT
      
  public:
    ChannelizeDialog(
	QWidget*	parent, 
	const char* 	name, 
	SoundList*	soundList, 
	QStatusBar*	status
    );
    
  private slots:
    void	 channelizing();

  private:
    QStatusBar*		statusbar;
    QGroupBox*		paramBox;
    QSpinBox*		maxSpinBox;
    QLabel*		maxLabel;
    QLabel*		minLabel;
    QLabel*		nrLabel;
    QSpinBox*		refSpinBox;
    QSpinBox*		nrSpinBox;
    QSpinBox*		minSpinBox;
    QLabel*		refLabel;
    QPushButton*	channelizeButton;
    QPushButton*	cancelButton;
    QGridLayout*	ChannelizationLayout;
    QGridLayout*	paramBoxLayout;
 
    SoundList*	soundList;
 
    void		setGui();
    void		setConnections();
};
#endif // CHANNELIZATION_H
