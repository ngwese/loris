#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

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
 * exportDialog.h++
 *
 * Definition of class ExportDialog, ExportAiffDialog, ExportSdifDialog, 
 * and ExportAiffWidget. ExportDialog is basically a class made to avoid
 * unnecessary repetition of code in ExportAiffDialog and ExportSdifDialog.
 * ExportAiffWidget is inserted in ExportAiffDialog to enable the user to 
 * specify sample rate, number of channels, and number of sample bits, of
 * synthesized sound.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qwidget.h>
#include <qfiledialog.h>

class QVBoxLayout; 
class QGridLayout; 
class QLabel;
class QStatusBar;
class QSpinBox;
class QGroupBox;

class SoundList;

// ---------------------------------------------------------------------------
// class ExportAiffWidget
//
// Contains the GUI for specifying sample rate, and number
// of sample bits for synthesis of a collection of sound.

class ExportAiffWidget:public QWidget{ 
  Q_OBJECT

  public:
    ExportAiffWidget(QWidget* parent, char* name);
    int			getSampleRate();
    int			getNbOfBits();
    bool		getFileType(); //aiff = true;

  private:
    void		setGui();

    QGridLayout*	thisLayout;
    QVBoxLayout*	paramLayout;
    QGridLayout*	audioParamBoxLayout;

    QGroupBox*		audioParamBox;
    QSpinBox*		nbOfBitsSpinBox;
    QSpinBox*		sampleRateSpinBox;
    QLabel*		sampleRateLabel;
    QLabel*		nbOfBitsLabel;
};

// class ExportDialog
//
// Provided to implement common code used in both ExportAiffDialog and 
// ExportSdifDialog

class ExportDialog:public QFileDialog{ 
  Q_OBJECT  

  public:
    ExportDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	statusbar
    );

  protected:
    QStringList filter;
    QString path;
    QString ending;
    QStatusBar* statusbar;
    bool startDialog(QString fileType);
};

// ---------------------------------------------------------------------------
// class ExportAiffDialog
//
// A Dialog which lets the user export sound to an aiff file

class ExportAiffDialog:public ExportDialog{ 
  Q_OBJECT  

  public:
    ExportAiffDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	statusbar
	);

  signals: 
    void exportAiff(double, int, int, const char* name);
};

// ---------------------------------------------------------------------------
// class ExportSdifDialog
//
// A Dialog which lets the user export sound to an sdif file

class ExportSdifDialog:public ExportDialog{ 
  Q_OBJECT  

  public:
    ExportSdifDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	statusbar
    );

  signals:
    void exportSdif(const char* name);
};

#endif // EXPORT_DIALOG_H
