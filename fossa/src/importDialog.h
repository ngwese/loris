#ifndef IMPORT_DIALOG_H
#define IMPORT_DIALOG_H

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
 * importDialog.h++
 *
 * Definition of class ImportDialog, ImportAiffDialog, ImportSdifDialog, 
 * and AnalyzerWidget. ImportDialog is basically a class made to avoid
 * unnecessary repetition of code in ImportAiffDialog and ImportSdifDialog.
 * AnalyzerWidget is inserted in ImportAiffDialog to enable the user to 
 * specify the analysis parameters, frequency resolution and window width.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qwidget.h>

class QGridLayout; 
class QFrame;
class QGroupBox;
class QLabel;
class QSlider;
class QSpinBox;
class QFileDialog;
class QStatusBar;

class AnalyzeDialog;
class PartialsList;

#include <qfiledialog.h> 

// ---------------------------------------------------------------------------
// class AnalyzeAiffWidget
//
// Contains the sliders for specifying the analysis parameters, frequency resolution 
// and window width.

class AnalyzeAiffWidget:public QWidget{ 
 
  Q_OBJECT

 public:
  AnalyzeAiffWidget(QWidget* parent, char* name, PartialsList* pList);
      
  double getResolution();
  double getWidth();

 private:
 QGridLayout* layout;
 QGroupBox* parameterGroup;
 QGridLayout* parameterGroupLayout;
 QGridLayout* sliderLayout;
 
 QSpinBox* resolutionSpinBox;
 QSlider* resolutionSlider;
 QLabel* resolutionLabel;
 
 QFrame* sliderLine;
 
 QSlider* widthSlider;
 QLabel* widthLabel;
 QSpinBox* widthSpinBox;

 QStatusBar* statusbar;

 void setGui();
 void addToolTips();
 void addWhatIsThis();
 void setSliders(QSlider* s, QSpinBox* sp, int minVal, int maxVal, int def, QLabel* l, char* text);
};

// ---------------------------------------------------------------------------
// class ImportDialog
//
// Provided to implement common code used in both ImportAiffDialog and 
// ImportSdifDialog

class ImportDialog:public QFileDialog{ 

  Q_OBJECT  
  
 public:
  ImportDialog(QWidget* parent,  char* name, PartialsList* pList, QStatusBar* status);
 
 protected:
  QStringList filter;
  QString path;
  QString name;
  QStatusBar* statusbar;
  
  bool startDialog();
};

// ---------------------------------------------------------------------------
// class ImportAiffDialog
//
// A Dialog which lets the user import an aiff file and at the same time analyse
// the file samples with given parameters, frequency resolution and window width.

class ImportAiffDialog:public ImportDialog{ 

  Q_OBJECT  
  
 public:
  ImportAiffDialog(QWidget* parent,  char* name, PartialsList* pList, QStatusBar* status);
  void importAiffFile(QString path, QString name, double resolution, double width);
};

// ---------------------------------------------------------------------------
// class ImportSdifDialog
//
// A Dialog which lets the user import an sdif file 

class ImportSdifDialog:public ImportDialog{ 

  Q_OBJECT  
  
 public:
  ImportSdifDialog(QWidget* parent,  char* name, PartialsList* pList, QStatusBar* status);
  void importSdifFile(QString path, QString name);
};


#endif // IMPORT_DIALOG_H

















