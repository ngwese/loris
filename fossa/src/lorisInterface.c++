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
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "lorisInterface.h"

using namespace Loris;
using std::cout;
using std::list;
using std::vector;
using std::string;

/*
---------------------------------------------------------------------------
	LorisInterface constructor
---------------------------------------------------------------------------
*/
LorisInterface::LorisInterface(){}

/*
---------------------------------------------------------------------------
	importAiff
---------------------------------------------------------------------------
Import and analyze aiff file with given frequency resolution and window
width.
Consider combining importSdif and importAiff providing resolution and width 
with default parameters.  Check parameters to decide which format to 
import. 
*/
list<Partial>* LorisInterface::importAiff(
	const char*	path,
	double		resolution,
	double		width
){ 
  try{
    list<Partial>* sound = new list<Partial>;  // deleted in soundlist
    AiffFile file(path);
    vector<double> sampleVector(file.numFrames());
    sampleVector = file.samples();

    Analyzer analyzer(width);               // according to Kelly Fitz
    analyzer.setFreqResolution(resolution); // loris version 8 has 
    analyzer.setFreqFloor(resolution);      // constructor for this.
    analyzer.setFreqDrift(resolution);

    analyzer.analyze(sampleVector, file.sampleRate());
    sound->splice(sound->end(), analyzer.partials());
    return sound;
  }

  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	importSdif
---------------------------------------------------------------------------
Import sdif file 
Concider combining importSdif and importAiff providing resolution and width 
with default parameters.  Check parameters to decide which format to 
import. 
*/
list<Partial>* LorisInterface::importSdif(const char* path){ 
  try{
    SdifFile* file = new SdifFile(string(path));
    list<Partial>* sound = new list<Partial>;
    sound->splice(sound->end(), file->partials());
    return sound;
  }

  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	channelize
---------------------------------------------------------------------------
Channelize list of Partial with given frequency label, 
minimum frequency, and mximum frequency.
*/
void LorisInterface::channelize(
	int		refLabel,
	double		minFreq,
	double		maxFreq,
	list<Partial>&	sound
){ 
  try{
    list<Partial>::const_iterator begin = sound.begin();
    list<Partial>::const_iterator end   = sound.end();
    FrequencyReference freRef(begin, end, minFreq, maxFreq);
    LinearEnvelope referenceEnvelope = freRef.envelope();
    Channelizer channelizer(referenceEnvelope, refLabel);
    channelizer.channelize(sound.begin(), sound.end());
  }
  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	distill
---------------------------------------------------------------------------
Distill list of Partial
*/
void LorisInterface::distill(list<Partial>& sound){
  try{
    Distiller distiller;
    distiller.distill(sound);
  }

  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	dilate
---------------------------------------------------------------------------
Dilate one sound onto timepoints specified relative to another.
*/
void LorisInterface::dilate(
	list<Partial>* sound,
	list<double>* source,
	list<double>* target
){
  try{
    Loris::Dilator::dilate(
	sound->begin(),
	sound->end(),
	source->begin(),
	source->end(),
	target->begin()
    );
  }catch(...){ throw; }
}


/*
---------------------------------------------------------------------------
	morph
---------------------------------------------------------------------------
Morph 2 lists of Partial with given frequency, amplitude, and noise
envelopes.
*/
list<Partial>* LorisInterface::morph(
	LinearEnvelope&		famp,
	LinearEnvelope&		ffreq,
	LinearEnvelope&		fbw,
	list<Partial>		sound1,
	list<Partial>		sound2
){
  try{
    Morpher morpher(ffreq, famp, fbw);
    morpher.morph(
	sound1.begin(),
	sound1.end(),
	sound2.begin(),
	sound2.end()
    );
    
    // is deleted in soundList;
    list<Partial>* morphResult = new list<Partial>;
    morphResult->splice(morphResult->end(), morpher.partials());
    return morphResult;
  }
  
  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	exportAiff
---------------------------------------------------------------------------
Export list of Partial to aiff file format
*/
void LorisInterface::exportAiff(
	double sampleRate,
	int bitsPerSample,
	const char* name,
	list<Partial> sound,
	double maxtime
){
  try{
    //Have to ensure a long enough buffer for synthesized sound
    //10ms is more than enough
    const double Padding = .01;
    double time = ((maxtime + Padding) * sampleRate);
    vector<double> sampleVector = vector<double>(time);
    Synthesizer synthesizer(sampleRate, sampleVector);
    AiffFile file(sound.begin(), sound.end(), time);
   
    // Loris can only synthesise mono files (channels = 1)
    /*AiffFile::Export(
	name,
	sampleRate,
	1,
	bitsPerSample,
	sampleVector.begin(),
	sampleVector.end()
    );*/
    file.write(name, bitsPerSample);
  }
  
  catch(...){
    throw;
  }
}

/*
---------------------------------------------------------------------------
	exportSdif
---------------------------------------------------------------------------
Export list of Partial to sdif file format
*/
void LorisInterface::exportSdif(const char* name, list<Partial> sound){
  try{
    SdifFile::Export(name, sound);
  }
  
  catch(...){
    throw;
  }
}
