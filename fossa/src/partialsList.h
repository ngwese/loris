#ifndef PARTIALS_LIST_H
#define PARTIALS_LIST_H

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
 * partialsList.h
 *
 * Definition of the PartialsList class, the model of the application. 
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <qlist.h>
#include <qobject.h>
#include <qpixmap.h>

#include "partials.h"

#include <Partial.h>
#include <Exception.h>

#include <list>

using namespace Loris;

// ---------------------------------------------------------------------------
// class PartialsList
// The PartialsList class is the main model of the application, ie. changes in 
// PartialsList will trigger updates in view classes. This class has a list
// containing imported partials and partials produced from different 
// manipulations. One partials in the list is always set to current partials, 
// and modifications can be made only to current partials. 
// PartialsList also keeps track of two partials which are selected when 
// the user decides to make a morph between two sounds. The class 
// communicates with LorisInterface for necessary operations on partials.
// 

class PartialsList:public QObject{
  Q_OBJECT;
 
  public:
    PartialsList();
    ~PartialsList();
    QString		getCurrentName();
    int			getCurrentIndex();
    double		getCurrentDuration();
    double		getCurrentMaxAmplitude();
    double		getCurrentMaxFrequency();
    double		getCurrentMaxNoise();
    int			getCurrentNrOfPartials();
    bool		isCurrentDistilled();
    bool		isCurrentChannelized();
    QPixmap		getCurrentAmplitudePixmap();
    QPixmap		getCurrentFrequencyPixmap();
    QPixmap		getCurrentNoisePixmap();
    Partials::State	getCurrentState();  
    std::list<Partial>*	getCurrentPartials();
    const Partials*	getPartials(int position);//not be able to distill, channelize etc
    double		getMorph1Duration();
    double		getMorph2Duration();
    bool		isEmpty();
    int			getLength();
    void		setCurrentPartials(int position);
    void		setMorphPartials1(int position);
    void		setMorphPartials2(int position);
    void		shiftCurrentAmplitude(int value); 
    void		shiftCurrentFrequency(int value);
    void		shiftCurrentNoise(int value); 
    void		setCurrentState(Partials::State state);  
    void		distillCurrent();
    void importAiff(
	QString	path,
	QString	name,
	double	resolution,
	double	width
    );
    void importSdif(
	QString path,
	QString name
    );
    void channelizeCurrent(
	int refLabel,
	double minFreq,
	double maxFreq
    );
    void exportAiff(
	double sampleRate,
	int sampleBits,
	const char* name
    );
    void		exportSdif(const char* name);
    void		renameCurrent(QString newName);
    void		copyCurrent();
    void		removeCurrent();
    void		playCurrent();
    void morph(
	LinearEnvelope&		amp,
	LinearEnvelope&		fre,
	LinearEnvelope&		noise
    );
  
    //void		dilate() should be implemented!
  
  signals:
    void		listChanged();
    void		currentChanged();

   private:
     QList<Partials>	partialsList;
     int		morph1;
     int		morph2;
     int		current;
     LorisInterface*	interface;
     bool		inList(int position);
};
#endif
