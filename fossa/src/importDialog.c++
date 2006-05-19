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
 * importDialog.c++ 
 *
 *
 * The ImportAiff class provides the user with a dialog for importing aiff file formats
 * into a collection of partials. When importing an aiff file, the file samples
 * get analyzed according to user specified parameters. Inherits ImportDialog.
 *
 * ImportSdif differs from the ImportAiff class only in the lack of user specified
 * parameters.  Sdif files need not to be analysed since they already are a
 * collection of sound. Inherits ImportDialog.
 *
 * The AnalyzeAiffWidget consists of 2 sliders and 2 spinboxes for specifying 
 * analysis parameters and are used in the importAiffDialog.  
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "importDialog.h"
#include "soundList.h"

#include <qlayout.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qwidget.h>
#include <qwhatsthis.h>

/*
---------------------------------------------------------------------------
	AnalyzeAiffWidget constructor
---------------------------------------------------------------------------
Creates a widget with 2 sliders and 2 spinboxes, available for users to 
specify analysis parameters frequency resolution and window width.
*/
AnalyzeAiffWidget::AnalyzeAiffWidget(
	QWidget*	parent,
	char*		name,
	SoundList*	pList
):QWidget(parent,name){
  setGui();
  addWhatIsThis();
}

/*
---------------------------------------------------------------------------
	getResolution
---------------------------------------------------------------------------
Returns the frequency resolution specified by the resolutionSlider and 
ResolutionSpinbox.  Used for analysis.
*/
double AnalyzeAiffWidget::getResolution(){
  //value() method is not always correct
  return (resolutionSpinBox->text()).toDouble();
}

/*
---------------------------------------------------------------------------
	getWidth
---------------------------------------------------------------------------
Returns the window width specified by the widthSlider and widthSpinbox. 
Used for analysis.
*/
double AnalyzeAiffWidget::getWidth(){
  return (widthSpinBox->text()).toDouble();  //value() method is not always correct
}

/*
---------------------------------------------------------------------------
	setGui
---------------------------------------------------------------------------
Adds all GUI components of the widget.
*/
void AnalyzeAiffWidget::setGui(){
  resize(200,200);
 
  layout = new QGridLayout(this); 
  layout->setSpacing(0);
  layout->setMargin(0);
  
  parameterGroup = new QGroupBox(this, "parameterGroup");
  parameterGroup->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)7,
		parameterGroup->sizePolicy().hasHeightForWidth()
	)
  );
  parameterGroup->setTitle( tr( "parameter configuration for analysis"));
  parameterGroup->setColumnLayout(0, Qt::Vertical );
  parameterGroup->layout()->setSpacing(0);
  parameterGroup->layout()->setMargin(0);
  QFont parameterGroup_font(parameterGroup->font());
  parameterGroup_font.setPointSize(12);
  parameterGroup->setFont(parameterGroup_font); 
  parameterGroupLayout = new QGridLayout( parameterGroup->layout());
  parameterGroupLayout->setAlignment( Qt::AlignTop );
  parameterGroupLayout->setSpacing(6);
  parameterGroupLayout->setMargin(11);

  sliderLayout = new QGridLayout; 
  sliderLayout->setSpacing(6);
  sliderLayout->setMargin(0);

  resolutionSlider = new QSlider(parameterGroup, "resolutionSlider");
  resolutionSpinBox = new QSpinBox(parameterGroup, "resolutionSpinBox");
  resolutionLabel = new QLabel( parameterGroup, "resolutionLabel" );
  sliderLayout->addWidget(resolutionSlider, 0, 0);
  sliderLayout->addWidget(resolutionSpinBox, 0, 1);
  sliderLayout->addWidget(resolutionLabel, 0, 2 );
  setSliders(
	resolutionSlider,
	resolutionSpinBox,
	0,
	1000,
	100,
	resolutionLabel,
	"Resolution (Hz)"
  );

  widthSlider = new QSlider(parameterGroup, "widthSlider");
  widthSpinBox = new QSpinBox(parameterGroup, "widthSpinBox");
  widthLabel = new QLabel( parameterGroup, "widthLabel" );
  sliderLayout->addWidget(widthSlider, 3, 0 );
  sliderLayout->addWidget(widthSpinBox, 3, 1);
  sliderLayout->addWidget( widthLabel, 3, 2 );
  setSliders(
	widthSlider,
	widthSpinBox,
	0,
	1000,
	100,
	widthLabel,
	"Window Width (Hz)"
  );

  parameterGroupLayout->addMultiCellLayout(sliderLayout, 2, 2, 0, 1);
 
  layout->addMultiCellWidget( parameterGroup, 1, 2, 0, 4 );
}

/*
---------------------------------------------------------------------------
	setSliders
---------------------------------------------------------------------------
Help method for setting the characteristics of the sliders and spinboxes, and the
connection between them.
*/
void AnalyzeAiffWidget::setSliders(
	QSlider*	s,
	QSpinBox*	sp,
	int		minVal,
	int		maxVal,
	int		def,
	QLabel*		l,
	char*		t
){
 
  s->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)0,
		s->sizePolicy().hasHeightForWidth()
	)
  );
  s->setMinimumSize(QSize(250, 20));
  s->setMaximumSize(QSize(2000, 20));
  QFont slidFont(s->font());
  slidFont.setPointSize(12);
  s->setFont(slidFont); 
  s->setMinValue(minVal);
  s->setMaxValue(maxVal);
  s->setValue(def);
  s->setOrientation(QSlider::Horizontal);
  s->setTickmarks(QSlider::Right);
  s->setTickInterval((maxVal-minVal)/20);
  s->setTracking(true);

  sp->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)0,
		(QSizePolicy::SizeType)4,
		sp->sizePolicy().hasHeightForWidth()
	)
  );
  sp->setMinimumSize(QSize(60, 20));
  sp->setMaximumSize(QSize(60, 20));
  sp->setMinValue(minVal);
  sp->setMaxValue(maxVal);
  sp->setValue(def);
  QFont spinFont(sp->font());
  spinFont.setPointSize(12);
  sp->setFont(spinFont); 

  l->setSizePolicy(
	QSizePolicy(
		(QSizePolicy::SizeType)7,
		(QSizePolicy::SizeType)7,
		l->sizePolicy().hasHeightForWidth()
	)
  );
  QFont l_font(l->font());
  l_font.setPointSize(12);
  l->setFont(l_font); 
  l->setMinimumSize(QSize(220, 20));
  l->setText(t);

  connect(sp, SIGNAL(valueChanged(int)),  s, SLOT(setValue(int)));
  connect(s, SIGNAL(valueChanged(int)), sp, SLOT(setValue(int)));
}

