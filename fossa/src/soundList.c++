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
 * soundList.c++
 *
 * The SoundList class is the main model of the application, ie. changes in 
 * SoundList will trigger updates in view classes. This class has a list
 * containing imported sounds and sounds produced from different 
 * manipulations. One sound in the list is always set to current sound, 
 * and modifications can be made only to the current sound. 
 * SoundList also keeps track of two sounds which are selected when 
 * the user decides to make a morph between two sounds. The class 
 * communicates with LorisInterface for necessary operations on sounds.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 */


#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "soundList.h"

using std::cout;

// ---------------------------------------------------------------------------
//	SoundList constructor
// ---------------------------------------------------------------------------
SoundList::SoundList(){
  soundList.setAutoDelete(TRUE);  // QList will delete objects automatically
  morph1    = -1;
  morph2    = -1;
  current   = -1;
  interface = new LorisInterface();
}

// ---------------------------------------------------------------------------
//	SoundList destructor
// ---------------------------------------------------------------------------
SoundList::~SoundList(){
  delete interface;
}

// ---------------------------------------------------------------------------
//	getSound
// ---------------------------------------------------------------------------
const Sound* SoundList::getSound(int pos){
  return soundList.at(pos);
}

// ---------------------------------------------------------------------------
//      importAiff
// ---------------------------------------------------------------------------
// Communicates with lorisInterface in order to import and analyse a sound file
// with given frequency resolution and window width.
// Consider combining importAiff and importSdif since they are so similar.  
// By providing resolution and width with default parameters a check of 
// parameters can be made in order to figure out what format to import.
void SoundList::importAiff(
	QString		path,
	QString		name,
	double		resolution,
	double		width
){
  try{
    //take away ending, what if no ending????
    name.remove(name.length()-5, name.length());
    list<Loris::Partial>* p = interface->importAiff(path, resolution, width); 
    Sound* newSound = new Sound(*p, name, interface); 
    soundList.append(newSound);

    current = soundList.count();

    emit listChanged();    // model changed -> update views
  }

  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      importSdif
// ---------------------------------------------------------------------------
// Communicates with lorisInterface in order to import a sound file.
// Consider combining importAiff and importSdif since they are so similar. 
// By providing resolution and width with default parameters a check 
// of parameters can be made in order to figure out what format to import.
void SoundList::importSdif(
	QString		path,
	QString		name
){
  try{
    // take away ending, what if no ending????
    name.remove(name.length()-5, name.length());
    list<Loris::Partial>* p = interface->importSdif(path);
    Sound* newSound = new Sound(*p, name, interface);
    soundList.append(newSound);
    
    current = soundList.count();

    emit listChanged();  // model changed -> update views
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      morph
// ---------------------------------------------------------------------------
// Morph 2 sound from the list with given breakpointsenvelopes for 
// amplitude, frequency, and noise.
void SoundList::morph(
	LinearEnvelope&		famp,
	LinearEnvelope&		ffreq,
	LinearEnvelope&		fbw
){
  list<Loris::Partial> list1 = *soundList.at(morph1)->getPartials();
  list<Loris::Partial> list2 = *soundList.at(morph2)->getPartials();
  QString name1       = soundList.at(morph1)->getName();
  QString name2       = soundList.at(morph2)->getName();
  
  try{
    list<Loris::Partial>* morphedSound = interface->morph(
	famp,
	ffreq,
	fbw,
	list1,
	list2
    );
    QString morphedSoundName = name1 + name2;
    soundList.append(
	new Sound(
		*morphedSound,
		morphedSoundName,
		interface
	)
    );

    emit currentChanged(); 
    emit listChanged();             // model changed -> update views
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      removeCurrent
// ---------------------------------------------------------------------------
// Remove current sound from the list
void SoundList::removeCurrent(){
  if(inList(current)){
    soundList.remove(current);
    if(current == 0 && !isEmpty()){  // if current was the first item
      current = 0;                   // but not last.
    } else {
      current --;                    
    }
    emit listChanged();
    emit currentChanged();       // model changed -> update views
  } else {
    cout<<"SoundList::removeCurrent(): current not in range, weird"<<endl;
  }
}

// ---------------------------------------------------------------------------
//      isEmpty
// ---------------------------------------------------------------------------
// Returns true if soundList is empty.
bool SoundList::isEmpty(){
  return !inList(current);
}

// ---------------------------------------------------------------------------
//      getCurrentIndex
// ---------------------------------------------------------------------------
// Returns the position of current sound. 
int SoundList::getCurrentIndex(){
  return current;
}

// ---------------------------------------------------------------------------
//      getCurrentNrOfPartials
// ---------------------------------------------------------------------------
// Returns number of partials in current sound 
int SoundList::getCurrentNrOfPartials(){
  if(inList(current))
    return soundList.at(current)->getNumberOfPartials();
  else
    cout<<"SoundList::getCurrentNrOfPartials(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//      isCurrentDistilled
// ---------------------------------------------------------------------------
// Returns true if current sound are distilled
bool SoundList::isCurrentDistilled(){
  if(inList(current))
    return soundList.at(current)->isDistilled();
  else
    cout<<"SoundList::isCurrentDistilled(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//      isCurrentChannelized
// ---------------------------------------------------------------------------
// Returns true if current sound are channelized
bool SoundList::isCurrentChannelized(){
  if(inList(current))
    return soundList.at(current)->isChannelized();
  else
    cout<<"SoundList::isCurrentChannelized(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//      setCurrentSound
// ---------------------------------------------------------------------------
// change current
void SoundList::setCurrentSound(int pos){
  if(inList(pos)){
    current = pos; 
    emit currentChanged();
  }else
    cout<<"SoundList::setCurrentSound(int pos): pos not valid"<<endl;
}

// ---------------------------------------------------------------------------
//      getLength
// ---------------------------------------------------------------------------
// Returns the number of samples in the list.
int SoundList::getLength(){
  return soundList.count();
}

// ---------------------------------------------------------------------------
//      getMorph1Duration
// ---------------------------------------------------------------------------
// get the maximum time of the longest partial in first sound to be morphed
double SoundList::getMorph1Duration(){
  if(!isEmpty()){
    if(inList(morph1)){
      return soundList.at(morph1)->getDuration();
    } else {
      morph1 = 0;     // no morph1 specified so set to 0
      return soundList.at(morph1)->getDuration();
    }
  }
}

// ---------------------------------------------------------------------------
//      getMorph2Duration
// ---------------------------------------------------------------------------
// get the maximum time of the longest partial in second sound to be morphed
double SoundList::getMorph2Duration(){
  if(!isEmpty()){
    if(inList(morph2)){
      return soundList.at(morph2)->getDuration();
    } else {
      morph2 = 0;   // no morph2 specified so set to 0
      return soundList.at(morph2)->getDuration();
    }
  }
}

// ---------------------------------------------------------------------------
//	getCurrentMax
// ---------------------------------------------------------------------------
// get the maximum of amplitude/frequency/noise of the current sound
// depending on t
double SoundList::getCurrentMax(Sound::ValType t){
  if(inList(current)){
    return soundList.at(current)->getMax(t);
  } else
    cout<<"SoundList::getCurrentMax(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentDuration
// ---------------------------------------------------------------------------
// get the duration of current sound
double SoundList::getCurrentDuration(){
  if(inList(current)){
    return soundList.at(current)->getDuration();
  } else
    cout<<"SoundList::getCurrentDuration(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     shiftCurrentFrequency
// ---------------------------------------------------------------------------
// scale frequency of current sound with given parameter value.
void SoundList::shiftCurrentFrequency(double val){
  if(inList(current)){
    try{
      soundList.at(current)->shiftFrequency(val);
      emit currentChanged();
    }
    catch(...){
      cout<<"SoundList::shiftCurrentFrequency, caught any exception: "<<endl;
      throw;
    }
  } else
    cout<<"SoundList::shiftCurrentFrequency(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     shiftCurrentNoise
// ---------------------------------------------------------------------------
// scale noise of current sound with given parameter value.
void SoundList::shiftCurrentNoise(double val){
  if(inList(current)){
    try{
      soundList.at(current)->shiftNoise(val);
      emit currentChanged();
    }
    catch(...){
      cout<<"SoundList::shiftCurrentNoise, caught any exception: "<<endl;
      throw;
    }
  } else
    cout<<"SoundList::shiftCurrentNoise(): current not in list"<<endl;
}  

// ---------------------------------------------------------------------------
//     shiftCurrentAmplitude
// ---------------------------------------------------------------------------
// scale amplitude of current sound with given parameter value.
void SoundList::shiftCurrentAmplitude(double val){
  if(inList(current)){
    try{
      soundList.at(current)->shiftAmplitude(val);
      emit currentChanged();
    }
    catch(...){
      cout<<"SoundList::shiftCurrentAmplitude, caught any exception: "<<endl;
      throw;
    }
  } else
    cout<<"SoundList::shiftCurrentAmplitude(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//    getCurrentPartials
// ---------------------------------------------------------------------------
// Retrurns a list of Loris::Partial 
list<Partial>* SoundList::getCurrentPartials(){
  if(inList(current)){
    soundList.at(current)->getPartials();
  } else
    cout<<"SoundList::getCurrentPartials(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     channelizeCurrent
// ---------------------------------------------------------------------------
// Channelize current sound with given parameters, reference label, 
// minimum frequency, and maximum frequency
void SoundList::channelizeCurrent(
	int refLabel,
	double minFreq,
	double maxFreq
){
  if(inList(current)){
    soundList.at(current)->channelize(refLabel, minFreq, maxFreq);
    // model changed -> update views
    emit currentChanged();
  } else
    cout<<"SoundList::channelizeCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     distillCurrent
// ---------------------------------------------------------------------------
// Distill current sound 
void SoundList::distillCurrent(){
  if(inList(current)){
    soundList.at(current)->distill();
    emit currentChanged();
  } else
    cout<<"SoundList::distillCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     exportAiff
// ---------------------------------------------------------------------------
// Export current sound to aiff file format. 
void SoundList::exportAiff(
	double sampleRate,
	int sampleBits,
	const char* name
){
  if(inList(current))
    soundList.at(current)->exportToAiff(sampleRate, sampleBits, name);
}

// ---------------------------------------------------------------------------
//     exportSdif
// ---------------------------------------------------------------------------
// Export current sound to sdif file format. 
void SoundList::exportSdif(const char* name){
  if(inList(current))
    soundList.at(current)->exportToSdif(name);
  else
    cout<<"SoundList::exportCurrentToSdif(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     copyCurrent
// ---------------------------------------------------------------------------
// Copy current sound and insert the result into the list 
void SoundList::copyCurrent(){
  if(inList(current)){
    Sound* currentSound = soundList.at(current);
    Sound* soundCopy = new Sound(
	*currentSound->getPartials(),
	currentSound->getName(),
	interface
    );
    soundList.append(soundCopy);
    current ++;
    emit listChanged();  // model changed -> update views
    emit currentChanged();
  } else
    cout<<"SoundList::copyCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     renameCurrent
// ---------------------------------------------------------------------------
// Rename current sound  
void SoundList::renameCurrent(QString newName){
  if(inList(current)){
    soundList.at(current)->rename(newName);
    emit currentChanged();
    emit listChanged();          // model changed -> update views
  } else
    cout<<"SoundList::renameCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentName
// ---------------------------------------------------------------------------
// Return the name of current sound
QString SoundList::getCurrentName(){
  if(inList(current))
    return soundList.at(current)->getName();
  else
    return "";
}

// ---------------------------------------------------------------------------
//     playCurrent
// ---------------------------------------------------------------------------
// Plays current sound
void SoundList::playCurrent(){
  if(inList(current))
    soundList.at(current)->play();
  else
    cout<<"SoundList::playCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     inList
// ---------------------------------------------------------------------------
// Returns true if sound at given position are in the list
bool SoundList::inList(int pos){
  if(pos > -1 & pos < soundList.count()){
    return true;
  } else
    return false;
}
