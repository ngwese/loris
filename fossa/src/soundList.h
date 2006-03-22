#ifndef SOUND_LIST_H
#define SOUND_LIST_H

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
 * soundList.h
 *
 * Definition of the SoundList class, the model of the application. 
 * 
 * Susanne Lefvert, 1 March 2002
 */

#include <qlist.h>
#include <qobject.h>

#include "sound.h"

#include <Partial.h>
#include <Exception.h>

#include <list>

using namespace Loris;

// ---------------------------------------------------------------------------
// class SoundList
// The SoundList class is the main model of the application, ie. changes in 
// SoundList will trigger updates in view classes. This class has a list
// of imported sounds as well as sounds produced from different 
// manipulations. One sound in the list is always set to current sound, 
// and modifications can be made only to current sound. 
// SoundList also keeps track of two sound which are selected when 
// the user decides to make a morph between two sounds. The class 
// communicates with LorisInterface for necessary operations on sound.

class SoundList:public QObject{
  Q_OBJECT;
 
  public:
    SoundList();
    ~SoundList();
    QString		getCurrentName();
    int			getCurrentIndex();
    double		getCurrentDuration();
    double		getCurrentMax(Sound::ValType t);
    int			getCurrentNrOfPartials();
    bool		isCurrentDistilled();
    bool		isCurrentChannelized();
    std::list<Partial>*	getCurrentPartials();
    const Sound*	getSound(int position);
    double		getMorph1Duration();
    double		getMorph2Duration();
    bool		isEmpty();
    int			getLength();
    void		setCurrentSound(int position);
    void		setMorphSound1(int position);
    void		setMorphSound2(int position);
    void		shiftCurrentAmplitude(double val); 
    void		shiftCurrentFrequency(double val);
    void		shiftCurrentNoise(double val); 
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
     QList<Sound>	soundList;
     int		morph1;
     int		morph2;
     int		current;
     LorisInterface*	interface;
     bool		inList(int position);
};
#endif	//SOUND_LIST_H