/*
---------------------------------------------------------------------------
	addWhatIsThis
---------------------------------------------------------------------------
Adds text to the GUI shown when whatIsThis mode is enabled in the help menu.
For the moment the dialogs are modal and the text can not be reached, if the
dialogs would be none modal we make more use of this functionality.
*/
void AnalyzeAiffWidget::addWhatIsThis(){
  QString resolutionText = "The frequency resolution controls the frequency density of partials.  Two partials will differ in frequency by no less than the specified frequency resolution.  The frequency should be slightly less than the anticipated partial frequency density. For quasi-harmonic sounds, the anticipated partial frequency density is equal to the fundamental frequency, and the frequency resolution is typically set to 70% to 85% of the fundamental frequency. For non-harmonic sounds, some experimentation may be necessary, and intuition can offen be obtained using a spectrogram tool";
  
  QString widthText = "For quasi-harmonic sounds, the window width is set equal to the fundamental frequency, but it is rarely necessary to use windows wider than than 500Hz. Similarly, for very low-frequency quasi-harmonic sounds, best results are often obtained using windows as wide as 120Hz.";

  QWhatsThis::add(resolutionSlider, resolutionText); 
  QWhatsThis::add(resolutionSpinBox, resolutionText);
  QWhatsThis::add(resolutionLabel, resolutionText);
  QWhatsThis::add(widthSlider, widthText);
  QWhatsThis::add(widthSpinBox, widthText);
  QWhatsThis::add(widthLabel, widthText);
}

/*
---------------------------------------------------------------------------
	addToolTips
---------------------------------------------------------------------------
Not implemented yet.
*/
void AnalyzeAiffWidget::addToolTips(){}

/*
---------------------------------------------------------------------------
	ImportDialog constructor
---------------------------------------------------------------------------
This class is implemented to avoid repeated code in ImportAiffDialog and 
ImportSdifDialog. (Might be unnecessary). The dialog is modal, the user have
to finish the operation before selecting another window.
*/
ImportDialog::ImportDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):QFileDialog(parent, name, TRUE){
  statusbar = status;
  QStringList filter;
  setFilters(filter);
  QFileDialog::Mode mode = QFileDialog::ExistingFile;
  setMode(mode);
}

/*
---------------------------------------------------------------------------
	startDialog
---------------------------------------------------------------------------
If the user has selected a file when the open button the path and name of 
the file is set and the method returns true. 
*/
bool ImportDialog::startDialog(){
  if(exec() == QDialog::Accepted){
    QString dir  = dirPath();
    path         = selectedFile();
    name         = path;
   
    name.remove(0, dir.length()+1);
    return true;
  }
  else
    return false;
};

/*
---------------------------------------------------------------------------
	ImportAiffDialog
---------------------------------------------------------------------------
Inherits ImportDialog and provides the user with a dialog for importing
and analyzing aiff files. 
*/
ImportAiffDialog::ImportAiffDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):ImportDialog(parent, name, soundList, status){
  AnalyzeAiffWidget* widget = new AnalyzeAiffWidget(this,"ok", soundList);

  addWidgets(0, widget, 0);
  addFilter("Audio file (*.aiff)");
 
  if(startDialog()){ 
    try{
      double resolution = widget->getResolution();
      double width      = widget->getWidth();
      QString temp = "";
      statusbar->message(
	"Importing and analyzing "
	+ImportDialog::name
	+", with frequency resolution: " 
	+ temp.setNum(resolution)
	+ " and window width: " 
	+ temp.setNum(width)
      );

      soundList->importAiff(
	ImportDialog::path,
	ImportDialog::name,
	resolution,
	width
      );

      statusbar->message("Imported "
	+ImportDialog::name
	+" successfully.", 5000);
    }

    catch(...){
      statusbar->message(
	"Could not open "
	+ImportDialog::name
	+", please try again."
      );
    }
  }
}

/*
---------------------------------------------------------------------------
	ImportSdifDialog
---------------------------------------------------------------------------
Inherits ImportDialog and provides the user with a dialog for importing
sdif files. 
*/
ImportSdifDialog::ImportSdifDialog(
	QWidget*	parent,
	char*		name,
	SoundList*	soundList,
	QStatusBar*	status
):ImportDialog(parent, name, soundList, status){
  resize(500, 300);
  addFilter("Audio file (*.sdif)");
  
  if(startDialog()){ 
    try{
      soundList->importSdif(ImportDialog::path, ImportDialog::name);
      statusbar->message("Imported "+ImportDialog::name+" successfully.", 5000);
    }
    catch(...){
      statusbar->message(
	"Could not open "
	+ImportDialog::name
	+", please try again."
      );
    }
  }
}
