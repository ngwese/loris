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
 * partialsList.c++
 *
 * The PartialsList class is the main model of the application, ie. changes in 
 * PartialsList will trigger updates in view classes. This class has a list
 * containing imported partials and partials produced from different 
 * manipulations. One partials in the list is always set to current partials, 
 * and modifications can be made only to current partials. 
 * PartialsList also keeps track of two partials which are selected when 
 * the user decides to make a morph between two sounds. The class 
 * communicates with LorisInterface for necessary operations on partials.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */


#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "partialsList.h"

using std::cout;

// ---------------------------------------------------------------------------
//	PartialsList constructor
// ---------------------------------------------------------------------------
PartialsList::PartialsList(){
  partialsList.setAutoDelete(TRUE);  // QList will delete objects automatically
  morph1    = -1;
  morph2    = -1;
  current   = -1;
  interface = new LorisInterface();
}

// ---------------------------------------------------------------------------
//	PartialsList destructor
// ---------------------------------------------------------------------------
PartialsList::~PartialsList(){
  delete interface;
}

// ---------------------------------------------------------------------------
//	getPartials
// ---------------------------------------------------------------------------
const Partials* PartialsList::getPartials(int pos){
  return partialsList.at(pos);
}

// ---------------------------------------------------------------------------
//      importAiff
// ---------------------------------------------------------------------------
// Communicates with lorisInterface in order to import and analyse a sound file
// with given frequency resolution and window width.
// Consider combining importAiff and importSdif since they are so similar.  
// By providing resolution and width with default parameters a check of 
// parameters can be made in order to figure out what format to import.
void PartialsList::importAiff(
	QString		path,
	QString		name,
	double		resolution,
	double		width
){
  try{
    //take away ending, what if no ending????
    name.remove(name.length()-5, name.length());
    list<Loris::Partial>* p = interface->importAiff(path, resolution, width); 
    Partials* newPartials = new Partials(*p, name, interface); 
    partialsList.append(newPartials);
    
    Partials::State state; 
    state   = getCurrentState();
    current = partialsList.count()-1;  
    setCurrentState(state);   // we want to start at same state as last current
    emit listChanged();    // model changed -> update views
  }

  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      importSdif
// ---------------------------------------------------------------------------
// Communicates with lorisInterface in order to import a partials file.
// Consider combining importAiff and importSdif since they are so similar. 
// By providing resolution and width with default parameters a check 
// of parameters can be made in order to figure out what format to import.
void PartialsList::importSdif(
	QString		path,
	QString		name
){
  try{
    // take away ending, what if no ending????
    name.remove(name.length()-5, name.length());
    list<Loris::Partial>* p = interface->importSdif(path);
    Partials* newPartials = new Partials(*p, name, interface);
    partialsList.append(newPartials);
    
    Partials::State state; 
    state   = getCurrentState();
    current =  partialsList.count()-1;
    setCurrentState(state);  // we want to start at same state as last current
    
    emit listChanged();  // model changed -> update views
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      morph
// ---------------------------------------------------------------------------
// Morph 2 partials from the list with given breakpointsenvelopes for 
// amplitude, frequency, and noise.
void PartialsList::morph(
	LinerEnvelope&		famp,
	LinerEnvelope&		ffreq,
	LinerEnvelope&		fbw
){
  list<Loris::Partial> list1 = *partialsList.at(morph1)->getPartials();
  list<Loris::Partial> list2 = *partialsList.at(morph2)->getPartials();
  QString name1       = partialsList.at(morph1)->getName();
  QString name2       = partialsList.at(morph2)->getName();
  
  try{
    list<Loris::Partial>* morphedPartials = interface->morph(
	famp,
	ffreq,
	fbw,
	list1,
	list2
    );
    QString morphedPartialsName = name1 + name2;
    partialsList.append(
	new Partials(
		*morphedPartials,
		morphedPartialsName,
		interface
	)
    );

    Partials::State state; 
    state   = getCurrentState();
    current =  partialsList.count()-1;
    setCurrentState(state);
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
// Remove current partials from the list
void PartialsList::removeCurrent(){
  if(inList(current)){
    partialsList.remove(current);
    if(current == 0 && !isEmpty()){  // if current was the first item
      current = 0;                   // but not last.
    }
    else{
      current --;                    
    }
    emit listChanged();
    emit currentChanged();       // model changed -> update views
  }
  else{
    cout<<"PartialsList::removeCurrent(): current not in range, weird"<<endl;
  }
}

// ---------------------------------------------------------------------------
//      isEmpty
// ---------------------------------------------------------------------------
// Returns true if partialsList is empty.
bool PartialsList::isEmpty(){
  return !inList(current);
}

// ---------------------------------------------------------------------------
//      getCurrentIndex
// ---------------------------------------------------------------------------
// Returns the position of current partials. 
int PartialsList::getCurrentIndex(){
  return current;
}

// ---------------------------------------------------------------------------
//      getCurrentNrOfPartials
// ---------------------------------------------------------------------------
// Returns number of partials in current partials 
int PartialsList::getCurrentNrOfPartials(){
  if(inList(current)){
    return partialsList.at(current)->getNumberOfPartials();
  }
  else{
    cout<<"PartialsList::getCurrentNrOfPartials(): current not in list"<<endl;
  }
}

// ---------------------------------------------------------------------------
//      isCurrentDistilled
// ---------------------------------------------------------------------------
// Returns true if current partials are distilled
bool PartialsList::isCurrentDistilled(){
  if(inList(current)){
    return partialsList.at(current)->isDistilled();
  }
  else{
    cout<<"PartialsList::isCurrentDistilled(): current not in list"<<endl;
  }
}

// ---------------------------------------------------------------------------
//      isCurrentChannelized
// ---------------------------------------------------------------------------
// Returns true if current partials are channelized
bool PartialsList::isCurrentChannelized(){
  if(inList(current)){
    return partialsList.at(current)->isChannelized();
  }
  else{
    cout<<"PartialsList::isCurrentChannelized(): current not in list"<<endl;
  }
}

// ---------------------------------------------------------------------------
//      setCurrentPartials
// ---------------------------------------------------------------------------
// change current
void PartialsList::setCurrentPartials(int pos){
  if(inList(pos)){
    Partials::State state;
    if(inList(current)){
      state = partialsList.at(current)->getState(); 
    }
    else{
      state = Partials::amplitude;
    }
    current = pos; 
    setCurrentState(state); // want to keep state, when user clicks on new 
    emit currentChanged();  // partials
  }
  else cout<<"PartialsList::setCurrentPartials(int pos): pos not valid"<<endl;
}

// ---------------------------------------------------------------------------
//      getLength
// ---------------------------------------------------------------------------
// Returns the number of samples in the list.
int PartialsList::getLength(){
  return partialsList.count();
}

// ---------------------------------------------------------------------------
//      setMorphPartials1
// ---------------------------------------------------------------------------
// set first partials to be morphed with second morph partials. This occurs
// when the user selects a collection of partials from listboxes in the
// morphDialog.
void PartialsList::setMorphPartials1(int pos){
  if(inList(pos)){
    morph1 = pos;
    QString text = partialsList.at(morph1)->getName();
  }
  else cout<<"PartialsList::setMorphPartials1(): pos not in list"<<endl;
}

// ---------------------------------------------------------------------------
//      setMorphPartials2
// ---------------------------------------------------------------------------
// set second partials to be morphed with first morph partials. This occurs
// when the user selects a collection of partials from listboxes in the
// morphDialog
void PartialsList::setMorphPartials2(int pos){
  if(inList(pos)){
    morph2 = pos;
    QString text = partialsList.at(morph2)->getName();
  }
  else cout<<"PartialsList::setMorphPartials2(): pos not in list"<<endl;
}

// ---------------------------------------------------------------------------
//      getMorph1Duration
// ---------------------------------------------------------------------------
// get the maximum time of the longest partial in first partials to be morphed
double PartialsList::getMorph1Duration(){
  if(!isEmpty()){
    if(inList(morph1)){
      return partialsList.at(morph1)->getDuration();
    }else{
      morph1 = 0;     // no morph1 specified so set to 0
      return partialsList.at(morph1)->getDuration();
    }
  }
}

// ---------------------------------------------------------------------------
//      getMorph2Duration
// ---------------------------------------------------------------------------
// get the maximum time of the longest partial in second partials to be morphed
double PartialsList::getMorph2Duration(){
  if(!isEmpty()){
    if(inList(morph2)){
      return partialsList.at(morph2)->getDuration();
    }else{
      morph2 = 0;   // no morph2 specified so set to 0
      return partialsList.at(morph2)->getDuration();
    }
  }
}

// ---------------------------------------------------------------------------
//      setCurrentState
// ---------------------------------------------------------------------------
// Set the state of current partials (amplitude, frequency, or noise)
void PartialsList::setCurrentState(Partials::State s){
  if(inList(current)){
    partialsList.at(current)->setState(s);
    emit currentChanged();
  }
}

// ---------------------------------------------------------------------------
//     getCurrentMaxAmplitude
// ---------------------------------------------------------------------------
// get the maximum amplitude of current partials
double PartialsList::getCurrentMaxAmplitude(){
  if(inList(current)){
    partialsList.at(current)->getMaxAmplitude();
  }
  else cout<<"PartialsList::getCurrentMaxAmplitude(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentMaxFrequency
// ---------------------------------------------------------------------------
// get the maximum frequency of current partials
double PartialsList::getCurrentMaxFrequency(){
  if(inList(current)){
    partialsList.at(current)->getMaxFrequency();
  }
  else cout<<"PartialsList::getCurrentMaxFrequency(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentMaxNoise
// ---------------------------------------------------------------------------
// get the maximum noise of current partials
double PartialsList::getCurrentMaxNoise(){
  if(inList(current)){
    partialsList.at(current)->getMaxNoise();
  }
  else cout<<"PartialsList::getCurrentMaxNoise(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentDuration
// ---------------------------------------------------------------------------
// get the duration of current partials
double PartialsList::getCurrentDuration(){
  if(inList(current)){
    return partialsList.at(current)->getDuration();
  }
  else cout<<"PartialsList::getCurrentDuration(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentState
// ---------------------------------------------------------------------------
// get the state of current partials (amplitude, frequency, or noise)
Partials::State PartialsList::getCurrentState(){
  if(inList(current)){
    return partialsList.at(current)->getState();
  }
  else {
    return Partials::frequency;
  }
}  

// ---------------------------------------------------------------------------
//     shiftCurrentFrequency
// ---------------------------------------------------------------------------
// scale frequency of current partials with given parameter value.
void PartialsList::shiftCurrentFrequency(int val){
  if(inList(current)){
    try{
      partialsList.at(current)->shiftFrequency(val);
      emit currentChanged();
    }
    catch(...){
      cout<<"PartialsList::shiftCurrentFrequency, caught any exception: "<<endl;
      throw;
    }
  }
  else cout<<"PartialsList::shiftCurrentFrequency(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     shiftCurrentNoise
// ---------------------------------------------------------------------------
// scale noise of current partials with given parameter value.
void PartialsList::shiftCurrentNoise(int val){
  if(inList(current)){
    try{
    partialsList.at(current)->shiftNoise(val);
    emit currentChanged();
    }
    catch(...){
      cout<<"PartialsList::shiftCurrentNoise, caught any exception: "<<endl;
      throw;
    }
  } 
  else cout<<"PartialsList::shiftCurrentNoise(): current not in list"<<endl;
}  

// ---------------------------------------------------------------------------
//     shiftCurrentAmplitude
// ---------------------------------------------------------------------------
// scale amplitude of current partials with given parameter value.
void PartialsList::shiftCurrentAmplitude(int val){
  if(inList(current)){
    try{
    partialsList.at(current)->shiftAmplitude(val);
    emit currentChanged();
    }
    catch(...){
      cout<<"PartialsList::shiftCurrentAmplitude, caught any exception: "<<endl;
      throw;
    }
  }
  else cout<<"PartialsList::shiftCurrentAmplitude(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     shiftCurrentAmplitude
// ---------------------------------------------------------------------------
// Retrurns a list of Loris::Partial 
list<Partial>* PartialsList::getCurrentPartials(){
  if(inList(current)){
    partialsList.at(current)->getPartials();
  }
  else cout<<"PartialsList::getCurrentPartials(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentAmplitudePixmap
// ---------------------------------------------------------------------------
// Returns the amplitude plot of current partials
QPixmap PartialsList::getCurrentAmplitudePixmap(){
  if(inList(current)){
    return partialsList.at(current)->getAmplitudePixmap();
  }
  else {
    return EmptyPixmap(0, 800, 450);
  }
}

// ---------------------------------------------------------------------------
//     getCurrentFrequencyPixmap
// ---------------------------------------------------------------------------
// Returns the frequency plot of current partials
QPixmap PartialsList::getCurrentFrequencyPixmap(){
  if(inList(current)){
    return partialsList.at(current)->getFrequencyPixmap();
  }
  else {
    return EmptyPixmap(0, 800, 450);
  }
}

// ---------------------------------------------------------------------------
//     getCurrentNoisePixmap
// ---------------------------------------------------------------------------
// Returns the noise plot of current partials
QPixmap PartialsList::getCurrentNoisePixmap(){
  if(inList(current)){
    return partialsList.at(current)->getNoisePixmap();
  }
   else {
    return EmptyPixmap(0, 800, 450);
  }
}

// ---------------------------------------------------------------------------
//     channelizeCurrent
// ---------------------------------------------------------------------------
// Channelize current partials with given parameters, reference label, 
// minimum frequency, and maximum frequency
void PartialsList::channelizeCurrent(
	int refLabel,
	double minFreq,
	double maxFreq
){
  if(inList(current)){
    partialsList.at(current)->channelize(refLabel, minFreq, maxFreq);
    // model changed -> update views
    emit currentChanged();
  }
  else cout<<"PartialsList::channelizeCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     distillCurrent
// ---------------------------------------------------------------------------
// Distill current partials 
void PartialsList::distillCurrent(){
  if(inList(current)){
    partialsList.at(current)->distill();
    emit currentChanged();
  }
  else cout<<"PartialsList::distillCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     exportAiff
// ---------------------------------------------------------------------------
// Export current partials to aiff file format. 
void PartialsList::exportAiff(
	double sampleRate,
	int sampleBits,
	const char* name
){
  if(inList(current))
    partialsList.at(current)->exportToAiff(sampleRate, sampleBits, name);
}

// ---------------------------------------------------------------------------
//     exportSdif
// ---------------------------------------------------------------------------
// Export current partials to sdif file format. 
void PartialsList::exportSdif(const char* name){
  if(inList(current))
    partialsList.at(current)->exportToSdif(name);

  else cout<<"PartialsList::exportCurrentToSdif(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     copyCurrent
// ---------------------------------------------------------------------------
// Copy current partials and insert the result into the list 
void PartialsList::copyCurrent(){
  if(inList(current)){
    Partials* currentPartials = partialsList.at(current);
    Partials* partialsCopy = new Partials(
	*currentPartials->getPartials(),
	currentPartials->getName(),
	interface
    );
    partialsList.append(partialsCopy);
    current ++;
    emit listChanged();  // model changed -> update views
    emit currentChanged();
  }
  else cout<<"PartialsList::copyCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     renameCurrent
// ---------------------------------------------------------------------------
// Rename current partials  
void PartialsList::renameCurrent(QString newName){
  if(inList(current)){
    partialsList.at(current)->rename(newName);
    emit currentChanged();
    emit listChanged();          // model changed -> update views
  }
  else cout<<"PartialsList::renameCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     getCurrentName
// ---------------------------------------------------------------------------
// Return the name of current partials
QString PartialsList::getCurrentName(){
  if(inList(current))
    return partialsList.at(current)->getName();
  else return "";
}

// ---------------------------------------------------------------------------
//     playCurrent
// ---------------------------------------------------------------------------
// Plays current partials
void PartialsList::playCurrent(){
  if(inList(current))
    partialsList.at(current)->play();
  else cout<<"PartialsList::playCurrent(): current not in list"<<endl;
}

// ---------------------------------------------------------------------------
//     inList
// ---------------------------------------------------------------------------
// Returns true if partials at given position are in the list
bool PartialsList::inList(int pos){
  if(pos > -1 & pos < partialsList.count()){
    return true;
  }
  else return false;
}
